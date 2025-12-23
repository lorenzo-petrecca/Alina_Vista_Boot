#pragma once

#include <Arduino.h>
#include "boot_display_driver/boot_display_driver.hpp"

uint8_t getDigits (int num); // helper per ottenere il numero di caratteri della stringa
uint8_t calcAbscissaToCenter (uint8_t strLen, uint8_t maxLen, uint8_t glyphW, uint8_t glyphS); // helper per calcolare l'ascissa di partenza in modo da centrare orizzontalmente la stringa

void onBack();                                          // Generale Callback "Indietro"
void renderScreen(const char *label, uint16_t curVal);  // Renderizza le schermate di modifica, formattandole
void exitAction ();                                           // Esce dalla schermata di modifica attuale e salva le modifiche

void renderExitNotice (const uint8_t noticeFinalRow);


// Struttura per progress bar
struct ProgressBar {
    int64_t current;
    const int64_t start;
    const int64_t end;
};

/* 
 *  Renderizza in tempo reale una barra di progresso
 *  Input:
 *      - screenWidth: pixel orizzontali schermo    [PCD8544: 84px]
 *      - screenPages: pagine schermo (rows)        [PCD8544: 6 pages]
 *      - renderingPage: riga su cui stampare la progress bar
 *      - bar: struttura progress bar da stampare
 */
void renderProgessBar (
    const uint8_t screenWidth,
    const uint8_t screenPages,
    uint8_t renderingPage,
    ProgressBar *bar
);