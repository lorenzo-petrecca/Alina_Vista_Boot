#include "ui_update_tab.hpp"
#include "boot_ui/ui_commons.hpp"
#include "boot_display_driver/boot_display_driver.hpp"
#include "font/mono_5x8px/data.h"
#include "font/mono_5x8px/meta.h"
#include "config.hpp"
#include "boot_nvs/boot_nvs.hpp"
#include "boot_versions/boot_versions.hpp"
#include "boot_ota/boot_ota.hpp"



// UpdateTab    =======================================================================================

// Info vers. —————————————————————————————————————————————————————————————————————————————————————
static void versInfoRender () {
    auto &lcd = display_getInstance();
    lcd.clear();
    delay(25);
    lcd.setCursor(0, 0);
    lcd.print("Firmware:");
    
    // ottenere nome app firmware
    size_t appNameStrLen = 8;
    char appName[appNameStrLen];
    bool foundName = bootNvsGetString(APP_NVS_NAMESPACE, NVS_KEY_APP_NAME, appName, appNameStrLen);

    // ottenere versione app firmware
    size_t appVersStrLen = 8;
    char appVers[appVersStrLen];
    bool foundVers = bootNvsGetString(APP_NVS_NAMESPACE, NVS_KEY_APP_VERSION, appVers, appVersStrLen);

    lcd.setCursor(0, 1);
    if (!foundName || !foundVers) {
        lcd.print(F("Versione non trovata"));
        return;
    }

    lcd.print(appName);
    lcd.print(" v");
    lcd.print(appVers);

    // ottenere autore app firmware
    size_t appAuthStrLen = 22;
    char appAuth[appAuthStrLen];
    bool foundAuth = bootNvsGetString(APP_NVS_NAMESPACE, NVS_KEY_APP_AUTH, appAuth, appAuthStrLen);
    lcd.setCursor(0, 3);
    lcd.print("Auth: ");
    if (!foundAuth) {
        lcd.print("---");
    } else {
        lcd.print(appAuth);
    }
    
    // ottenere data app firmware
    size_t appDateStrLen = 8;
    char appDate[appDateStrLen];
    bool foundDate = bootNvsGetString(APP_NVS_NAMESPACE, NVS_KEY_APP_DATE, appDate, appDateStrLen);
    lcd.setCursor(0, 5);
    lcd.print("Date: ");
    if (!foundDate) {
        lcd.print("---");
    } else {
        lcd.print(appDate);
    }

}

void onVersInfo () {
    MenuController::Action a;

    a.onRender  = versInfoRender;
    a.onLeft    = onBack;
    a.onRight   = onBack;
    a.onSelect  = onBack;

    auto &controller = mController_getInstance();
    controller.enterAction(a);
} 

// ———————————————————————————————————————————————————————————————————————————————————————————————

// Altre vers. —————————————————————————————————————————————————————————————————————————————————————

// dichiarazione array di versioni
static RemoteVersion g_releases[BOOT_MAX_REMOTE_VERSIONS];
static uint8_t g_releasesCount = 0;


// array figli del sottomenu "Altre vers."
static MenuItem g_ovItems[1 + BOOT_MAX_REMOTE_VERSIONS]; 
// [0] = "Indietro", [1..N] = versioni

// callback per installare versione selezionata
static void onInstallSelectedVers ();

// calback per popolare sotto-menu "Altre vers."
static void onOtherVersions ();

// ———————————————————————————————————————————————————————————————————————————————————————————————

// ——————————————————————————————————————————————————————————————————————————————————————————————————
static const MenuItem BackItem("Indietro", onBack, nullptr, 0);             // Voce: Indietro
// Inizialmente non assegniamo ad Altre vers. un array di figli
// verrà riempito a runtime dalla callback
static MenuItem OtherVersItem("Altre vers.", onOtherVersions, nullptr, 0);  // Voce: Altre vers.
static const MenuItem VersInfoItem("Info vers.", onVersInfo, nullptr, 0);   // Voce: Info vers.


// array delle voci di "WiFi tab"
static MenuItem updateTabs[] = {
    BackItem,
    OtherVersItem,
    VersInfoItem,
};


// array labels versioni
static char g_ovLabels[BOOT_MAX_REMOTE_VERSIONS][14];

static void onOtherVersions () {
    auto &lcd = display_getInstance();

    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Caricamento...");

    // 1. Fetch del manifest
    bool ok = bootFetchRemoteVersions (
        APP_MANIFEST_URL,
        g_releases,
        BOOT_MAX_REMOTE_VERSIONS,
        g_releasesCount
    );

    // 2. Prima voce del submenu "Indietro"
    g_ovItems[0] = MenuItem("Indietro", onBack, nullptr, 0);

    uint8_t used = 1;

    if (ok && g_releasesCount != 0) {
        for (uint8_t i = 0; i < g_releasesCount; i++) {
            snprintf(g_ovLabels[i], sizeof(g_ovLabels[i]),
                     "%s %s",
                     g_releases[i].appName,
                     g_releases[i].version);

            g_ovItems[i+1] = MenuItem(
                g_ovLabels[i],
                onInstallSelectedVers,
                nullptr,
                0
            );
            used++;
        }
    }

    // 3. aggancia i figli all'item "Altre vers." del menu "Installa"
    updateTabs[1].children      = g_ovItems;
    updateTabs[1].childCount    = used;
    updateTabs[1].label         = "Installa vers.";
}

// ———————————————————————————————————————————————————————————————————————————————————————————————————

// Dichiarazione del sotto menu Install -> Indietro | Altre vers. | Info vers.
const MenuItem UpdateTab("Installa", nullptr, updateTabs, sizeof(updateTabs) / sizeof(updateTabs[0]));

// ===================================================================================================



// OtherVersionsTab =======================================================================================

static void onInstallSelectedVers () {
    auto &menu = mController_getInstance();
    uint8_t cursor = menu.getCursor();

    if (cursor == 0) return; // "Indietro" (gestito da onBack)

    uint8_t idx = cursor - 1;
    if (idx >= g_releasesCount) return;

    const RemoteVersion &sv = g_releases[idx];

    // Esegue aggiornamento OTA. Se va a buon fine riavvia con boot su partizione app
    bootOtaInstallRemote(sv);

    // Esce dalla schermata qual'ora non sia andata a buon fine
    exitAction();    
}

// ———————————————————————————————————————————————————————————————————————————————————————————————————

// ===================================================================================================