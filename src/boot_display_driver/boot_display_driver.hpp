#pragma once

#include <Arduino.h>
#include <PCD8544.h>
#include "menu/menu.h"

// Accesso al PCD8544 grezzo (per la UI/Menu)
PCD8544& display_getInstance();

// Accesso al Menu Controller
MenuController& mController_getInstance();

void display_init();