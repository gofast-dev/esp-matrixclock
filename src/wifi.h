#ifndef WIFI_H
#define	WIFI_H

#define WEATHER_APIKEY_LEN	    40
#define WEATHER_CITYID_LEN		12
#define FORECAST_APIKEY_LEN	    40
#define FORECAST_LAT_LEN         12
#define FORECAST_LON_LEN         12

extern char weatherApiKey[WEATHER_APIKEY_LEN];
extern char weatherCityId[WEATHER_CITYID_LEN];
extern char forecastApiKey[FORECAST_APIKEY_LEN];
extern char forecastLat[FORECAST_LAT_LEN];
extern char forecastLon[FORECAST_LON_LEN];

void setupWiFi(void);
void resetWiFi(void);

#endif /* WIFI_H */
