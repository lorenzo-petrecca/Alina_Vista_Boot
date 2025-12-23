// boot_ota.hpp
#pragma once

#include "boot_versions/boot_versions.hpp"  // per RemoteVersion
#include <esp_err.h>

bool bootOtaInstallRemote(const RemoteVersion& v);
