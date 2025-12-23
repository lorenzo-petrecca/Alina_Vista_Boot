#pragma once

#include <Arduino.h>

// Definizione dei tipi di dato da salvare nelle impostazioni
enum class NvsType : uint8_t {
//  TYPE            DATA            SIZE (bytes)
    BOOL,//         bool            1
    CHAR,//         int8_t          1
    UCHAR,//        uint8_t         1
    SHORT,//        int16_t         2
    USHORT,//       uint16_t        2
    INT,//          int32_t         4  
    UINT,//         uint32_t        4
    LONG64,//       int64_t         8
    ULONG64,//      uint64_t        8
};

// Inizializza NVS (va chiamata in setup())
bool bootNvsInit();

// Scrive il flag boot_mode (true = resta in boot, false = avvia Luna)
bool bootNvsSetBootMode(bool value);

// Legge una risorsa generica (non stringa) da NVS.
// - nvsNamespace: es. "boot", "luna_meta", "wifi"
// - type: tipo del valore
// - key: chiave NVS
// - outValue: puntatore alla variabile dove scrivere il risultato.
//   ATTENZIONE: il chiamante deve passare il tipo giusto! (es. bool*, uint32_t*, ecc.)
//
// Ritorna:
//   - true  se la chiave esiste e il valore è stato letto correttamente
//   - false se la chiave NON esiste o c’è stato un errore
bool bootNvsGetResource (const char* nvsNamespace, NvsType type, const char* key, void* outValue);


// Funzione dedicata per leggere stringhe NVS.
// - buffer: char[] dove scrivere la stringa (terminata da '\0')
// - bufferSize: dimensione del buffer, incluso il terminatore
// Ritorna true se letta con successo, false se chiave mancante/errore o buffer troppo piccolo.
bool bootNvsGetString(const char* nvsNamespace, const char* key, char* buffer, size_t bufferSize);