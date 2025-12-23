#include "boot_state.hpp"

static BootState g_bootState = BootState::AUTO;

void bootStateInit () {
    g_bootState = BootState::AUTO;
}

BootState getBootState () {
    return g_bootState;
}

void setBootState (BootState s) {
    g_bootState = s;
}