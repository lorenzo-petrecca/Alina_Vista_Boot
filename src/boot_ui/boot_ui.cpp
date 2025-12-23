#include "boot_ui.hpp"
#include "ui_commons.hpp"
#include "config.hpp"
#include "pinmap.hpp"

#include "ui_wifi_tab/ui_wifi_tab.hpp"
#include "ui_update_tab/ui_update_tab.hpp"


// ACTION TABS
void onStartApp () {

}

void bootInfoRender () {
    auto &lcd = display_getInstance();
    lcd.clear();
    delay(25);
    lcd.setCursor(0,0);
    lcd.print("Boot:");
    lcd.setCursor(0,1);
    lcd.print(BOOT_NAME);
    lcd.print(" ");
    lcd.print(BOOT_VERSION);
    //lcd.print(F("Alina Vista Boot v0.1.0"));
    lcd.setCursor(0,3);
    lcd.print("Auth: ");
    lcd.print(BOOT_AUTH);
    lcd.setCursor(0,5);
    lcd.print("Date: ");
    lcd.print(BOOT_DATE);
}

void onBootInfo () {
    MenuController::Action a;
    a.onRender = bootInfoRender;
    a.onSelect = exitAction;

    auto &controller = mController_getInstance();
    controller.enterAction(a);
} 

void onPowerOff () {
    digitalWrite(HOLD_ON, LOW);
}


// BOOT TAGS    ——————————————————————————————————————————————————————————————————————————
static const MenuItem StartAppItem("Avvia app", onStartApp, nullptr, 0);     // Voce: Avvia app
static const MenuItem WiFiConfItem = WiFiConfTab;                           // Voce: WiFi
static const MenuItem UpdateItem = UpdateTab;                               // Voce: Installa
static const MenuItem BootInfoItem("Info", onBootInfo, nullptr, 0);         // Voce: Info
static const MenuItem PowerOffItem("Spegni", onPowerOff, nullptr, 0);       // Voce: Spegni

// array delle voci di "Root"
static const MenuItem bootTabs[] = {
    StartAppItem,
    WiFiConfItem,
    UpdateItem,
    BootInfoItem,
    PowerOffItem,
};

// ———————————————————————————————————————————————————————————————————————————————————————————————————

// Dichiarazione della root del menu ("Main Menu")
const MenuItem bootMenu("Boot", nullptr, bootTabs, sizeof(bootTabs) / sizeof(bootTabs[0]));
