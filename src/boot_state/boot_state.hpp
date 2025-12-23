#pragma once

#include <Arduino.h>

// Stati macro del boot

enum class BootState : uint8_t {
    AUTO = 0,   // stato iniziale: decide se andare in Luna o in Boot
    BOOT,       // Boot Menu
    WIFI_CONF,  // Configurazione WiFi
    OTA,        // Aggiornamento firmware app
    START_APP,  // Avvio firmware app
    ERROR       // Stato di errore/fallback
};

void bootStateInit ();

BootState getBootState ();
void setBootState (BootState s);