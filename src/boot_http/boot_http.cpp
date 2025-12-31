#include "boot_http.hpp"
#include <WiFiClientSecure.h>

// ============================================================================
//  Root CA
// ============================================================================
//
// Qui devi incollare la root CA (in PEM) che firma i certificati usati
// dal tuo dominio su Vercel. Esempio tipico (Let’s Encrypt ISRG Root X1).
// Puoi sostituire il contenuto dopo aver estratto il certificato reale.
//
// ATTENZIONE: mantieni esattamente le righe BEGIN/END e il formato PEM.
//

static const char *rootCACert = R"EOF(
-----BEGIN CERTIFICATE-----
REPLACE_WITH_REAL_ROOT_CA_PEM
-----END CERTIFICATE-----
)EOF";

// Se vuoi una build "insicure" per sviluppo, puoi abilitare questa define
// da build_flags in platformio.ini, es:
//   -D BOOT_HTTP_INSECURE
//
// In quel caso verrà usato client.setInsecure() invece della root CA.
//
// #define BOOT_HTTP_INSECURE


// ============================================================================
//  Client HTTPS condiviso
// ============================================================================

static WiFiClientSecure &bootGetSecureClient() {
    static WiFiClientSecure client;
    static bool initialized = false;

    if (!initialized) {
    #ifdef BOOT_HTTP_INSECURE
        // Modalità insicura: accetta qualsiasi certificato
        client.setInsecure();
    #else
        // Modalità sicura: pinning della root CA
        client.setCACert(rootCACert);
    #endif
        initialized = true;
    }

    return client;
}

// ============================================================================
//  Funzioni pubbliche
// ============================================================================

bool bootHttpBegin (HTTPClient &http, const char *url) {
    WiFiClientSecure &client = bootGetSecureClient();

    return http.begin(client, url);
}

int bootHttpGet (HTTPClient &http, const char *url) {
    if (!bootHttpBegin(http, url)) {
        return -1; // ritorna errore
    }
    return http.GET();
}