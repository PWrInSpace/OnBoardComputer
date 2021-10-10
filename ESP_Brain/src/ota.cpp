#include "ota.h"

WebServer serverOta(80);
bool useOta = false;

String serverIndex = 
    "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
    "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
    "<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
    "<label id='file-input' for='file'>   Wybierz plik firmware.bin</label>"
    "<input type='submit' class=btn value='Update'>"
    "<br><br>"
    "<div id='prg'></div>"
    "<br><div id='prgbar'><div id='bar'></div></div><br></form>"
    "<script>"
    "function sub(obj){"
    "var fileName = obj.value.split('\\\\');"
    "document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"
    "};"
    "$('form').submit(function(e){"
    "e.preventDefault();"
    "var form = $('#upload_form')[0];"
    "var data = new FormData(form);"
    "$.ajax({"
    "url: '/update',"
    "type: 'POST',"
    "data: data,"
    "contentType: false,"
    "processData:false,"
    "xhr: function() {"
    "var xhr = new window.XMLHttpRequest();"
    "xhr.upload.addEventListener('progress', function(evt) {"
    "if (evt.lengthComputable) {"
    "var per = evt.loaded / evt.total;"
    "$('#prg').html('progress: ' + Math.round(per*100) + '%, ' + evt.loaded + ' / ' + evt.total);"
    "}"
    "}, false);"
    "return xhr;"
    "},"
    "success:function(d, s) {"
    "console.log('success!') "
    "},"
    "error: function (a, b, c) {"
    "}"
    "});"
    "});"
    "</script>";

void initOtaSerwer() {

    WiFi.begin(SSID, PASS);

    int8_t timeoutOta = 20;
    while (WiFi.status() != WL_CONNECTED && timeoutOta > 0) {
        delay(500);
        timeoutOta--;
    }
    if (timeoutOta > 0) useOta = true;

    Serial.println(WiFi.localIP());

    MDNS.begin(HOST_NAME); //http://esp32.local

    serverOta.on("/", HTTP_GET, []() {
        serverOta.sendHeader("Connection", "close");
        serverOta.send(200, "text/html", serverIndex);
    });
    /*handling uploading firmware file */
    serverOta.on("/update", HTTP_POST, []() {
        serverOta.sendHeader("Connection", "close");
        serverOta.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
        ESP.restart();
    }, []() {
        HTTPUpload& upload = serverOta.upload();
        if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
            Update.printError(Serial);
        }
        } else if (upload.status == UPLOAD_FILE_WRITE) {
        /* flashing firmware to ESP*/
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
        }
        } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
            Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
        } else {
            Update.printError(Serial);
        }
        }
    });
}