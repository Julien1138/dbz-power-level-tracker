/* exported as a string so index.js can embed it as a data URI */
module.exports = '<!DOCTYPE html>' +
'<html lang="en">' +
'<head>' +
'<meta charset="utf-8">' +
'<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1,user-scalable=no">' +
'<title>DBZ Power Level Tracker</title>' +
'<style>' +

'*{box-sizing:border-box;margin:0;padding:0}' +

'body{' +
'  background:#0f0f0f;' +
'  color:#e8e8e8;' +
'  font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,sans-serif;' +
'  font-size:16px;' +
'  min-height:100vh;' +
'  padding-bottom:32px;' +
'}' +

'header{' +
'  background:#e8500a;' +
'  padding:14px 16px;' +
'  display:flex;' +
'  align-items:center;' +
'  gap:12px;' +
'  position:sticky;top:0;z-index:10;' +
'  box-shadow:0 2px 8px rgba(0,0,0,.5);' +
'}' +

'.back-btn{' +
'  background:none;' +
'  border:none;' +
'  color:#fff;' +
'  font-size:22px;' +
'  line-height:1;' +
'  padding:0 4px;' +
'  cursor:pointer;' +
'  -webkit-tap-highlight-color:transparent;' +
'}' +
'.back-btn:active{opacity:.7}' +

'.save-btn{' +
'  background:rgba(255,255,255,.2);' +
'  border:none;' +
'  border-radius:8px;' +
'  color:#fff;' +
'  font-size:15px;' +
'  font-weight:600;' +
'  padding:6px 16px;' +
'  cursor:pointer;' +
'  -webkit-tap-highlight-color:transparent;' +
'}' +
'.save-btn:active{background:rgba(255,255,255,.35)}' +

'header h1{' +
'  font-size:18px;' +
'  font-weight:700;' +
'  color:#fff;' +
'  letter-spacing:.01em;' +
'  flex:1;' +
'}' +

'.section{margin:24px 0 0}' +

'.section-title{' +
'  color:#e8500a;' +
'  font-size:11px;' +
'  font-weight:700;' +
'  letter-spacing:.1em;' +
'  text-transform:uppercase;' +
'  padding:0 20px 8px;' +
'}' +

'.card{' +
'  background:#1c1c1e;' +
'  border-radius:12px;' +
'  margin:0 12px;' +
'  overflow:hidden;' +
'}' +

'.row{' +
'  display:flex;' +
'  align-items:center;' +
'  justify-content:space-between;' +
'  padding:14px 16px;' +
'  gap:16px;' +
'  border-bottom:1px solid #2c2c2e;' +
'}' +
'.row:last-child{border-bottom:none}' +

'.row-info{flex:1;min-width:0}' +
'.row-info .label{font-size:16px;color:#e8e8e8}' +
'.row-info .desc{font-size:12px;color:#888;margin-top:3px}' +

/* Toggle switch */
'.toggle{position:relative;width:51px;height:31px;flex-shrink:0}' +
'.toggle input{opacity:0;width:0;height:0;position:absolute}' +
'.track{' +
'  position:absolute;inset:0;' +
'  background:#3a3a3c;' +
'  border-radius:31px;' +
'  transition:background .25s;' +
'  cursor:pointer;' +
'}' +
'.thumb{' +
'  position:absolute;' +
'  top:2px;left:2px;' +
'  width:27px;height:27px;' +
'  background:#fff;' +
'  border-radius:50%;' +
'  box-shadow:0 1px 4px rgba(0,0,0,.4);' +
'  transition:transform .25s;' +
'  pointer-events:none;' +
'}' +
'.toggle input:checked ~ .track{background:#e8500a}' +
'.toggle input:checked ~ .thumb{transform:translateX(20px)}' +

/* Number input */
'.num-input{' +
'  background:#3a3a3c;' +
'  border:none;' +
'  border-radius:8px;' +
'  color:#fff;' +
'  font-size:16px;' +
'  font-weight:500;' +
'  padding:8px 10px;' +
'  text-align:right;' +
'  width:100px;' +
'  -webkit-appearance:none;' +
'}' +
'.num-input:focus{outline:2px solid #e8500a;outline-offset:1px}' +

'</style>' +
'</head>' +
'<body>' +

'<header>' +
'  <button class="back-btn" onclick="save()">&#8592;</button>' +
'  <h1>⚡ DBZ Power Level</h1>' +
'  <button class="save-btn" onclick="save()">Save</button>' +
'</header>' +

'<div class="section">' +
'  <div class="section-title">Power Level</div>' +
'  <div class="card">' +
'    <div class="row">' +
'      <div class="row-info">' +
'        <div class="label">Super Saiyan threshold</div>' +
'        <div class="desc">Daily steps required to transform</div>' +
'      </div>' +
'      <input class="num-input" type="number" id="stepsThreshold" min="100" max="50000" step="100">' +
'    </div>' +
'  </div>' +
'</div>' +

'<div class="section">' +
'  <div class="section-title">Bluetooth</div>' +
'  <div class="card">' +
'    <div class="row">' +
'      <div class="row-info">' +
'        <div class="label">Vibrate on disconnect</div>' +
'      </div>' +
'      <label class="toggle">' +
'        <input type="checkbox" id="vibeOnBtDisconnect">' +
'        <div class="track"></div>' +
'        <div class="thumb"></div>' +
'      </label>' +
'    </div>' +
'  </div>' +
'</div>' +

'<script>' +
'var defaults={stepsThreshold:7000,vibeOnBtDisconnect:true};' +
'var settings=defaults;' +
'try{' +
'  var h=window.location.hash.slice(1);' +
'  if(h)settings=JSON.parse(decodeURIComponent(h));' +
'}catch(e){}' +

'document.getElementById("stepsThreshold").value=settings.stepsThreshold||defaults.stepsThreshold;' +
'document.getElementById("vibeOnBtDisconnect").checked=settings.vibeOnBtDisconnect!==false;' +

'function save(){' +
'  var result={' +
'    stepsThreshold:parseInt(document.getElementById("stepsThreshold").value,10)||defaults.stepsThreshold,' +
'    vibeOnBtDisconnect:document.getElementById("vibeOnBtDisconnect").checked' +
'  };' +
'  document.location="pebblejs://close#"+encodeURIComponent(JSON.stringify(result));' +
'}' +

'window.addEventListener("beforeunload",save);' +

'</script>' +

'</body></html>';
