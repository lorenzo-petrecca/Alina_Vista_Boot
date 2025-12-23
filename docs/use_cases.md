# CASI D'USO

## UC 1 - Avvio automatico del firmware principale "luna"
1. L'utente accende il dispositivo.
2. Si avvia automaticamente l'app Boot che si trova nella partizione "***boot***" della flash.
3. Il boot controlla se c'è un flag "boot_mode" salvato in NVS.
4. Se il flag è FALSE:
   → Il boot seleziona la partizione app "***luna***"
   → Effettua esp_ota_set_boot_partition()
   → Riavvia il dispositivo
5. L'utente non interagisce con il boot in questo caso.

<br>


## UC 2 - Avvio automatico del boot
1. L'utente accende il dispositivo.
2. Si avvia automaticamente l'app Boot che si trova nella partizione "***boot***" della flash.
3. Il boot controlla se c'è un flag "boot_mode" salvato in NVS.
4. Se il flag è TRUE:
    → Il boot avvia la User Interface
    → Mostra le opzioni: “Avvia app”, "WiFi”, “Installa”, “Info”
    → L’utente interagisce con il boot tramite i pulsanti
   
<br>

## UC 3a - Configurazione del WiFi ("automatica")
1. L’utente si trova nel Boot Menu (UC2).
2. L'utente seleziona "WiFi → Connetti" dalla UI locale.
3. Il boot cerca in NVS se sono già salvate ssid e password.
4. Se sono presenti:
    → Il boot tenta la connessione alla rete in modalità STA (o STA+AP)
    → Se la connessione ha successo, la configurazione termina con esito positivo.
    → Se la connessione fallisce, si passa al flusso “senza credenziali valide” (punto 5).
5. Se NON sono presenti o non sono valide:
    1. Il boot entra in AP Mode ed espone una interfaccia web.
    2. L'utente si collega all'Access Point tramite smartphone, o pc.
    3.  L'utente accede alla pagina web del boot.
    4.  L'utente seleziona l'SSID all'elenco delle reti trovate
    5.  L'utente inserisce la password.
    6.  Il boot tenta la connessione in STA (o STA+AP).
    7.  Se la connessione ha successo:
        → salva SSID e password in NVS
        → conferma all’utente (display + web)
    8. Se la connessione NON ha successo, ritorna al punto 5.4 (selezione rete).

<br>

## UC 3b - Configurazione del WiFi ("manuale")
1. L’utente si trova nel Boot Menu (UC2).
2. L'utente seleziona "WiFi → Configura" dalla UI locale.
3. Il boot parte dal punto UC3a 5.1 (AP mode).

<br>

## UC 4 - Aggiornamento OTA (cloud)
1. Una volta configurato il WiFi, l'utente deve selezionare da interfaccia utente *(display Alina Vista)* l'opzione "**Installa → Altre vers.**".
2. Il boot si collega al sito dove sono presenti i file ***.bin*** relativi alle versioni disponibili del firmware **luna**, il quale risponderà con un file ***json*** con le versioni disponibili e scaricabili.
3. Il boot mostra a display le versioni scaricabili ottenute.
4. L'utente selezionerà la versione che vuole scaricare.
5. Il boot avvia l'installazione della versione selezionata del firmware.
6. Il boot sovrascrive la partizione *"**luna**"* della flash con il nuovo firmware.
7. Il boot modifica il flag "boot_mode" impostandolo su FALSE.
8. Il boot esegue il riavvio.

<br>

## UC 5 - Fallback se app non parte
Se dopo aver impostato Luna, il sistema resetta N volte in X secondi → il boot forza ***boot_mode = TRUE*** e resta in boot menu.

<br>
<br>

---
<br>

<p style="font-size: 12px">Per maggiori informazioni sul menu di boot fare riferimento al file "<b>docs/boot_menu.md</b>".</p>