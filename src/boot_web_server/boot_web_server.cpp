#include "boot_web_server.hpp"

#include "boot_we_pages/boot_we_pages.hpp"

#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ArduinoJson.h>

#include "boot_nvs/boot_nvs.hpp"
#include "boot_wifi/boot_wifi.hpp"


static WebServer g_server(80);
static DNSServer g_dns;
static bool g_running = false;

static constexpr uint16_t DNS_PORT = 53;

static void sendNoCache() {
  g_server.sendHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
  g_server.sendHeader("Pragma", "no-cache");
}

static void handleIndex () {
    sendNoCache();
    g_server.send_P(200, "text/html", BOOT_WEB_INDEX_HTML);
}

static void handleScan () {
    sendNoCache();

    int n = WiFi.scanNetworks(/* async */false, /* hidden */true);
    JsonDocument doc;
    JsonArray arr = doc["networks"].to<JsonArray>();

    for (int i = 0; i < n && i < 15; i++) {
        JsonObject obj = arr.add<JsonObject>();
        obj["ssid"] = WiFi.SSID(i);
        obj["rssi"] = WiFi.RSSI(i);
        obj["secure"] = (WiFi.encryptionType(i) != WIFI_AUTH_OPEN);
    }

    String out;
    serializeJson(doc, out);
    g_server.send(200, "application/json", out);
}


static void handleSave () {
    sendNoCache();
    
    if (!g_server.hasArg("plain")) {
        g_server.send(400, "text/plain", "Missing body");
        return;
    }

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, g_server.arg("plain"));
    if (err) {
        g_server.send(400, "text/plain", "Bad JSON");
        return;
    }

    const char *ssid = doc["ssid"] | "";
    const char *pass = doc["pass"] | "";

    if (ssid[0] == '\0') {
        g_server.send(400, "text/plain", "SSID empty");
        return;
    }

    // salvataggio in NVS
    bool okSSid = bootNvsSetString(WIFI_KEY_SSID, ssid);
    bool okPass = bootNvsSetString(WIFI_KEY_PSW, pass);

    if (!okSSid || !okPass) {
        g_server.send(400, "text/plain", "Error while saving");
        return;
    }

    // tenta connessione STA
    bool connection = bootWifiConnectFromNvs();

    if (!connection) {
        g_server.send(400, "text/plain", "Unable to connect to the network");
        return;
    }

    g_server.send(200, "text/plain", "OK");
}


static void handleNotFound () {
    IPAddress ip = WiFi.softAPIP();
    g_server.sendHeader("Location", String("http://") + ip.toString() + "/", true);
    g_server.send(302, "text/plain", "");
}




void bootWebStart () {
    if (g_running) return;

    // IMPORTANTE: DNS captive deve partire DOPO che l'AP è attivo
    IPAddress apIp = WiFi.softAPIP();
    g_dns.start(DNS_PORT, "*", apIp);

    g_server.on("/", HTTP_GET, handleIndex);
    g_server.on("/scan", HTTP_GET, handleScan);
    g_server.on("/save", HTTP_POST, handleSave);
    g_server.onNotFound(handleNotFound);

    g_server.begin();
    g_running = true;
}


void bootWebStop () {
    if (!g_running) return;
    g_server.stop();
    g_dns.stop();
    g_running = false;
}


void bootWebRun () {
    if (!g_running) return;
    
    g_dns.processNextRequest();
    g_server.handleClient();
}


bool bootWebIsRunning() {
    return g_running;
}