#include "boot_ota.hpp"
#include <esp_ota_ops.h>
#include <esp_system.h>
#include <HTTPClient.h>
#include "boot_display_driver/boot_display_driver.hpp"
#include "config.hpp"
#include "boot_nvs/boot_nvs.hpp"
#include "boot_ui/ui_commons.hpp"

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

    // 2. HTTP GET del bin
    HTTPClient http;
    if (!http.begin(v.url)) {
        lcd.setCursor(0, 2);
        lcd.print("Err. URL");
        delay(2000);
        return false;
    }

    int httpCode = http.GET();
    if (httpCode != HTTP_CODE_OK) {
        lcd.setCursor(0, 2);
        lcd.print("Err. HTTP");
        http.end();
        delay(2000);
        return false;
    }

    int64_t contentLength = http.getSize();
    if (contentLength <= 0) {
        lcd.setCursor(0, 2);
        lcd.print("Err. Server");
        http.end();
        delay(2000);
        return false;
    }

    // 3. esp_ota_begin
    esp_ota_handle_t ota_handle;
    esp_err_t err = esp_ota_begin(app_part, contentLength, &ota_handle);
    if (err != ESP_OK) {
        lcd.setCursor(0, 2);
        lcd.print("Err. OTA");
        http.end();
        delay(2000);
        return false;
    }

    // 4. Stream + esp_ota_writ
    WiFiClient &stream = http.getStream();
    uint8_t buf[1024];
    int64_t written = 0;

    ProgressBar bar {
        written,
        0,
        contentLength
    };

    while (http.connected() && written < contentLength) {
        size_t avail = stream.available();
        if (!avail) {
            delay(10);
            continue;
        }

        if (avail > sizeof(buf)) avail = sizeof(buf);
        int r = stream.readBytes(buf, avail);
        if (r <= 0) {
            err = ESP_FAIL;
            break;
        }

        err = esp_ota_write(ota_handle, buf, r);
        if (err != ESP_OK) break;

        written += r;

        // Barra di progresso
        bar.current = written;
        renderProgessBar(COLUMNS, PAGES, 3, &bar);
        
    }

    http.end();

    if (err != ESP_OK || written != contentLength) {
        lcd.setCursor(0,5);
        lcd.print("OTA write err");
        esp_ota_end(ota_handle);
        delay(2000);
        return false;
    }

    err = esp_ota_end(ota_handle);
    if (err != ESP_OK) {
        lcd.setCursor(0, 5);
        lcd.print("OTA end err.");
        delay(2000);
        return false;
    }

    // 5. Imposta partizione di boot
    err = esp_ota_set_boot_partition(app_part);
    if (err != ESP_OK) {
        lcd.setCursor(0, 5);
        lcd.print("Set Boot err.");
        delay(2000);
        return false;
    }

    // 6. boot_mode = false in NVS (torna a lanciare Luna al prossimo boot)
    bool okFlag = bootNvsSetBootMode(false);

    lcd.clear();
    lcd.setCursor(0,2);
    lcd.print("Rebooting...");
    delay(2000);

    // 7. Riavvia
    esp_restart();
    return true;
}