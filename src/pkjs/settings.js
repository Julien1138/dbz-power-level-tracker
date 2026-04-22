/* exported as a string so index.js can embed it as a data URI */
var GOKU_IMG = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAE4AAABZCAYAAACQeRI+AAAGNElEQVR4nO1cW5KDOAxstvaK0iGlQ2o/jEEyJiSzY5mZoauokAScuKO35QAPHjx48ODBTwURGQB/PHiFlrD1+YNX6EjZQ9qbeKTtUzzS9nXcWtr+mf0FKt70moHIu5GZihOVfPv4k+T9X9KaIxW3UdVPQUQAsABY1vNU8pbMD8P3TS77ex+QLXFVOl5Cri95bFvvMCET2p/7c3/8GfLOCDMhM6FT4vx7nePvoCWwEtcjq330UubG+XM4kGciZmZBRc2snt8GU8KRnqQsrOGawt3+eDf8O+NDVXULJ4jIVDV6UtX63nZ+N0yPhwCgqqGCVgIJqgwAIBKoMljv8V0r0r/Mma0iIqiqIw8ACIBCVUEEgATK9yAx5Qu0ZHkSKmElMC5EAUA1eQQ93AcAUIYqppE43jmQGEOOryuDpLx+zCaK1BE0vLeZO2WApBJpILmnB/kySIzEjMQMJGYmZlLCDJP1sDJxEwnxXLnGxXck+33rOGayjZ1N3nCJU+cVlXlTtdZZLgosWtSVyEsooSuxDYgImeQNDUcExTOylokzBMIcbRwYJoKlXIpF97ureat2sIdqH1UVUE6zdylxnIChZFDVQp6u5KFInjdxveqJqkKw34M1VJmJlMwhes4VzjlAGVZPVcMRB5JwXqW0EpuJ4cTVMEPA0d45Tg78EG3HQdq64+cjLVclot3mYZe0V1IXB5AtdquSV4k12j4jzTkkEedyT/+q4pS8nrR5e7hwVFEiCsHyaKRXR7wt6hr7lbxA2ipt/ryOVX6MjoQOxlDiooTR4TVVZ99IQjoVSFuv9eq6k3YcNwPDiIv56etJVZUtl1b7tZ8rV/fJJ1WmfAeRqKqKMsHiJIpM7eFFjzwo76Th6H1nYkoh06OSV8IVHIJbT+4WD0K3ut3+g+RiAnGrh0WpvQGFkJqWtejZLppElsew3K7auGMIEvWtlagz1PuiV6bwvoJCWX4khkrc0avGoqQqYFSIWFRehhQmOHEOZVyqHvebvvsVkpzDbovOpMqkkGhSpMpoPwS8FS+3EfcgEO2PkoF7dCutJaYWPr16cTNmOIhhxDEEi9JanKyIUhHLSc0AF4TFcffPzEKKjVuzSQBYk3JH4LZoczJIE574pdadKK+2ORiqqr7K0Rr+rh84IfGM1G4VJQn3sHE9tFlE+zgZKcQdqr9o11Z79/QGqiUkhHLSDIyzccqLX3V6R6VaEklkD0NO1hl+parKmsob6XoARtqVpraUXkk7JP8unmvHzlx3SLRxtD2WZB0vPea7K1mzHMQk51AXCWQj620713EOZQH712YONSxhhEnWHhD0V742Qj1hGhN9VU6XuqHE1U6iXZ32VfeY/5+VlK4/wxOW2bk0tc0r4KQsfpA45yhm9sql2jiiz0g7wx1K6PMzhxrUSl9dg5e9uDYTc4hTDo8/TdqAGcQ1tsoHu0A08K23DaTVRelJ+x+GE9edWKe58CybaMm7g5oCiatcbbtDIOIiSzjvVJqnumNzVYL11O1MenqhRSCm6TgHCUhkirom9AAf1Q1wpHWkrSWwtW13cBBJ9bj4vJW0t4hoSkuzbd1wG9faoTDhT2zbWegyqR94fLeSS+J7pL2Stva9g22cuLtmvKrWXTAXpJ1NvhvDuTFmIcU5hEyhQ9qVvbq69ld6VSCSF7owscdx70y+dK1G8mc5iTTn8MpBXHnVXvP07JBkOHF1sgfp+CDBb6vB3knMqskN/UCvfldrCr3J9+73JK5FZfCEDctjiQPMNxGdqdkrielWj7GTpmJQXn4XcSAxUX7ZgXWlZnLxvyKziBtu45jKbsHue2/8cNyU2X3LA+XtsjwgJRypk2eSIiFiB0J+GuavOVyg3Z/llwNJCJTTK50MtydfgLA//5OdfnV//uGxtg5L/t/hjLNxJCZgsMrZruf3RUWBZd2VQyCA1l6A2jLHCyBmmU4iZRMc8QJdO75qB9NX0ds9rWKg0lX2O/arLkpbv6+AQbqUpucP0yUGFlE+eOfQKptM3jjRdk2Fogwm2TfvKn852m/jutY7V3JHZxPDiSOioKrEy7dOqhcgZ6RgqZvgys6/78WMPHUoSp4qWyhSvOy9/nXwtvCEPaR9iN9K2n9qSot42ymz2gAAAABJRU5ErkJggg==';
var VEGETA_IMG = 'data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAE4AAABZCAMAAACnp+IMAAAABGdBTUEAALGPC/xhBQAAAAFzUkdCAdnJLH8AAAAgY0hSTQAAeiYAAICEAAD6AAAAgOgAAHUwAADqYAAAOpgAABdwnLpRPAAAACRQTFRFAAAAqqqqAFX/AACqAAAA////AFWqqlUAqqoAVVVV/6pV/6qqPqwUrgAAAAF0Uk5TAEDm2GYAAAABYktHRACIBR1IAAAACXBIWXMAAAsTAAALEwEAmpwYAAAAB3RJTUUH6gQVFQQ2WHeiLwAAAlxJREFUWMPt1+1uqzAMBmBiE9yE+7/f+Ssp3b/ZPtKks0xqaaU+exMHA8fxN/7Hcf5u7iwlhbOXIu7Ul7OMY6rIM8hHfg3Pp5dfwvPbKOZ+ebyjxpsl1V2Fnfd0syTbLdy8KTvbzbGnEfNrR+fUePdNVMHNOcYtAQcV9ADmWh+iNTrKPBqdRkmHIq3FPamk5xGxR3RTK+GQaEypa0Gy1wsRadIYhPTKYQCcjNjDOfmNjwHik0S4gONxPmAY5Pi6IKFdFg84H1i6sMdxhGs8+ODCSw75uwQHxiljR/wdBucKW7PRe09w7CEq0rfX5F9E09nSvTkemEi3StGeXKwWvDM2Rz64saQ5nqnsZR7MNYQop2tn3ErX4hzqtkMpp8VTTU6PWCX03DraXjx+H7p7ooWFxe3dgsHJrkpozPcYGGsCtu10yjZY4XkO/fhzzpb9g7uUkylHOFutTw6GLGOKwz1ZVK4XcJdqF3PybZjDdZ2QTZPi2uZ0mmjNPs5JCdEVXAexwrLnk/WFG490UU68Ks6871xs6dbiOQfCaVGCS+ceODe8FvFbFD1tn5we527tnnNNpvNLN1hj8gtlmvPG5BzkODOkKUN2tu/uibaKiEnOT9ZdlCx3SSLjMM2tnn742uXTXW54LSD+rNKcA1h1wWA/8R7lle1+BmuPSZy0va9EJZwC2uHeNyqpdMXcJv4B15Jc61tYt+4jwz0CDfsYjzc+8uyd0hLp+r50La7Ti+L77oPT2+6X/MWv3O3z0uvPGHkuOfxxokg7yB526jgS8SjzKsNZvLpwGu+Hv/gChJIWQk1rBKEAAAAASUVORK5CYII=';

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

    /* Character picker */
    '.char-picker{display:flex;gap:16px;justify-content:center;padding:12px 0}' +
    '.char-btn{' +
    '  background:#3a3a3c;' +
    '  border:2px solid transparent;' +
    '  border-radius:10px;' +
    '  color:#aaa;' +
    '  font-size:12px;' +
    '  font-weight:600;' +
    '  padding:10px 16px 8px;' +
    '  cursor:pointer;' +
    '  display:flex;flex-direction:column;align-items:center;gap:8px;' +
    '  -webkit-tap-highlight-color:transparent;' +
    '  transition:all .2s;' +
    '}' +
    '.char-btn img{width:52px;height:auto;image-rendering:pixelated}' +
    '.char-btn.active{border-color:#e8500a;color:#fff;background:#2a2a2c}' +

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
    '  <h1>⚡ DBZ Power Level</h1>' +
    '  <button class="save-btn" onclick="save()">Save</button>' +
    '</header>' +

    '<div class="section">' +
    '  <div class="section-title">Character</div>' +
    '  <div class="card">' +
    '    <div class="char-picker">' +
    '      <button class="char-btn" id="btnGoku" onclick="selectChar(0)">' +
    '        <img src="' + GOKU_IMG + '" alt="Goku">' +
    '        <span>Goku</span>' +
    '      </button>' +
    '      <button class="char-btn" id="btnVegeta" onclick="selectChar(1)">' +
    '        <img src="' + VEGETA_IMG + '" alt="Vegeta">' +
    '        <span>Vegeta</span>' +
    '      </button>' +
    '    </div>' +
    '  </div>' +
    '</div>' +

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
    'var defaults={stepsThreshold:7000,vibeOnBtDisconnect:true,character:0};' +
    'var settings=defaults;' +
    'try{' +
    '  var h=window.location.hash.slice(1);' +
    '  if(h)settings=JSON.parse(decodeURIComponent(h));' +
    '}catch(e){}' +

    'document.getElementById("stepsThreshold").value=settings.stepsThreshold||defaults.stepsThreshold;' +
    'document.getElementById("vibeOnBtDisconnect").checked=settings.vibeOnBtDisconnect!==false;' +

    'var s_char=settings.character||0;' +
    'document.getElementById(s_char===1?"btnVegeta":"btnGoku").classList.add("active");' +

    'function selectChar(idx){' +
    '  s_char=idx;' +
    '  document.querySelectorAll(".char-btn").forEach(function(b){b.classList.remove("active");});' +
    '  document.getElementById(idx===1?"btnVegeta":"btnGoku").classList.add("active");' +
    '}' +

    'function collect(){' +
    '  return{' +
    '    stepsThreshold:parseInt(document.getElementById("stepsThreshold").value,10)||defaults.stepsThreshold,' +
    '    vibeOnBtDisconnect:document.getElementById("vibeOnBtDisconnect").checked,' +
    '    character:s_char' +
    '  };' +
    '}' +

    'function save(){' +
    '  document.location="pebblejs://close#"+encodeURIComponent(JSON.stringify(collect()));' +
    '}' +

    'window.addEventListener("beforeunload",save);' +

    '</script>' +

    '</body></html>';
