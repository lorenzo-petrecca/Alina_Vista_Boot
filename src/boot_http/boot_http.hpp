#pragma once

#include <HTTPClient.h>


// Inizializza una connessione HTTPS verso `url` usando WiFiClientSecure
// con root CA pinning. Restituisce true se http.begin() ha avuto successo.
bool bootHttpBegin (HTTPClient &http, const char *url);


// Wrapper opzionale: esegue direttamente un GET su `url`
// (chiama bootHttpBegin + http.GET), restituendo il codice HTTP
// oppure un codice < 0 in caso di errore begin().
int bootHttpGet (HTTPClient &http, const char *url);


inline void bootHttpEnd (HTTPClient &http) {
    http.end();
}
