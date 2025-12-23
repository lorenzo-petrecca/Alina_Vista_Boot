#pragma once

/*

CONTRATTO MANIFEST JSON 

{
  "schema": 1,
  "flash_size": "16MB",
  "apps": [
    {
      "id": "luna",
      "display_name": "Luna",
      "latest": "1.3.0",
      "versions": [
        {
          "version": "1.3.0",
          "build": 10300,
          "date": "2025-12-04",
          "channel": "stable",
          "url": "https://mio-sito.it/alina-vista/luna/16mb/luna-1.3.0-16mb.bin",
          "sha256": "d41d8cd98f00b204e9800998ecf8427e",
          "min_boot": "0.1.0",
          "min_boot_build": 100,
          "notes": "Fix bug irrigazione"
        },
        {
          "version": "1.2.1",
          "build": 10201,
          "date": "2025-11-20",
          "channel": "stable",
          "url": "https://mio-sito.it/alina-vista/luna/16mb/luna-1.2.1-16mb.bin",
          "sha256": "e2fc714c4727ee9395f324cd2e7f331f",
          "min_boot": "0.1.0",
          "min_boot_build": 100,
          "notes": "Patch sicurezza"
        }
      ]
    },

    {
      "id": "luna-lite",
      "display_name": "Luna Lite",
      "latest": "0.5.0",
      "versions": [
        {
          "version": "0.5.0",
          "build": 500,
          "date": "2025-10-01",
          "channel": "beta",
          "url": "https://mio-sito.it/alina-vista/luna-lite/16mb/luna-lite-0.5.0-16mb.bin",
          "sha256": "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",
          "min_boot": "0.2.0",
          "min_boot_build": 200,
          "notes": "Firmware leggero sperimentale"
        }
      ]
    }
  ]
}


*/

#include <Arduino.h>
#include <IPAddress.h>
#include "config.hpp"

// Struttura singola versione remota
struct RemoteVersion{
    char appId[16];
    char appName[16];
    char version[16];
    uint32_t build;
    char url[128];
    char sha[65];
    uint32_t minBootBuild;
    char minBoot[16];
};


// Scarica e parsa il manifest dal server.
//
// - manifestUrl: URL del file JSON (deciso in config.hpp per ogni env)
// - out: array di RemoteVersion preallocato
// - maxCount: dimensione dell'array out
// - outCount: numero di versioni realmente lette (0..maxCount)
//
// Ritorna:
//   true  = manifest ok, almeno 1 versione letta
//   false = errore di rete / parse / nessuna versione valida
bool bootFetchRemoteVersions (const char* manifestUrl, RemoteVersion* out, uint8_t maxEntries, uint8_t& outCount);
