#ifndef _WEATHER_H
#define _WEATHER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>

String getWeatherString();
String getForecastString();
String getQuoteString();

bool getWeatherData(const char* weatherApiKey, const char* weatherCityId);//pass the apikey and location
bool getForecastData(const char* forecastApiKey, const char* forecastLat, const char* forecastLon);//pass the apikey, Lat and Long
bool getQuoteData(const char* category);//pass the category

#endif
