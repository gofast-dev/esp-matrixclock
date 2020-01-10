#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WebPage.h>
#include "config.h"

//#define DEBUG true
#if DEBUG
#define PRINT(s, v) { Serial.print(F(s)); Serial.print(v); }
#define PRINTS(s)   { Serial.print(F(s)); }
#else
#define PRINT(s, v)
#define PRINTS(s)
#endif
// WiFi Server object and parameters
WiFiServer server(80);
static WiFiClient sClient;

const uint8_t MESG_SIZE = 255;
char message[MESG_SIZE];
bool messageAvailable = false;
const char WebResponse[] = "HTTP/1.1 200 OK\nContent-Type: text/html\n\n";

const uint sizeOfSection=5;
const String sectionName[sizeOfSection] = {"Sensor","Clock","Weather","Quote","Message"};
static bool sectionEnabled[sizeOfSection] = {true,true,true,true,true};

uint intensity = 8;
const uint intesitySize = 3;
const String intensityLabels[intesitySize] = {"Low","Medium","High"};
const int intensityValues[intesitySize] = {0,2,8};

uint msgCount=1;
const uint msgCountSize = 3;
const int msgCountValues[msgCountSize] = {1,2,9};

uint msgSpeed = 4;
const uint msgSpeedSize = 5;
const String msgSpeedLabels[msgSpeedSize] = {"slowest","slow","normal","fast","fastest"};
const int msgSpeedValues[msgSpeedSize] = {9,7,4,2,1};

const char WebPage[] =
"<!DOCTYPE html\n>" \
"<html>\n" \
"<head>\n" \
"<title>" \
WEB_TITLE
"</title>\n" \

"<script>\n" \
"strLine = \"\";\n" \
"function SendText(){\n" \
"  nocache = \"/&nocache=\" + Math.random() * 1000000;\n" \
"  var request = new XMLHttpRequest();\n" \
"  strLine = \"&MESG=\" + document.getElementById(\"txt_form\").Message.value;\n" \
"  request.open(\"GET\", strLine + nocache, false);\n" \
"  request.send(null);\n" \
"}\n" \
"</script>\n" \

"<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n" \
"<link rel=\"icon\" href=\"data:,\">\n" \
"<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n" \
".frm { color: black; padding: 16px 40px; text-decoration: none; font-size: 30px; margin: 2px;}\n" \
".button { border: none; color: white; padding: 16px 40px;\n" \
"text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}\n" \
".redBtnOn { background-color: #C00000;}\n" \
".redBtnOff { background-color: #700000;}\n" \
".grnBtnOn {background-color: #00A000;}\n" \
".grnBtnOff {background-color: #004000;}\n</style>\n" \
"</head>\n" \

"<body>\n" \
"<h1>" \
WEB_TITLE
"</h1>";


