#include "boot_nvs.hpp"
#include "config.hpp"

extern "C" {
    #include "nvs_flash.h"
    #include "nvs.h"
}

bool bootNvsInit() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // In caso di problemi resettiamo NVS
        nvs_flash_erase();
        err = nvs_flash_init();
    }
    return (err == ESP_OK);
}


/*bool bootNvsGetBootMode(bool defaultValue) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(BOOT_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        // Namespace non esistente → usiamo default
        return defaultValue;
    }

    uint8_t value = defaultValue ? 1 : 0;
    size_t required_size = sizeof(value);
    err = nvs_get_u8(handle, KEY_BOOT_MODE, &value);
    nvs_close(handle);

    if (err == ESP_OK) {
        return (value != 0);
    }

    // Chiave non trovata → default
    return defaultValue;
}*/

bool bootNvsSetBootMode(bool value) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(BOOT_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return false;
    }

    uint8_t v = value ? 1 : 0;
    err = nvs_set_u8(handle, KEY_BOOT_MODE, v);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return (err == ESP_OK);
}


bool bootNvsGetResource (const char* nvsNamespace, NvsType type, const char* key, void* outValue) {
    if (!nvsNamespace || !key || !outValue) {
        return false;
    }

    nvs_handle_t handle;
    esp_err_t err = nvs_open(nvsNamespace, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return false;   // Namespace non esistente
    }

    switch (type) {
    case NvsType::BOOL: {
        uint8_t tmp = 0;
        err = nvs_get_u8(handle, key, &tmp);
        if (err == ESP_OK) {
            *static_cast<bool*>(outValue) = (tmp != 0);
        }  
        break;
    }
    case NvsType::CHAR: {
        err = nvs_get_i8(handle, key, static_cast<int8_t*>(outValue));
        break;
    }
    case NvsType::UCHAR: {
        err = nvs_get_u8(handle, key, static_cast<uint8_t*>(outValue));
        break;
    }
    case NvsType::SHORT: {
        err = nvs_get_i16(handle, key, static_cast<int16_t*>(outValue));
        break;
    }
    case NvsType::USHORT: {
        err = nvs_get_u16(handle, key, static_cast<uint16_t*>(outValue));
        break;
    }
    case NvsType::INT: {
        err = nvs_get_i32(handle, key, static_cast<int32_t*>(outValue));
        break;
    }
    case NvsType::UINT: {
        err = nvs_get_u32(handle, key, static_cast<uint32_t*>(outValue));
        break;
    }
    case NvsType::LONG64: {
        err = nvs_get_i64(handle, key, static_cast<int64_t*>(outValue));
        break;
    }
    case NvsType::ULONG64: {
        err = nvs_get_u64(handle, key, static_cast<uint64_t*>(outValue));
        break;
    }
    default: {
        err = ESP_ERR_INVALID_ARG;
        break;
    }
    }


    nvs_close(handle);

    return (err == ESP_OK);
}



bool bootNvsGetString (const char* nvsNamespace, const char* key, char* buffer, size_t bufferSize) {
    if (!nvsNamespace || !key || !buffer || bufferSize == 0) {
        return false;
    }
    nvs_handle_t handle;
    esp_err_t err = nvs_open(nvsNamespace, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return false;   // Namespace non esistente
    }

    size_t required_size;
    // Prima call per sapere quanti byte servono (incluso '\0')
    err = nvs_get_str(handle, key, nullptr, &required_size);

    // Se non ci stiamo nel buffer, meglio fallire che troncare in silenzio
    if (required_size > bufferSize) {
        nvs_close(handle);
        return false;
    }

    err = nvs_get_str(handle, key, buffer, &required_size);
    nvs_close(handle);
    return (err == ESP_OK);
}



bool bootNvsSetString(const char* key, const char* value) {
    if (!key || !value) return false;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(BOOT_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) return false;

    err = nvs_set_str(handle, key, value);
    if (err != ESP_OK) err = nvs_commit(handle);

    nvs_close(handle);
    return (err == ESP_OK);
}