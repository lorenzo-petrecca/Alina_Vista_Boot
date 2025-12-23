# Composizione del menu di boot

<br>

## Tabs

|n. tab|Nome tab|Descrizione|
|---|---|---|
|1|Avvia app|Avvia la partizione "luna" (app)|
|2|WiFi|Configurazioni WiFi|
|3|Installa|Aggiornamento firmware (OTA)|
|4|Info|Informazioni boot|
|5|Spegni|Spegne dispositivo|

<br>
<br>

### 1. Avvia app
È un *"action tab"* che quando selezionato avvia il firmware presente nella partizione "luna" (ota_0).

<br>

### 2. WiFi
|n. tab|Nome tab|Descrizione|
|---|---|---|
|1|Indietro|Torna indietro nel menu|
|2|Connetti|Connessione *automatica* a rete WiFi|
|3|Configura|Connessione *manuale* a rete WiFi|

#### 2.2 Connetti
È un *"action tab"* che quando selezionato stabilisce una connessione automatica alla rete WiFi salvata in memoria *(se fallisce la connessione il dispositivo entra in AP mode)*.

#### 2.3 Configura
È un *"action tab"* che quando selezionato fa entrare il dispositivo in AP mode al fine di esporre un'interfaccia web per poter connettere manualmente il dispositivo ad una rete WiFi.

<br>

### 3. Installa
|n. tab|Nome tab|Descrizione|
|---|---|---|
|1|Indietro|Torna indietro nel menu|
|2|Altre vers.|Elenca le versioni disponibili|
|3|Info vers.|Informazioni sulla versione attualmente installata|

#### 3.2 Altre vers.
È un "tab" che, quando selezionato, espone a schermo una lista di versioni in questo modo:
```
 ——————————————
|   Installa   |
|--------------|
|> Indietro    |
|  ···         |
|  ···         |
|  ···         |
 ————————————–– 
```

#### 3.3 Info vers.
È un "tab" che, quando selezionato, espone a schermo le informazioni sulla versione firmware app attualmente installata, in questo modo:
```
 ——————————————
|Firmware:     |
|luna v0.1.0   |
|              |
|Auth: Lorenzo |
|Petrecca      |
|Date: 2025/12 |
 ————————————–– 
```


<br>

### 4. Info
È un "tab" che, quando selezionato, espone a schermo le informazioni sulla versione firmware di boot, in questo modo:
```
 ——————————————
|Boot:         |
|Alina-Vista_Bo|
|ot_v0.1       |
|Auth: Lorenzo |
|Petrecca      |
|Date: 2025/12 |
 ————————————–– 
```

<br>

### 5. Spegni
È un *"action tab"* che, quando selezionato, spegne il dispositivo.