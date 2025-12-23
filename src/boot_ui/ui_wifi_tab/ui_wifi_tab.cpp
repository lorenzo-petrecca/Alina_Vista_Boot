#include "ui_wifi_tab.hpp"
#include "boot_ui/ui_commons.hpp"
#include "boot_display_driver/boot_display_driver.hpp"
#include "font/mono_5x8px/data.h"
#include "font/mono_5x8px/meta.h"
#include "boot_wifi/boot_wifi.hpp"
#include "config.hpp"


// ACTION TABS
void connectRender () {
    auto &lcd = display_getInstance();
    lcd.clear();
    const char* connecting = "Connessione...";
    const uint8_t xConnecting = calcAbscissaToCenter((uint8_t)strlen(connecting), COLUMNS, GLYPH_WIDTH, GLYPH_SPACING);
    lcd.setCursor(xConnecting, 0);
    lcd.print(connecting);
    delay(100);

    // Tentativo di connessione
    bool ok = bootWifiConnectFromNvs();

    lcd.clear();
    
    if (ok) {
        lcd.setCursor(0, 0);
        lcd.print("Wi-Fi Connesso");
        renderExitNotice(5);
    } else {
        const char* en1 = "Errore di";
        const uint8_t xen1 = calcAbscissaToCenter((uint8_t)strlen(en1), COLUMNS, GLYPH_WIDTH, GLYPH_SPACING);
        const char* en2 = "connessione";
        const uint8_t xen2 = calcAbscissaToCenter((uint8_t)strlen(en2), COLUMNS, GLYPH_WIDTH, GLYPH_SPACING);
        lcd.setCursor(xen1, 0); lcd.print(en1);
        lcd.setCursor(xen2, 1); lcd.print(en2);
        renderExitNotice(5);
    }
}

void onConnect () {
    MenuController::Action a;
    a.onLeft = exitAction;
    a.onRight = exitAction;
    a.onSelect = exitAction;
    a.onRender = connectRender;
    auto &controller = mController_getInstance();
    controller.enterAction(a);
}



void configRender () {
    auto &lcd = display_getInstance();
    lcd.clear();

    // Stampa Avvio AP...
    const char* connecting = "Avvio AP...";
    const uint8_t xconn = calcAbscissaToCenter((uint8_t)strlen(connecting), COLUMNS, GLYPH_WIDTH, GLYPH_SPACING);
    lcd.setCursor(xconn, 0);
    lcd.print(connecting);

    // Tentativo di creazione Access Point
    if (bootWifiStartAp(WIFI_AP_SSID, WIFI_AP_PSW)) {
        lcd.clear();
        // Stampa AP riuscito
        const char* apOk = "AP attivo";
        const uint8_t xapOk = calcAbscissaToCenter((uint8_t)strlen(apOk), COLUMNS, GLYPH_WIDTH, GLYPH_SPACING);
        lcd.setCursor(xapOk, 0);
        lcd.print(apOk);

        // stampa istruzioni AP
        lcd.setCursor(0, 1); lcd.print("Connettersi a:");
        lcd.setCursor(0, 2); lcd.print(WIFI_AP_SSID);
        lcd.setCursor(0, 3); lcd.print("Psw: ");
        lcd.print(WIFI_AP_PSW);
        
        // stampa exit notice
        renderExitNotice(5);
    } else {
        lcd.clear();
        // Stampa AP fallito
        const char* apErr = "AP error";
        const uint8_t xapErr = calcAbscissaToCenter((uint8_t)strlen(apErr), COLUMNS, GLYPH_WIDTH, GLYPH_SPACING);
        lcd.setCursor(xapErr, 0);
        lcd.print(apErr);
        lcd.setCursor(0, 1);
        lcd.print("Non e' stato");
        lcd.setCursor(0, 2);
        lcd.print("possibile");
        lcd.setCursor(0, 3);
        lcd.print("avviare AP");
        
        // stampa exit notice
        renderExitNotice(5);
    }
}

void onConfig () {
    MenuController::Action a;
    a.onLeft = exitAction;
    a.onRight = exitAction;
    a.onSelect = exitAction;
    a.onRender = configRender;
    auto &controller = mController_getInstance();
    controller.enterAction(a);
} 



// ROOT    ——————————————————————————————————————————————————————————————————————————
static const MenuItem BackItem("Indietro", onBack, nullptr, 0);                 // Voce: Indietro
static const MenuItem ConnectItem("Connetti", onConnect, nullptr, 0);                 // Voce: Indietro
static const MenuItem ConfItem("Configura", onConfig, nullptr, 0);                 // Voce: Indietro


// array delle voci di "WiFi tab"
static const MenuItem wiFiTabs[] = {
    BackItem,
    ConnectItem,
    ConfItem,
};

// ———————————————————————————————————————————————————————————————————————————————————————————————————

// Dichiarazione del sotto menu
const MenuItem WiFiConfTab("WiFi", nullptr, wiFiTabs, sizeof(wiFiTabs) / sizeof(wiFiTabs[0]));