void sendWebpage(WiFiClient &client, time_t now, float temp, float humidity, String sWeatherMessage, String sForecastMessage, String sQuoteMessage){
  client.println(WebPage);
 // Print Date
  if(now){//not null
    struct tm* p_tm = localtime(&now);
    client.print("<h1>Date ");
    client.printf("%02d",(p_tm->tm_mday));
    client.printf("/%02d",(p_tm->tm_mon + 1));
    client.printf("/%02d",(p_tm->tm_year + 1900));
    client.println("</h1><h1>Time ");
// Print time
    client.printf(" %02d",((p_tm->tm_hour)>12 ? (p_tm->tm_hour)-12 : (p_tm->tm_hour)));
    client.printf(":%02d",(p_tm->tm_min));
    client.printf(":%02d",(p_tm->tm_sec)); 
    client.print((p_tm->tm_hour)>12?" PM":" AM");
    client.println("</h1>");
  }
        
  client.print("<h1>Temperature ");
  client.print(temp,1);
  client.println("&deg;C </h1>");

  client.print("<h1>Humidity ");
  client.print(humidity,1);
  client.println("% </h1>");

  client.println("<h1>Weather</h1>");
  client.println(sWeatherMessage);
  client.println("<h1>Forecast</h1>");
  client.println(sForecastMessage);
  client.println("<h1>Quote of the day</h1>");
  client.println(sQuoteMessage);

  client.println("<form class=frm id=\"txt_form\" name=\"frmText\">");
  client.println("<label>Enter a Message:<input type=\"text\" class=frm name=\"Message\" maxlength=\"255\"></label><br><br>");
  client.println("<input class=\"button grnBtnOff\" type=\"submit\" value=\"Send Text\" onclick=\"SendText()\">");
  client.println("</form>");
//check boxes
  client.println("<h1>");
  for(uint i=0;i<sizeOfSection;i++){
    client.print("<br><label><input type=\"checkbox\" class=frm id=\"");
    client.print(sectionName[i]);
    client.print("Enable\" checked=\"checked\">");
    client.print(sectionName[i]);
    client.println("</label>");
  }
  client.println("</h1>");

  client.println("<script>");
// add listeners
  for(uint i=0;i<sizeOfSection;i++){
    client.print("document.getElementById('");
    client.print(sectionName[i]);
    client.println("Enable').addEventListener('click', sendChecks, false);");
  }
// add function
  client.println("function sendChecks(){");
  client.println("  strLine = \"\";");
  client.println("  nocache = \"/&nocache=\" + Math.random() * 1000000;");
  client.println("  var request = new XMLHttpRequest();");
  for(uint i=0;i<sizeOfSection;i++){
    client.print("  strLine = strLine + \"&");
    client.print(sectionName[i]);
    client.print("Enable=\"+((document.getElementById(\"");
    client.print(sectionName[i]);
    client.println("Enable\").checked)?1:0);");
  }
  client.println("  request.open(\"GET\", strLine + nocache, false);");
  client.println("  request.send(null);");
  client.println("}");
  client.println("</script>");

  client.println("<br><br>Brightness");
  for(uint i=0;i<intesitySize;i++){
    client.print("<a href=\"/&BRIGHT=");
    client.print(intensityValues[i]);
    client.print("b\"><button class=\"button grnBtnOff\">");
    client.print(intensityLabels[i]);
    client.println("</button></a>");
  }

  client.println("<br><br>How many times to display Message?");
  for(uint i=0;i<msgCountSize;i++){
    client.print("<a href=\"/&MSGCNT=");
    client.print(msgCountValues[i]);
    client.print("b\"><button class=\"button grnBtnOff\">");
    client.print(msgCountValues[i]);
    client.println("</button></a>");
  }

  client.println("<br><br>Scroll Speed");
  for(uint i=0;i<msgSpeedSize;i++){
    client.print("<a href=\"/&MSGSPD=");
    client.print(msgSpeedValues[i]);
    client.print("b\"><button class=\"button grnBtnOff\">");
    client.print(msgSpeedLabels[i]);
    client.println("</button></a>");
  }

  client.println("</body>\n</html>");
  // The HTTP response ends with another blank line
  client.println();

}

uint8_t hexToInt(char c){
  c = toupper(c);
  if ((c >= '0') && (c <= '9')) return(c - '0');
  if ((c >= 'A') && (c <= 'F')) return(c - 'A' + 0xa);
  return(0);
}

boolean getText(char const* szMesg, char *psz, uint8_t len){
  boolean isValid = false;  // text received flag
  char *pStart, *pEnd;      // pointer to start and end of text

  // get pointer to the beginning of the text
  pStart = strstr(szMesg, "/&MESG=");
  if (pStart != NULL){
    pStart += 7;  // skip to start of data
    pEnd = strstr(pStart, "/&");

    if (pEnd != NULL){
      while (pStart != pEnd){
        if ((*pStart == '%') && isdigit(*(pStart+1))){
          // replace %xx hex code with the ASCII character
          char c = 0;
          pStart++;
          c += (hexToInt(*pStart++) << 4);
          c += hexToInt(*pStart++);
          *psz++ = c;
        }else *psz++ = *pStart++;
      }
      Serial.println("New MESG!");
      *psz = '\0'; // terminate the string
      isValid = true;
    }
  }

  pStart = strstr(szMesg, "/&BRIGHT=");
  if (pStart != NULL){
    Serial.println("bright!");
    pStart += 9;  // skip to start of data
    pEnd = strstr(pStart, "b");
    if (pEnd != NULL)
    if(*pStart>='0' && *pStart<='8'){
      intensity=*pStart-'0';
    }
  }

  pStart = strstr(szMesg, "/&MSGCNT=");
  if (pStart != NULL){
    Serial.println("Message count!");
    pStart += 9;  // skip to start of data
    pEnd = strstr(pStart, "b");
    if (pEnd != NULL)
    if(*pStart>='0' && *pStart<='9'){
      msgCount=*pStart-'0';
    }
  }

  pStart = strstr(szMesg, "/&MSGSPD=");
  if (pStart != NULL){
    Serial.println("Message speed!");
    pStart += 9;  // skip to start of data
    pEnd = strstr(pStart, "b");
    if (pEnd != NULL)
    if(*pStart>='0' && *pStart<='9'){
      msgSpeed=*pStart-'0';
    }
  }


  for(uint i=0;i<sizeOfSection;i++){
    String s = "&" + sectionName[i] + "Enable";
    pStart = strstr(szMesg, s.c_str());
    if (pStart != NULL){
      Serial.print(sectionName[i]);
      Serial.print(i);
      Serial.print(" found=");
      pStart += s.length()+1;  // skip to start of data
      if(*pStart=='1'){
        sectionEnabled[i]=true;
        Serial.println("true");
      }else if(*pStart=='0'){
        sectionEnabled[i]=false;
        Serial.println("false");
      }else{
        Serial.println(" invalid");
      }
    }
  }

  return(isValid);
}

