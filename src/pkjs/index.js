var messageKeys = require('message_keys');
var htmlPage    = require('./settings');

var STORAGE_KEY = 'dbz-settings';
var DEFAULTS = {
  stepsThreshold:     7000,
  vibeOnBtDisconnect: true
};

function loadSettings() {
  try {
    return JSON.parse(localStorage.getItem(STORAGE_KEY)) || DEFAULTS;
  } catch (e) {
    return DEFAULTS;
  }
}

function saveSettings(settings) {
  localStorage.setItem(STORAGE_KEY, JSON.stringify(settings));
}

function sendToWatch(settings) {
  var msg = {};
  msg[messageKeys.STEPS_THRESHOLD]       = settings.stepsThreshold  | 0;
  msg[messageKeys.VIBE_ON_BT_DISCONNECT] = settings.vibeOnBtDisconnect ? 1 : 0;
  Pebble.sendAppMessage(
    msg,
    function()  { console.log('[DBZ] Settings sent to watch'); },
    function(e) { console.log('[DBZ] Failed to send settings: ' + JSON.stringify(e)); }
  );
}

Pebble.addEventListener('ready', function() {
  console.log('[DBZ] PebbleKit JS ready');
});

Pebble.addEventListener('showConfiguration', function() {
  var settings = loadSettings();
  var url = 'data:text/html,' + encodeURIComponent(htmlPage) +
            '#' + encodeURIComponent(JSON.stringify(settings));
  Pebble.openURL(url);
});

// Settings are applied when the user closes the page (← button or back gesture).
Pebble.addEventListener('webviewclosed', function(e) {
  if (!e.response) return;
  try {
    var settings = JSON.parse(decodeURIComponent(e.response));
    saveSettings(settings);
    sendToWatch(settings);
  } catch (err) {
    console.log('[DBZ] Failed to parse settings response: ' + err);
  }
});
