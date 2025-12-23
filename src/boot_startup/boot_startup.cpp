#include "boot_startup.hpp"
#include "config.hpp"

extern "C" {
  #include "esp_system.h"
  #include "esp_ota_ops.h"
  #include "esp_partition.h"
}

bool bootStartApp () {
  Serial.println("[BOOT] App Launcher: Looking for the application partition...");

  // Trova la partizione app di tipo ota_0 con name "luna"
  const esp_partition_t* appPart = esp_partition_find_first(
      ESP_PARTITION_TYPE_APP,
      ESP_PARTITION_SUBTYPE_APP_OTA_0,
      APP_PARTITION_NAME    // puoi mettere nullptr se vuoi solo type/subtype
  );

  if (appPart == nullptr) {
    Serial.println("[BOOT] ERROR: app partition (\"luna\") not found.");
    return false;
  }

  Serial.print("[BOOT] app partition (\"luna\") at offset 0x");
  Serial.print(appPart->address, HEX);
  Serial.print(" with size 0x");
  Serial.println(appPart->size, HEX);

  // (Opzionale) potresti verificare l'header dell'app:
  // const esp_app_desc_t* desc = esp_ota_get_app_description();
  // ma per ora ci fidiamo che sia un'app valida.

  // Imposta 'luna' come prossima app di boot
  esp_err_t err = esp_ota_set_boot_partition(appPart);
  if (err != ESP_OK) {
    Serial.print("[BOOT] ERROR: esp_ota_set_boot_partition() = ");
    Serial.println((int)err);
    return false;
  }

  Serial.println("[BOOT] App partition (\"luna\") set as boot partition.");
  Serial.println("[BOOT] Rebooting system...");
  delay(100);  // piccolo delay per svuotare la Serial

  esp_restart();
  // Se tutto va bene, da qui NON si torna più.

  return true; // Non verrà mai eseguito, ma lo lasciamo per completezza.
}