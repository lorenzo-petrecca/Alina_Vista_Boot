#pragma once

#include <stdint.h>

static constexpr const char* BOOT_NAME              = "Alina-Vista Boot";
static constexpr const char* BOOT_VERSION           = "0.1.0";
static constexpr const uint32_t BOOT_VERSION_BUILD  = 100;
static constexpr const char* BOOT_AUTH              = "Lorenzo Petrecca";
static constexpr const char* BOOT_DATE              = "2025/12";

static constexpr const char* APP_PARTITION_NAME = "luna";

static constexpr const char* BOOT_NVS_NAMESPACE = "boot";
static constexpr const char* KEY_BOOT_MODE      = "boot_mode";   // bool: false = avvia Luna, true = resta in Boot

static constexpr const char* APP_NVS_NAMESPACE      = "luna-meta";      // namespace firmware app
static constexpr const char* NVS_KEY_APP_NAME       = "firm_name";      // key nvs app firmware name
static constexpr const char* NVS_KEY_APP_VERSION    = "firm_vers";      // key nvs app firmware version
static constexpr const char* NVS_KEY_APP_BUILD      = "firm_build";     // key nvs app firmware build
static constexpr const char* NVS_KEY_APP_AUTH       = "firm_auth";      // key nvs app firmware author
static constexpr const char* NVS_KEY_APP_DATE       = "firm_date";      // key nvs app firmware date

// WiFi
static constexpr const char* WIFI_AP_SSID           = "Alina-setup";
static constexpr const char* WIFI_AP_PSW            = "alinaboot";
static constexpr const uint32_t WIFI_CONN_TIMEOUT   = 10000;
static constexpr const char* WIFI_NVS_NAMESPACE     = "wifi";
static constexpr const char* WIFI_KEY_SSID          = "ssid";
static constexpr const char* WIFI_KEY_PSW           = "password";


// OTA
static constexpr uint8_t BOOT_MAX_REMOTE_VERSIONS = 7;  // Massimo numero di versioni listate nel menu

#if defined(FLASH_16MB)
    static constexpr const char* APP_MANIFEST_URL = "https://alina-lab.vercel.app/api/firmware?flash=16mb";
#elif defined(FLASH_8MB)
    static constexpr const char* APP_MANIFEST_URL = "https://alina-lab.vercel.app/api/firmware?flash=8mb";
#elif defined(FLASH_4MB)
    static constexpr const char* APP_MANIFEST_URL = "https://alina-lab.vercel.app/api/firmware?flash=4mb";
#else
    #error "FLASH SIZE NOT DEFINED! Define FLASH_4MB / FLASH_8MB / FLASH_16MB in platformio.ini"
#endif

static constexpr uint32_t BOOT_OTA_BUF_SIZE = 1024; // Dimensione buffer per OTA
static constexpr uint32_t BOOT_OTA_IDLE_TIMEOUT_MS = 20000; // Timeout per OTA (20s)