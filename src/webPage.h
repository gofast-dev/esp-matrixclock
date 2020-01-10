void startWebServer();
void handleWiFi(time_t now2, float temp, float humidity, String sWeatherMessage, String sForecastMessage, String sQuoteMessage);
boolean newWebMessage();
char* getWebMessage();
bool getSectionEnabled(int);
uint getIntensity();
uint getMsgCount();
uint getMsgSpeed();