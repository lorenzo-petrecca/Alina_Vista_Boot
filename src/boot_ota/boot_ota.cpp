#include "boot_ota.hpp"

#include <esp_ota_ops.h>
#include <esp_system.h>
#include <mbedtls/sha256.h>

#include "boot_http/boot_http.hpp"
#include "boot_display_driver/boot_display_driver.hpp"
#include "config.hpp"
#include "boot_nvs/boot_nvs.hpp"
#include "boot_ui/ui_commons.hpp"


static bool hexToNibble(char c, uint8_t &out) {
    if (c >= '0' && c <= '9') { out = (uint8_t)(c - '0'); return true; }
    if (c >= 'a' && c <= 'f') { out = (uint8_t)(c - 'a' + 10); return true; }
    if (c >= 'A' && c <= 'F') { out = (uint8_t)(c - 'A' + 10); return true; }
    return false;
}


static bool parseHex64ToBytes(const char *hex64, uint8_t out32[32]) {
    if (!hex64) return false;
    // deve essere esattamente 64 hex
    for (int i = 0; i < 64; i++) {
        if (hex64[i] == '\0') return false;
    }
    if (hex64[64] != '\0') {
        // se è più lunga, accettiamo comunque se i primi 64 sono validi
        // ma idealmente dovrebbe essere terminata.
    }

    for (int i = 0; i < 32; i++) {
        uint8_t hi, lo;
        if (!hexToNibble(hex64[i * 2], hi)) return false;
        if (!hexToNibble(hex64[i * 2 + 1], lo)) return false;
        out32[i] = (uint8_t)((hi << 4) | lo);
    }
    return true;
}



bool bootOtaInstallRemote(const RemoteVersion& v) {
    auto &lcd = display_getInstance();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Installo...");

    // 1. Trova la partizione di Luna (per etichetta)
    const esp_partition_t *app_part = esp_partition_find_first (
        ESP_PARTITION_TYPE_APP,
        ESP_PARTITION_SUBTYPE_ANY,
        APP_PARTITION_NAME
    );
    if (!app_part) {
        lcd.setCursor(0, 2);
        lcd.print("Fatal Error");
        lcd.setCursor(0, 3);
        lcd.print("Err. Partiz.");
        delay(2000);
        return false;
    }

    // 2. HTTP GET del bin (HTTPS)
    HTTPClient http;
    if (!bootHttpBegin(http, v.url)) {
        lcd.setCursor(0, 2);
        lcd.print("Err. URL");
        delay(2000);
        return false;
    }

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        lcd.setCursor(0, 2);
        lcd.print("Err. HTTP");
        bootHttpEnd(http);
        delay(2000);
        return false;
    }

    // Content length può essere -1 (chunked)
    int64_t contentLength = http.getSize();
    const bool lengthKnown = (contentLength > 0);

    // se length nota, controlla che la partizione sia abbastanza grande
    if (lengthKnown && (uint64_t)contentLength > app_part->size) {
        lcd.setCursor(0, 2);
        lcd.print("Err. Server");
        bootHttpEnd(http);
        delay(2500);
        return false;
    }

    // 3. esp_ota_begin
    esp_ota_handle_t ota_handle = 0;
    esp_err_t err = esp_ota_begin(app_part, lengthKnown ? contentLength : OTA_SIZE_UNKNOWN, &ota_handle);
    if (err != ESP_OK) {
        lcd.setCursor(0, 2);
        lcd.print("Err. OTA");
        bootHttpEnd(http);
        delay(2000);
        return false;
    }

    // 4. Setup SHA256 (solo se v.sha sembra valido)
    bool verifySha = false;
    uint8_t expectedSha[32] = {0};
    if (v.sha[0] != '\0') {
        // verifica veloce: se è lungo 64 hex allora lo abilitiamo
        if (strlen(v.sha) >= 64 && parseHex64ToBytes(v.sha, expectedSha)) {
            verifySha = true;
        }
    }

    mbedtls_sha256_context shaCtx;
    if (verifySha) {
        mbedtls_sha256_init(&shaCtx);
        mbedtls_sha256_starts_ret(&shaCtx, 0);
    }

    // 5. Stream + esp_ota_write
    WiFiClient &stream = http.getStream();
    uint8_t buf[BOOT_OTA_BUF_SIZE];
    int64_t written = 0;

    ProgressBar bar {
        written,
        0,
        lengthKnown ? contentLength : 1 // evita div/0 nel render
    };

    uint32_t lastDataMs = millis();

    while (http.connected()) {
        size_t avail = stream.available();

        if (avail == 0) {
            // timeout se non arrivano dati per troppo tempo
            if (millis() - lastDataMs > BOOT_OTA_IDLE_TIMEOUT_MS) {
                err = ESP_ERR_TIMEOUT;
                break;
            }
            delay(10);
            continue;
        }

        if (avail > sizeof(buf)) avail = sizeof(buf);

        int r = stream.readBytes(buf, avail);
        if (r <= 0) {
            if (!lengthKnown) break; // se length sconosciuta, un r==0 può voler dire "fine"
            err = ESP_FAIL;
            break;
        }

        lastDataMs = millis();

        // sha update
        if (verifySha) {
            mbedtls_sha256_update_ret(&shaCtx, buf, r);
        }

        err = esp_ota_write(ota_handle, buf, r);
        if (err != ESP_OK) break;

        written += r;

        // Barra di progresso
        bar.current = written;
        renderProgessBar(COLUMNS, PAGES, 3, &bar);
        
        if (lengthKnown && written >= contentLength) {
            break;
        }
    }

    bootHttpEnd(http);

    // 6. Verifica condizioni di fine download
    if (err != ESP_OK) {
        lcd.setCursor(0,5);
        lcd.print("OTA write err");
        esp_ota_end(ota_handle);
        delay(2000);
        return false;
    }

    if (lengthKnown && written != contentLength) {
        lcd.setCursor(0,5);
        lcd.print("OTA size err");
        esp_ota_end(ota_handle);
        delay(2000);
        return false;
    }


    // 7. SHA256 final check
    if (verifySha) {
        uint8_t gotSha[32];
        mbedtls_sha256_finish_ret(&shaCtx, gotSha);
        mbedtls_sha256_free(&shaCtx);

        if (memcmp(gotSha, expectedSha, 32) != 0) {
            lcd.setCursor(0, 5);
            lcd.print("SHA mismatch");
            esp_ota_end(ota_handle);
            delay(2500);
            return false;
        }
    }


    // 8. OTA end
    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        lcd.setCursor(0, 5);
        lcd.print("OTA end err.");
        delay(2000);
        return false;
    }


    // 9. Imposta partizione di boot (Luna)
    err = esp_ota_set_boot_partition(app_part);
    if (err != ESP_OK) {
        lcd.setCursor(0, 5);
        lcd.print("Set Boot err.");
        delay(2000);
        return false;
    }

    // 10. boot_mode = false in NVS (torna a lanciare Luna al prossimo boot)
    (void)bootNvsSetBootMode(false);

    
    // 11. Riavvia
    lcd.clear();
    lcd.setCursor(0,2);
    lcd.print("Rebooting...");
    delay(2000);

    esp_restart();
    return true;
}