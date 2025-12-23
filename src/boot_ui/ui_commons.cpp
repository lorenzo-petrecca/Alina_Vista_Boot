#include "ui_commons.hpp"
#include "font/mono_5x8px/data.h"
#include "font/mono_5x8px/meta.h"

// HELPERS
uint8_t getDigits (int num) {
    if (num == 0) return 1;
    uint8_t i = 0;
    while (num != 0) {
        num /= 10;
        i++;
    }
    return i;
};

uint8_t calcAbscissaToCenter (uint8_t strLen, uint8_t maxLen, uint8_t glyphW, uint8_t glyphS) {
    return (uint8_t)((uint16_t)(maxLen - (uint16_t)(strLen * (glyphW + glyphS))) / (uint16_t)2);
}


// COMMON ACTIONS

void onBack() {
    auto &menu = mController_getInstance();
    menu.exit();
    menu.displayMenu();
}

void renderScreen(const char *label, uint16_t curVal) {
    auto &lcd = display_getInstance();
    lcd.clear();
    delay(25);
    lcd.printStringCentered(label, 0);
    uint8_t valL = getDigits(curVal);
    uint8_t xval = calcAbscissaToCenter((valL + 4), COLUMNS, GLYPH_WIDTH, GLYPH_SPACING);
    lcd.setCursor(xval, 2);
    lcd.print("- ");
    lcd.print(curVal);
    lcd.print(" +");
}

void exitAction () {
    auto &lcd = display_getInstance();
    auto &menu = mController_getInstance();
    menu.exitAction();
    lcd.softRefresh();

    // DA COMPLETARE !!!
    // inserire la logica per
    // il salvataggio in NVS
}

void renderExitNotice (const uint8_t noticeFinalRow) {
    const char* exitNot1 = "Premi un tasto";
    const uint8_t xNot1 = calcAbscissaToCenter((uint8_t)strlen(exitNot1), COLUMNS, GLYPH_WIDTH, GLYPH_HEIGHT);
    const char* exitNot2 = "per uscire";
    const uint8_t xNot2 = calcAbscissaToCenter((uint8_t)strlen(exitNot2), COLUMNS, GLYPH_WIDTH, GLYPH_HEIGHT);

    auto &lcd = display_getInstance();
    lcd.setCursor(xNot1, noticeFinalRow - 1);
    lcd.print(exitNot1);
    lcd.setCursor(xNot2, noticeFinalRow);
    lcd.print(exitNot2);
}

static const uint8_t progressBlock[] = { 0x7f, 0x7f, 0x7f, 0x7f, 0x00 };

void renderProgessBar (
    const uint8_t screenWidth,
    const uint8_t screenPages,
    uint8_t renderingPage,
    ProgressBar *bar
) {
    if (!bar) return;
    auto &lcd = display_getInstance();
    lcd.setCursor(0, renderingPage);
    lcd.print("[");

    uint8_t maxProgressBlocks = (screenWidth / (GLYPH_WIDTH + GLYPH_SPACING)) - 2;

    uint8_t currProgress = (uint8_t)map(bar->current, bar->end, bar->start, 0, maxProgressBlocks);

    for (uint8_t i = 0; i < bar->current + 1; i++) {
        lcd.drawInRect(
            (maxProgressBlocks * (GLYPH_WIDTH + GLYPH_SPACING)), 
            (renderingPage * 8),
            (GLYPH_WIDTH + GLYPH_SPACING),
            8,
            progressBlock
        );
    }

    lcd.setCursor((maxProgressBlocks + 1) * (GLYPH_WIDTH + GLYPH_SPACING), renderingPage);
    lcd.print("]");
}