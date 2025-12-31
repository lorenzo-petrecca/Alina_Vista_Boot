#include "boot_wifi.hpp"
#include "config.hpp"

static BootWifiState g_wifiState = BootWifiState::DISCONNECTED;

void bootWifiInit () {
    g_wifiState = BootWifiState::DISCONNECTED;
    WiFi.persistent(false); // non scrive config wifi in flash
    WiFi.disconnect(true, true);    // disconnette e cancella configurazione runtime
}

bool bootWifiLoadCredentials (BootWifiCredentials &out) {
    // inizializza a stringhe vuote per sicurezza
    memset(&out, 0, sizeof(out));

    // carica ssid e password nella struttura credenziali da nvs
    bool okSsid = bootNvsGetString(WIFI_NVS_NAMESPACE, WIFI_KEY_SSID, out.ssid, sizeof(out.ssid));
    bool okPsw = bootNvsGetString(WIFI_NVS_NAMESPACE, WIFI_KEY_PSW, out.psw, sizeof(out.psw));

    if (!okSsid || ! okPsw) {
        // se non trovo una delle due, considero le credenziali non valide
        memset(&out, 0, sizeof(out));
        return false;
    }

    return true;
}


bool bootWifiConnectFromNvs() {
    BootWifiCredentials creds;
    if (!bootWifiLoadCredentials(creds)) {
        Serial.println(F("[WIFI] No credentials saved in NVS."));
        g_wifiState = BootWifiState::DISCONNECTED;
        return false;
    }

    Serial.print(F("[WIFI] Connecting to Wi-Fi network: "));
    Serial.println(creds.ssid);

    WiFi.mode(WIFI_AP_STA);
    g_wifiState = BootWifiState::STA_CONNECTING;

    WiFi.begin(creds.ssid, creds.psw);
    uint32_t connStartedAt = millis();
    wl_status_t status;

    do {
        delay (250);
        status = WiFi.status();
        Serial.print(".");
    } while (status != WL_CONNECTED && (millis() - connStartedAt) < WIFI_CONN_TIMEOUT);

    Serial.println();

    if (status == WL_CONNECTED) {
        g_wifiState = BootWifiState::STA_CONNECTED;
        Serial.print("[WIFI] Connected. IP: ");
        Serial.println(WiFi.localIP());
        return true;
    } else {
        Serial.println("[WIFI] Connection failed");
        g_wifiState = BootWifiState::DISCONNECTED;
        WiFi.disconnect(true, true);
    }
    return false;
}


bool bootWifiStartAp(const char* apSsid, const char* apPassword, uint8_t channel) {
    if (!apSsid || apSsid[0] == '\0') {
        Serial.println("[WIFI] ERROR: SSID AP not valid.");
        return false;
    }

    Serial.print("[WIFI] Avvio AP: ");
    Serial.println(apSsid);

    
    WiFi.mode(WIFI_AP);

    bool apOk = false;
    if (apPassword && apPassword[0] != '\0') {
        apOk = WiFi.softAP(apSsid, apPassword, channel);
    } else {
        apOk = WiFi.softAP(apSsid, nullptr, channel);
    }

    if (!apOk) {
        Serial.println("[WIFI] ERROR: Unable to start AP");
        g_wifiState = BootWifiState::DISCONNECTED;
        return false;
    }

    g_wifiState = BootWifiState::AP_RUNNING;

    Serial.print("[WIFI] AP is running. IP: ");
    Serial.println(WiFi.softAPIP());
    return true;
}


void bootWifiStop() {
    Serial.println("[WIFI] Stop WiFi (STA + AP).");
    WiFi.softAPdisconnect(true);
    WiFi.disconnect(true, true);
    WiFi.mode(WIFI_OFF);
    g_wifiState = BootWifiState::DISCONNECTED;
}


BootWifiState bootWifiGetState() {
    return g_wifiState;
}

