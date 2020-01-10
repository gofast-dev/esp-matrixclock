#include "apis.h"
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

//static WiFiClient client;
String  weatherMain        = "";
String  weatherString = "Weather No Yet Retreived";
String  weatherDescription = "";
String  weatherLocation    = "";
String  country;
int     owHumidity, pressure, precip, uv;
float   temp, tempMin, tempMax, windSpeed;
int     clouds, windDirection;
//String  date;



String  forecastString = "Forecast No Yet Retreived";
String  dateStr;




String quoteBody = "";
String quoteAuthor = "";
String quoteString = "Quote No Yet Retreived";

String getWeatherString(){
	return weatherString;
}
String getForecastString(){
	return forecastString;
}
String getQuoteString(){
	return quoteString;
}

bool getWeatherData(const char *weatherKey, const char* cityID){
	String url = String("https://api.openweathermap.org/data/2.5/weather?id=") + cityID + "&units=metric&appid=" + weatherKey + "&lang=en";
	Serial.println(String("GET " + url)); // debug info
	String line;
	if (WiFi.status() == WL_CONNECTED){ // Check WiFi connection status
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();	
		HTTPClient https;  // Declare an object of class HTTPClient
		https.begin(*client, url);  // Specify request destination
		int httpCode = https.GET(); //Send the request
		if (httpCode > 0) {// Check the returning code
			line = https.getString();   // Get the request response payload
		} else {
      weatherString=String("HTTP GET failed, error:") + https.errorToString(httpCode).c_str();
			Serial.println(weatherString);
			https.end();
			return false;
		}
		https.end();   //Close connection
		// Serial.print("Response :"); Serial.println(line); // debug info
	} else {
		weatherString="Wifi connection failed";
		Serial.println(weatherString);
		return false;
	}

  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(line);
  if (!root.success()) {
	  weatherString="parseObject() failed";
    Serial.println(weatherString);
    return false;
  }
  //weatherMain = root["weather"]["main"].as<String>();
  weatherDescription = root["weather"][0]["description"].as<String>();
  Serial.println("Description:"+weatherDescription);
  weatherDescription.toLowerCase();
  //weatherLocation = root["name"].as<String>();
  //country         = root["sys"]["country"].as<String>();
  temp           = root["main"]["temp"];
  owHumidity     = root["main"]["humidity"];
  pressure       = root["main"]["pressure"];
  tempMin        = root["main"]["temp_min"];
  tempMax        = root["main"]["temp_max"];
  windSpeed      = root["wind"]["speed"];
  windDirection  = root["wind"]["deg"];
  clouds         = root["clouds"]["all"];
  //String deg     = String(char('~'+25));
  weatherString  = "Outside Temperature:" + String(temp,1) + "c min:" + String(tempMin,1) + "c max:" + String(tempMax,1) + "c";
  weatherString += " Description: " + weatherDescription;
  weatherString += " Humidity: " + String(owHumidity) + "%";
  weatherString += " Pressure: " + String(pressure) + "hPa";
  weatherString += " Clouds: " + String(clouds) + "%";
  weatherString += " Wind: " + String(windSpeed*3.6,1) + "km/h";
  weatherString += " Direction:" + String(windDirection) + "deg" ;
  Serial.println(weatherString);
  return true;
}
bool getForecastData(const char *forecastApiKey, const char* lat, const char* lon){
//const char *forecastHost = "http://api.weatherbit.io";
/*
Field Decriptions:

    lat: Latitude (Degrees).
    lon: Longitude (Degrees).
    timezone: Local IANA Timezone.
    city_name: City name.
    country_code: Country abbreviation.
    state_code: State abbreviation/code.
    data: [
        valid_date: Date the forecast is valid for in format YYYY-MM-DD. [Midnight to midnight local time]
        ts: Forecast period start unix timestamp (UTC).
        datetime:[DEPRECATED] Forecast valid date (YYYY-MM-DD).
        wind_gust_spd: Wind gust speed (Default m/s).
        wind_spd: Wind speed (Default m/s).
        wind_dir: Wind direction (degrees).
        wind_cdir: Abbreviated wind direction.
        wind_cdir_full: Verbal wind direction.
        temp: Average Temperature (default Celcius).
        max_temp: Maximum Temperature (default Celcius).
        min_temp: Minimum Temperature (default Celcius).
        high_temp: High Temperature - Calculated from 6AM to 6AM local time (default Celcius).
        low_temp: Low Temperature - Calculated from 6AM to 6AM local (default Celcius).
        app_max_temp: Apparent/"Feels Like" temperature at max_temp time (default Celcius).
        app_min_temp: Apparent/"Feels Like" temperature at min_temp time (default Celcius).
        pop: Probability of Precipitation (%).
        precip: Accumulated liquid equivalent precipitation (default mm).
        snow: Accumulated snowfall (default mm).
        snow_depth: Snow Depth (default mm).
        pres: Average pressure (mb).
        slp: Average sea level pressure (mb).
        dewpt: Average dew point (default Celcius).
        rh: Average relative humidity (%).
        weather: {
            icon:Weather icon code.
            code:Weather code.
            description: Text weather description.
        }
        pod: Part of the day (d = day / n = night).
        clouds_low: Low-level (~0-3km AGL) cloud coverage (%).
        clouds_mid: Mid-level (~3-5km AGL) cloud coverage (%).
        clouds_hi: High-level (>5km AGL) cloud coverage (%).
        clouds: Average total cloud coverage (%).
        vis: Visibility (default KM).
        max_dhi: [DEPRECATED] Maximum direct component of solar radiation (W/m^2).
        uv: Maximum UV Index (0-11+).
        ozone: Average Ozone (Dobson units).
        moon_phase: Moon phase fraction (0-1).
        moonrise_ts: Moonrise time unix timestamp (UTC).
        moonset_ts: Moonset time unix timestamp (UTC).
        sunrise_ts: Sunrise time unix timestamp (UTC).
        sunset_ts: Sunset time unix timestamp (UTC) .
*/
	String url = "https://api.weatherbit.io/v2.0/forecast/daily?lat="+String(lat)+"&lon="+String(lon)+"&days=2&key="+String(forecastApiKey);
	String line;
	Serial.println(String("GET " + url)); // debug info
  
	if (WiFi.status() == WL_CONNECTED){ // Check WiFi connection status
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();	
		HTTPClient https;  // Declare an object of class HTTPClient
		https.begin(*client, url);  // Specify request destination
		int httpCode = https.GET(); //Send the request
		if (httpCode > 0) {// Check the returning code
			line = https.getString();   // Get the request response payload
		} else {
      forecastString=String("HTTPS GET failed, error:") + https.errorToString(httpCode).c_str();
			Serial.println(forecastString);
			https.end();
			return false;
		}
		https.end();   //Close connection
		// Serial.print("Response :"); Serial.println(line); // debug info
	} else {
		forecastString="Wifi connection failed";
		Serial.println(forecastString);
		return false;
	}

  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(line);
  if (!root.success()){
	  forecastString="forcast parseObject() failed";
    Serial.println(forecastString);
    return false;
  }
  //weatherMain = root["weather"]["main"].as<String>();
  //weatherLocation = root["name"].as<String>();
  //country         = root["sys"]["country"].as<String>();

  dateStr            = root["data"][0]["valid_date"].as<String>();
  weatherDescription = root["data"][0]["weather"]["description"].as<String>();
  Serial.println("Description:"+weatherDescription);
  weatherDescription.toLowerCase();
  owHumidity         = root["data"][0]["rh"];//avg
  pressure           = root["data"][0]["pres"];//avg
  tempMin            = root["data"][0]["min_temp"];
  tempMax            = root["data"][0]["max_temp"];
  windSpeed          = root["data"][0]["wind_spd"];
  windDirection      = root["data"][0]["wind_dir"];
  precip             = root["data"][0]["pop"];
  uv                 = root["data"][0]["uv"];

  forecastString  = String("Forcast:") + String(dateStr) + " min:" + String(tempMin,1) + "c max:" + String(tempMax,1) + "c";
  forecastString += " Precipitation:" + String(precip) + "%" ;
  forecastString += " UV Index(1-11+):" + String(uv);
  forecastString += " Description: " + weatherDescription;
  forecastString += " Humidity: " + String(owHumidity) + "%";
  forecastString += " Pressure: " + String(pressure) + "hPa";
  forecastString += " Wind: " + String(windSpeed*3.6,1) + "km/h";
  forecastString += " Direction:" + String(windDirection) + "deg" ;


  dateStr            = root["data"][1]["valid_date"].as<String>();
  weatherDescription = root["data"][1]["weather"]["description"].as<String>();
  Serial.println("Description:"+weatherDescription);
  weatherDescription.toLowerCase();
  owHumidity         = root["data"][1]["rh"];//avg
  pressure           = root["data"][1]["pres"];//avg
  tempMin            = root["data"][1]["min_temp"];
  tempMax            = root["data"][1]["max_temp"];
  windSpeed          = root["data"][1]["wind_spd"];
  windDirection      = root["data"][1]["wind_dir"];
  precip             = root["data"][1]["pop"];
  uv                 = root["data"][1]["uv"];

  forecastString += String(" Next Day:") + String(dateStr) + " min:" + String(tempMin,1) + "c max:" + String(tempMax,1) + "c";
  forecastString += " Precipitation:" + String(precip) + "%" ;
  forecastString += " UV Index(1-11+):" + String(uv);
  forecastString += " Description: " + weatherDescription;
  forecastString += " Humidity: " + String(owHumidity) + "%";
  forecastString += " Pressure: " + String(pressure) + "hPa";
  forecastString += " Wind: " + String(windSpeed*3.6,1) + "km/h";
  forecastString += " Direction:" + String(windDirection) + "deg" ;

  Serial.println(forecastString);
  return true;
}

