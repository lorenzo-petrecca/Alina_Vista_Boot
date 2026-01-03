#include <Arduino.h>
#include "boot_state/boot_state.hpp"
#include "boot_nvs/boot_nvs.hpp"
#include "pinmap.hpp"
#include "boot_display_driver/boot_display_driver.hpp"
#include "boot_ui/boot_ui.hpp"
#include "config.hpp"
#include "boot_startup/boot_startup.hpp"
#include "boot_wifi/boot_wifi.hpp"
#include "boot_web_server/boot_web_server.hpp"


static void decideInitialState() {
    // Se la chiave non esiste ancora, consideriamo default = true → resta in Boot
    bool bootMode;
    bool found = bootNvsGetResource(BOOT_NVS_NAMESPACE, NvsType::BOOL, "boot_mode", &bootMode);
    if (!found) bootMode = true;

    Serial.print("[BOOT] ");
    Serial.println(bootMode ? "Entering boot" : "Launching app");

    if (bootMode) {
      // Avvio automatico del boot (Boot Menu)
      setBootState(BootState::BOOT);
    } else {
      // Avvio automatico di Luna
      setBootState(BootState::START_APP);
    }
}

static inline void displayBootMenu () {
    auto &lcd = display_getInstance();
    auto &controller = mController_getInstance();
    controller.attachDisplay(&lcd);
    controller.createMenu(&bootMenu);
    controller.displayMenu();
}

void setup() {
    pinMode(HOLD_ON, OUTPUT);
    digitalWrite(HOLD_ON, HIGH);

    pinMode(D_EN_CTR, OUTPUT);
    pinMode(D_BOOT_CTR, OUTPUT);
    digitalWrite(D_EN_CTR, LOW);
    digitalWrite(D_BOOT_CTR, LOW);

    Serial.begin(115200);
    delay(1500);
    Serial.println();
    Serial.println("======================");
    Serial.println("   Alina-Vista BOOT   ");
    Serial.println("======================");

    bootStateInit();

    if (!bootNvsInit()) {
        Serial.println("[BOOT] ERROR: Unable to initialize NVS");
        setBootState(BootState::ERROR);
        return;
    }

    decideInitialState();

    BootState s = getBootState();
    switch (s) {
      case BootState::BOOT:
        Serial.println("[BOOT] Entered boot mode.");
        display_init();
        displayBootMenu();
        break;
    
      case BootState::START_APP:
        Serial.println("[BOOT] Boot application firmware (partition \"luna\")");
        bootStartApp();
        setBootState(BootState::ERROR); // fallback se dovesse fallire
        break;
    
      case BootState::ERROR:
        Serial.println("[BOOT] Error state, stay in Boot.");
        display_init();
        break;
    
      default:
        Serial.println("[BOOT] Unexpected state. Force boot menu");
        setBootState(BootState::BOOT);
        display_init();
        displayBootMenu();
        break;
    }

    // inizializza il modulo wifi
    bootWifiInit();
}

void loop() {
    auto &controller = mController_getInstance();
    controller.update();
    bootWebRun();
}