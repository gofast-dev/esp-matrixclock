/********************************************
 * WifiManager code with FS parameters (unused yet)
 * based on tzapu's filesystem code
 * https://github.com/tzapu/WiFiManager
 ********************************************/

#include <FS.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ArduinoJson.h> // https://github.com/bblanchon/ArduinoJson
#include "wifi.h"
//#include "display.h"

#define CONFIGAP_SSID

//define your default values here, if there are different values in config.json, they are overwritten.
char weatherApiKey[WEATHER_APIKEY_LEN] = "openweathermaps API_Key";
char weatherCityId[WEATHER_CITYID_LEN] = "City_ID";
char forecastApiKey[FORECAST_APIKEY_LEN] = "weatherbit.io API_Key";
char forecastLat[FORECAST_LAT_LEN] = "Latitude";
char forecastLon[FORECAST_LON_LEN] = "Longitude";


const char weatherApiKeyJson[] = "owApiKey";
const char weatherCityIdJson[] = "owCityId";
const char forecastApiKeyJson[] = "wbApiKey";
const char forecastLatJson[]= "wbLat";
const char forecastLonJson[]= "wbLon";



//flag for saving data
bool shouldSaveConfig = false;

//callback notifying us of the need to save config
void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void resetWiFi() {
//  drawReset();
	Serial.println("Disconnecting WiFi");
	WiFi.persistent(false);      
	WiFi.disconnect(true);
	delay(1000);
	WiFi.persistent(true);
	Serial.println("Reset settings");
	WiFiManager wifiManager;
	wifiManager.resetSettings();
	delay(2000);
	Serial.println("Resetting ESP");
	ESP.eraseConfig();
	delay(2000);
	ESP.reset();
}

void setupWiFi() {
  // read configuration from FS json
  Serial.println("mounting FS...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      // file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          Serial.println("AsStr"+json["weatherApiKey"].as<String>());
          if (json[weatherApiKeyJson].as<String>()>"0000"){
            strcpy(weatherApiKey, json[weatherApiKeyJson]);
            strcpy(weatherCityId, json[weatherCityIdJson]);
            strcpy(forecastApiKey, json[forecastApiKeyJson]);
            strcpy(forecastLat, json[forecastLatJson]);
            strcpy(forecastLon, json[forecastLonJson]);
            Serial.println("copied json");
          }else{
            Serial.println("failed json");
          }
        } else {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
		else {
      Serial.println("config.json does not exist");
		}
  } else {
    Serial.println("failed to mount FS");
  }
  //end read

  // The extra parameters to be configured (can be either global or just in the setup)
  // After connecting, parameter.getValue() will get you the configured value
  // id/name placeholder/prompt default length
	WiFiManagerParameter customOwApiKey("owApiKey", "Openweathermap API key", weatherApiKey, WEATHER_APIKEY_LEN);
	WiFiManagerParameter customOwCityId("owCityId", "Openweathermap City ID", weatherCityId, WEATHER_CITYID_LEN);

	WiFiManagerParameter customWbApiKey(forecastApiKeyJson, ".weatherbit.io API key", forecastApiKey, FORECAST_APIKEY_LEN);
  WiFiManagerParameter customWbLat(forecastLatJson, "City Lat", forecastLat, FORECAST_LAT_LEN);
  WiFiManagerParameter customWbLon(forecastLonJson, "City Lon", forecastLon, FORECAST_LON_LEN);

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  // set static ip
  // wifiManager.setSTAStaticIPConfig(IPAddress(10,0,1,99), IPAddress(10,0,1,1), IPAddress(255,255,255,0));
  
  //add all your parameters here
  wifiManager.addParameter(&customOwApiKey);
  wifiManager.addParameter(&customOwCityId);
  wifiManager.addParameter(&customWbApiKey);
  wifiManager.addParameter(&customWbLat);
  wifiManager.addParameter(&customWbLon);

	WiFi.mode(WIFI_AP_STA);

  if (!wifiManager.autoConnect("MatrixAP")) {
    Serial.println("failed to connect and hit timeout");
    delay(3000);
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(5000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");

  // read updated parameters
  strcpy(weatherApiKey, customOwApiKey.getValue());
  strcpy(weatherCityId, customOwCityId.getValue());
  strcpy(forecastApiKey, customWbApiKey.getValue());
  strcpy(forecastLat, customWbLat.getValue());
  strcpy(forecastLon, customWbLon.getValue());

  // save the custom parameters to FS
  if (shouldSaveConfig) {
    Serial.println("saving config");
    DynamicJsonBuffer jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json[weatherApiKeyJson] = weatherApiKey;
    json[weatherCityIdJson] = weatherCityId;
    json[forecastApiKeyJson] = forecastApiKey;
    json[forecastLatJson] = forecastLat;
    json[forecastLonJson] = forecastLon;

    File configFile = SPIFFS.open("/config.json", "w");
    if (!configFile) {
      Serial.println("failed to open config file for writing");
    }

    json.printTo(Serial);
    json.printTo(configFile);
    configFile.close();
    //end save
  }

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Serial.print("weatherApiKey:");
  Serial.println(weatherApiKey);
  Serial.print("weatherCityId:");
  Serial.println(weatherCityId);
  Serial.print("forecastApiKey:");
  Serial.println(forecastApiKey);
  Serial.print("forecastLat:");
  Serial.println(forecastLat);
  Serial.print("forecastLon:");
  Serial.println(forecastLon);
}