bool getQuoteData(const char* category){
	String url = String("https://quotes.rest/qod.json?category=") + category;
	String line;
	// Serial.println(String("GET " + url)); // debug info

	if (WiFi.status() == WL_CONNECTED) {// Check WiFi connection status
    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);
    client->setInsecure();	
	  HTTPClient https;  // Declare an object of class HTTPClient
		https.begin(*client, url);  // Specify request destination
		int httpCode = https.GET(); //Send the request
		if (httpCode > 0) { // Check the returning code
			line = https.getString();   // Get the request response payload
		} else {
      quoteString=String("HTTPS GET failed, error:") + https.errorToString(httpCode).c_str();
			Serial.println(quoteString);
			https.end();
			return false;
		}
		https.end();   //Close connection
		// Serial.print("Response :"); Serial.println(line); // debug info
	} else {
		quoteString="Wifi connection failed";
		Serial.println(quoteString);
		return false;
	}

  DynamicJsonBuffer jsonBuf;
  JsonObject &root = jsonBuf.parseObject(line);
  if (!root.success()){
	  quoteString="parseObject() failed";
    Serial.println(quoteString);
    return false;
  }
  quoteBody   = root["contents"]["quotes"][0]["quote"].as<String>();
  Serial.println("quoteBody:"+quoteBody);
  quoteAuthor = root["contents"]["quotes"][0]["author"].as<String>();
  quoteString = String("Theysaidso.com ") + category + ":" + quoteBody + " Author:" + quoteAuthor;
  Serial.println(quoteString);
  return true;
}