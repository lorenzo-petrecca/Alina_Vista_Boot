#include "boot_versions.hpp"
#include "boot_wifi/boot_wifi.hpp"
#include "boot_http/boot_http.hpp"
#include <ArduinoJson.h>


bool bootFetchRemoteVersions (const char* manifestUrl, RemoteVersion* out, uint8_t maxEntries, uint8_t& outCount) {
    outCount = 0;

    if (!manifestUrl || !out || maxEntries == 0) return false;

    // Controllo connessione WiFi STA
    if (WiFi.status() != WL_CONNECTED || bootWifiGetState() != BootWifiState::STA_CONNECTED) {
        Serial.println(F("[BOOT] WiFi not connected, unable to download manifest."));
        return false;
    }

    Serial.print(F("[BOOT] Download manifest: \""));
    Serial.print(manifestUrl);
    Serial.println("\"");

    HTTPClient http;

    int code = bootHttpGet(http, manifestUrl);

    if (code != HTTP_CODE_OK) {
        Serial.print(F("[BOOT] http error: "));
        Serial.println(code);
        bootHttpEnd(http);
        return false;
    }

    // Payload
    String payload = http.getString();
    bootHttpEnd(http);

    if (payload.length() == 0) {
        Serial.println(F("[BOOT] Empty manifest payload."));
        return false;
    }

    // JSON parse
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, payload);

    if (err) {
        Serial.println(F("[BOOT] Json parse error"));
        return false;
    }

    JsonArray apps = doc["apps"].as<JsonArray>();
    if (apps.isNull()) {
        Serial.println("[BOOT] JSON Error: corrupted manifest");
        return false;
    }

    uint8_t count = 0;

    for (JsonObject appObj : apps) {
        const char* appId = appObj["id"] | nullptr;
        const char* appName = appObj["display_name"] | appId;

        if (!appId) continue;

        JsonArray versions = appObj["versions"].as<JsonArray>();
        if (versions.isNull()) continue;

        for (JsonObject v : versions) {
            if (count >= maxEntries) break;

            const char* verStr = v["version"] | nullptr;
            const char* urlStr = v["url"] | nullptr;
            const char* shaStr = v["sha256"] | "";
            const uint32_t build = v["build"] | 0;
            const char* minBootS = v["min_boot"] | "0.0.0";
            const uint32_t minBootB = v["min_boot_build"] | 0;

            if (!verStr || !urlStr) continue;   // voce malformata

            // Filtro compatibilità boot
            if (BOOT_VERSION_BUILD < minBootB) continue;

            // Creazione struttura RemoteVersion prendendo il riferimento della struttura
            // corrispondente ancora vuota dell'array RemoteVersion out passato alla funzione
            RemoteVersion &dst = out[count];

            // copia sicura dei campi
            strncpy(dst.appId, appId, sizeof(dst.appId));   // id
            dst.appId[sizeof(dst.appId)-1] = '\0';

            strncpy(dst.appName, appName, sizeof(dst.appName));   // display_name
            dst.appName[sizeof(dst.appName)-1] = '\0';

            strncpy(dst.version, verStr, sizeof(dst.version));   // version
            dst.version[sizeof(dst.version)-1] = '\0';

            dst.build = build;  // build

            strncpy(dst.url, urlStr, sizeof(dst.url));   // url
            dst.url[sizeof(dst.url)-1] = '\0';

            strncpy(dst.sha, shaStr, sizeof(dst.sha));   // sha256
            dst.sha[sizeof(dst.sha)-1] = '\0';

            dst.minBootBuild = minBootB;    // min_boot_build

            strncpy(dst.minBoot, minBootS, sizeof(dst.minBoot));   // min_boot
            dst.minBoot[sizeof(dst.minBoot)-1] = '\0';


            count++;
        }

        if (count >= maxEntries) break;
    }

    outCount = count;
    return (outCount > 0);
}