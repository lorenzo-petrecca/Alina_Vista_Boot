#pragma once
#include <pgmspace.h>

static const char BOOT_WEB_INDEX_HTML[] PROGMEM = R"HTML(
<!doctype html><meta charset=utf-8><meta content="width=device-width,initial-scale=1"name=viewport><title>Alina Vista - WiFi Setup</title><style>*{margin:0;padding:0;box-sizing:border-box}body{font-family:system-ui;padding:10px;display:flex;align-items:center;justify-content:center;flex-direction:column;max-width:100dvw;min-height:100dvh;color:#001b3c}input,select{font-size:16px;width:100%;padding:12px;background-color:transparent;border:1px solid #6394be;border-radius:12px;outline:0}button{font-size:14px;padding:16px;border-radius:8px;border:none;transition:all .3s ease-in-out;cursor:pointer;background-color:#e1e1e1;background-color:#1c907e;color:#fff}button:hover{background-color:#2fc8b1;color:inherit}button:has(svg){border-radius:50000px;width:48px;height:48px;background-color:transparent;border:1px solid #1c907e;padding:14px}.wrap{margin:auto}.form{display:flex;gap:20px;flex-direction:column;justify-content:center;align-items:start;width:520px;min-width:360px;max-width:100%;height:auto;background-color:#f0fff8;padding:20px;border-radius:20px;box-shadow:0 1px 8px rgba(8,71,41,.3);border:1px solid rgba(158,250,255,.7)}.buttons-row{display:flex;gap:12px;align-items:center;justify-content:flex-end;padding:4px;width:100%}.form-row{width:100%;display:flex;gap:8px;align-items:flex-start;justify-content:center;padding:4px;flex-direction:column}small{opacity:.7}@media (max-width:600px){.form{width:auto}}@media (max-width:380px){.form{min-width:280px;padding:12px 10px}body{padding:4px}html{font-size:80%}}</style><div class=wrap><div class=form><h2 style=margin-bottom:16px>Alina Setup</h2><div class=form-row><label>Rete</label> <select id=ssid></select></div><div class=form-row><label>Password</label> <input id=pwd placeholder=password type=password></div><div class=buttons-row><button onclick=scan()><svg height=100 style=width:100%;height:100% viewBox="0 0 30 30"width=100 x=0px xmlns=http://www.w3.org/2000/svg y=0px><path d="M 15 3 C 12.031398 3 9.3028202 4.0834384 7.2070312 5.875 A 1.0001 1.0001 0 1 0 8.5058594 7.3945312 C 10.25407 5.9000929 12.516602 5 15 5 C 20.19656 5 24.450989 8.9379267 24.951172 14 L 22 14 L 26 20 L 30 14 L 26.949219 14 C 26.437925 7.8516588 21.277839 3 15 3 z M 4 10 L 0 16 L 3.0507812 16 C 3.562075 22.148341 8.7221607 27 15 27 C 17.968602 27 20.69718 25.916562 22.792969 24.125 A 1.0001 1.0001 0 1 0 21.494141 22.605469 C 19.74593 24.099907 17.483398 25 15 25 C 9.80344 25 5.5490109 21.062074 5.0488281 16 L 8 16 L 4 10 z"></path></svg></button> <button onclick=save()>Connect</button></div><pre id=out></pre><script>async function scan(){
  out("Scanning...");
  const r = await fetch('/scan');
  const j = await r.json();
  const s = document.getElementById('ssid');
  s.innerHTML='';
  j.networks.forEach(n=>{
    const o=document.createElement('option');
    o.value=n.ssid; o.textContent=`${n.ssid} (${n.rssi} dBm)${n.secure?' 🔒':''}`;
    s.appendChild(o);
  });
  out("OK");
}

async function save(){
  const ssid=document.getElementById('ssid').value;
  const pwd=document.getElementById('pass').value;
  out("Saving...");
  const r=await fetch('/save',{method:'POST',headers:{'Content-Type':'application/json'},
    body:JSON.stringify({ssid,pwd})});
  out(await r.text());
}

async function status(){
  const r=await fetch('/status');
  out(JSON.stringify(await r.json(),null,2));
}

function out(t){document.getElementById('out').textContent=t;}</script></div></div>
)HTML";
