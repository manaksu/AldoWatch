Pebble.addEventListener('showConfiguration', function() {
  var theme = localStorage.getItem('theme') || '0';
  var html = '<html><head><meta name="viewport" content="width=device-width, initial-scale=1"/>'
    + '<style>body{font-family:sans-serif;background:#1a1a1a;color:#eee;padding:20px;}'
    + 'h2{margin-bottom:20px;}select{width:100%;padding:10px;font-size:16px;margin:10px 0 20px;'
    + 'background:#333;color:#eee;border:1px solid #555;border-radius:6px;}'
    + 'button{width:100%;padding:14px;background:#4a90d9;color:#fff;border:none;'
    + 'border-radius:6px;font-size:16px;cursor:pointer;}</style></head>'
    + '<body><h2>Aldo Watchface</h2><label>Color Theme</label>'
    + '<select id="theme">'
    + '<option value="0"' + (theme=="0"?" selected":"") + '>Cream (White + Gray)</option>'
    + '<option value="1"' + (theme=="1"?" selected":"") + '>ePaper (Black + White)</option>'
    + '<option value="2"' + (theme=="2"?" selected":"") + '>Slate (Dark Gray)</option>'
    + '<option value="3"' + (theme=="3"?" selected":"") + '>Warm (Yellow)</option>'
    + '<option value="4"' + (theme=="4"?" selected":"") + '>Mint (Green)</option>'
    + '</select>'
    + '<button onclick="save()">Save</button>'
    + '<script>function save(){'
    + 'var v=document.getElementById("theme").value;'
    + 'localStorage.setItem("theme",v);'
    + 'location.href="pebblejs://close#"+encodeURIComponent(JSON.stringify({THEME:parseInt(v)}));'
    + '}</script></body></html>';
  Pebble.openURL('data:text/html,' + encodeURIComponent(html));
});

Pebble.addEventListener('webviewclosed', function(e) {
  if (e.response) {
    try {
      var config = JSON.parse(decodeURIComponent(e.response));
      Pebble.sendAppMessage(config, function() {
        console.log('Settings sent');
      }, function(e) {
        console.log('Settings failed: ' + JSON.stringify(e));
      });
    } catch(err) {
      console.log('Error: ' + err);
    }
  }
});
