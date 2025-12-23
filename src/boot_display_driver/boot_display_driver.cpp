#include "boot_display_driver.hpp"
#include "pinmap.hpp"
#include "config.hpp"
#include <SPI.h>
#include <font/mono_5x8px/data.h>
#include <font/mono_5x8px/meta.h>
#include "menu/menu.h"


// Istanza globale del display
static PCD8544 lcd (
    SPI,
    {
        SCK_LCD,
        MOSI_LCD,
        CS_LCD,
        DC_LCD,
        RST_LCD,
        BL_LCD,
    },
    1000000,
    SPI_MODE0
);

// Istanza globale del menu controller (UI)
static MenuController controller({
    BACK, 
    FWD, 
    SEL, 
    true, // active low = true (pulsante chiude a GND, pullup a VCC) | false (pulsante chiude a VCC, pulldown a GND)
    false, // resistenza interna (pullup/pulldown) = true (resistenza interna) | false (resistenza esterna)
    150 // debounce ms per pulsanti
});


PCD8544& display_getInstance () {
    return lcd;
}
    
MenuController& mController_getInstance() {
    return controller;
}

void display_init () {
    lcd.begin (95, 50, 4, 2);
    lcd.setFont(MONO_5x7);
    lcd.clear();
}
