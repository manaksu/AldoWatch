Pebble.addEventListener('ready', function() {
  console.log('PebbleKit JS ready');
});

Pebble.addEventListener('showConfiguration', function() {
  var theme = localStorage.getItem('aldo_theme') || '0';
  var html = '<html><head><meta name="viewport" content="width=device-width,initial-scale=1"/>'
    + '<style>'
    + 'body{font-family:sans-serif;background:#1a1a1a;color:#eee;padding:20px;margin:0;}'
    + 'h2{margin-bottom:4px;}p{color:#aaa;font-size:13px;margin-bottom:20px;}'
    + 'label{display:block;margin-bottom:6px;font-size:14px;color:#ccc;}'
    + 'select{width:100%;padding:10px;font-size:15px;margin-bottom:24px;'
    + 'background:#2a2a2a;color:#eee;border:1px solid #444;border-radius:8px;}'
    + 'button{width:100%;padding:14px;background:#4a90d9;color:#fff;border:none;'
    + 'border-radius:8px;font-size:16px;font-weight:bold;cursor:pointer;}'
    + '</style></head><body>'
    + '<h2>Aldo Watchface</h2>'
    + '<p>Choose your color theme</p>'
    + '<label>Theme</label>'
    + '<select id="t">'
    + '<option value="0"' + (theme==='0'?' selected':'') + '>Cream — White + Gray</option>'
    + '<option value="1"' + (theme==='1'?' selected':'') + '>ePaper — Black + White</option>'
    + '<option value="2"' + (theme==='2'?' selected':'') + '>Slate — Dark Gray</option>'
    + '<option value="3"' + (theme==='3'?' selected':'') + '>Warm — Yellow</option>'
    + '<option value="4"' + (theme==='4'?' selected':'') + '>Mint — Green</option>'
    + '</select>'
    + '<button onclick="save()">Save Settings</button>'
    + '<script>'
    + 'function save(){'
    + 'var v=document.getElementById("t").value;'
    + 'localStorage.setItem("aldo_theme",v);'
    + 'location.href="pebblejs://close#"+encodeURIComponent(JSON.stringify({THEME:parseInt(v)}));'
    + '}'
    + '</script></body></html>';
  Pebble.openURL('data:text/html,' + encodeURIComponent(html));
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (!e.response || e.response === 'CANCELLED') return;
  try {
    var config = JSON.parse(decodeURIComponent(e.response));
    var msg = { 1: config.THEME };  /* key 1 = MESSAGE_KEY_THEME */
    Pebble.sendAppMessage(msg,
      function() { console.log('Theme sent: ' + config.THEME); },
      function(err) { console.log('Send failed: ' + JSON.stringify(err)); }
    );
  } catch(e) {
    console.log('Parse error: ' + e);
  }
});