void startWebServer(){
  server.begin();
}
uint getIntensity(){
  return intensity;
}
uint getMsgCount(){
  return msgCount;
}
uint getMsgSpeed(){
  return msgSpeed;
}
bool getSectionEnabled(int i){
  return sectionEnabled[i];
}


void handleWiFi(time_t now, float temp, float humidity, String sWeatherMessage, String sForcastMessage, String sQuoteMessage){
// Variable to store the HTTP request
  String httpRequest;
  IPAddress clientIP;
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    clientIP = client.remoteIP();
    Serial.print("Request recieved from ");
    Serial.println(clientIP);
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        httpRequest += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP httpRequests always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:

            // Process the request
            messageAvailable = getText(httpRequest.c_str(), message, MESG_SIZE);

            // send a response/wepage
            client.print(WebResponse);
            sendWebpage(client, now, temp, humidity, sWeatherMessage, sForcastMessage, sQuoteMessage);

            // Break out of the while loop
            break;
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }

    // Clear the httpRequest variable
    httpRequest = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}



void handleWiFiOld(time_t now, float temp, float humidity, String sWeatherMessage, String sForcastMessage, String sQuoteMessage){
  static enum { S_IDLE, S_WAIT_CONN, S_READ, S_EXTRACT, S_RESPONSE, S_DISCONN } state = S_IDLE;
  static char szBuf[1024];
  static uint16_t idxBuf = 0;
  static uint32_t timeStart;

  switch (state){
    case S_IDLE:   // initialize
      PRINTS("\nS_IDLE");
      idxBuf = 0;
      state = S_WAIT_CONN;
      break;

    case S_WAIT_CONN:   // waiting for connection
    {
      sClient = server.available();
      if (!sClient) break;
      if (!sClient.connected()) break;

#if DEBUG
      char szTxt[20];
      sprintf(szTxt, "%03d:%03d:%03d:%03d", sClient.remoteIP()[0], sClient.remoteIP()[1], sClient.remoteIP()[2], sClient.remoteIP()[3]);
      PRINT("\nNew client @ ", szTxt);
#endif

      timeStart = millis();
      state = S_READ;
    }
    break;

  case S_READ: // get the first line of data
    PRINTS("\nS_READ");
    while (sClient.available()){
      char c = sClient.read();
      if ((c == '\r') || (c == '\n')){
        szBuf[idxBuf] = '\0';
        sClient.flush();
        PRINT("\nRecv: ", szBuf);
        state = S_EXTRACT;
      } else szBuf[idxBuf++] = (char)c;
    }
    if (millis() - timeStart > 10000) {
      PRINTS("\nWait timeout");
      state = S_DISCONN;
    }
    break;

  case S_EXTRACT: // extract data
    PRINTS("\nS_EXTRACT");
    // Extract the string from the message if there is one
    messageAvailable = getText(szBuf, message, MESG_SIZE);
    PRINT("\nNew Msg: ", message);
    state = S_RESPONSE;
    break;

  case S_RESPONSE:
  { // send the response to the client
    PRINTS("\nS_RESPONSE");
    // Return the response to the sClient (web page)
    sClient.print(WebResponse);
    sendWebpage(sClient, now, temp, humidity, sWeatherMessage, sForcastMessage, sQuoteMessage);
    state = S_DISCONN;
    break;
  }
  case S_DISCONN: // disconnect client
    PRINTS("\nS_DISCONN");
    sClient.flush();
    sClient.stop();
    state = S_IDLE;
    break;

  default:  state = S_IDLE;
  }
}

boolean newWebMessage(){
  return messageAvailable;
}

char* getWebMessage(){
  messageAvailable=false;
  return message;
}