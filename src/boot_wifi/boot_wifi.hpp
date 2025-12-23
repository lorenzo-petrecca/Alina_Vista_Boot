#include <Arduino.h>
#include <WiFi.h>
#include "boot_nvs/boot_nvs.hpp"
#include "boot_versions/boot_versions.hpp"

enum class BootWifiState : uint8_t {
    DISCONNECTED = 0,
    STA_CONNECTING,
    STA_CONNECTED,
    AP_RUNNING
};

struct BootWifiCredentials {
    char ssid[33];
    char psw[65];
};

// Inizializza il modulo WiFi (da chiamare una volta nel boot)
void bootWifiInit ();

// Prova a caricare le credenziali da NVS (namespace "wifi").
// Ritorna true se trovate entrambe (ssid + password).
bool bootWifiLoadCredentials (BootWifiCredentials &out);


// Connessione STA usando le credenziali da NVS.
// timeoutMs: tempo massimo di attesa per la connessione.
// Ritorna true se WL_CONNECTED, false se fallisce o credenziali mancanti.
bool bootWifiConnectFromNvs();

// Avvia un Access Point per la configurazione.
// Se apPassword è nullptr o stringa vuota, AP aperto (non consigliato).
// Ritorna true se AP avviato correttamente.
bool bootWifiStartAp(const char* apSsid, const char* apPassword = nullptr, uint8_t channel = 1);

// Stop WiFi (sia STA che AP)
void bootWifiStop();

// Ritorna lo stato WiFi attuale
BootWifiState bootWifiGetState();

