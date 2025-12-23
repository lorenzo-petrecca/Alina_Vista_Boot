#pragma once

#include <Arduino.h>

// Avvia la partizione applicativa "luna" (ota_0).
// Ritorna false solo in caso di errore grave (partizione mancante o set fallito).
// In caso di successo NON ritorna, perché chiama esp_restart().
bool bootStartApp();
