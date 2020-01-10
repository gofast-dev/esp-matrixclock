/*
 * ESP8266 driven matrix clock with weather
 * (c)2016-2018 Adam Kovesdi
 * version 3
 * 
 * Extensivly modified by Tony Phibbs
 * (c)2020 Tony Phibbs
 * Added scrolling display
 * Added webpage - customise plus dimming, scroll speed, text message etc.
 * Added quote of the day
 * Added sensor DHT
 * 
 */

#include <Arduino.h>
#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <Time.h>

#include "wifi.h"
#include "ntp.h"
#include "fonts.h"
//#include "max7219.h"
#include "esp8266DHT.h"
#include "clock.h"
#include "apis.h"
#include "webPage.h"
#include "MD_MAX72xx.h"
#include "config.h"

// ntp stuff
NTP NTPclient;

// weather related stuff
time_t lastWeatherApiUpdate = 0; // last weather update EPOCH timestamp
time_t lastForecastApiUpdate = 0; // last forecast update EPOCH timestamp
time_t lastQuoteApiUpdate = 0; // last quote update EPOCH timestamp

bool ntpSynced = false;
bool clockDisplayed= false;//so we know to clear it.
bool reset_trigger = false;
bool gotWebIP = false;
long lastDHTRead; //current time of last poll

uint currentIntensity=getIntensity();
uint maxMsgCount=getMsgCount();
uint currentMsgSpeed=getMsgSpeed();
uint currentMsgCount=getMsgCount();
unsigned long clockStarted=millis();
unsigned long clockDuration=currentMsgSpeed*CLOCK_DISP_DURATION;//ms - this times currentMsgSpeed

enum DisplayStates {DISPLAY_START=0, DISPLAY_DHT=0, DISPLAY_CLOCK=1, DISPLAY_WEATHER=2, DISPLAY_FORECAST=3, DISPLAY_QUOTE=4, DISPLAY_MESSAGE=5, DISPLAY_END=6, DISPLAY_RESET=99 };
static DisplayStates displayState,currentDisplayState = DISPLAY_DHT;

const uint quoteCategorySize=8;
String quoteCategory[quoteCategorySize] = {"inspire","management","sports","life","funny","love","art","students"};//longest string is 10 chars
uint quoteCategoryIndex=0;
/*--------------------------
    New MD MAX72xxstuff!!!
  --------------------------*/
const uint MESG_SIZE = 1024;
const uint8_t CHAR_SPACING = 1;

uint8_t scrollDelay = currentMsgSpeed*SCROLL_DELAY_MULTIPLYER;

char curMessage[MESG_SIZE];
static char *pCurMessage;
char webMessage[MESG_SIZE];

#if DEBUG
#define PRINT(s, v) { Serial.print(F(s)); Serial.print(v); }
#define PRINTS(s)   { Serial.print(F(s)); }
#else
#define PRINT(s, v)
#define PRINTS(s)
#endif

// Arbitrary pins
MD_MAX72XX mx = MD_MAX72XX(HARDWARE_TYPE, DATA_PIN, CLK_PIN, CS_PIN, MAX_DEVICES);

time_t getNTPtime(void)
{
	return NTPclient.getNtpTime();
}

void refreshWeather()
{
	if (now() > (lastWeatherApiUpdate + WEATHER_POLL_INTERVAL))
	{
    Serial.println("get Weather Data");
		if (getWeatherData(weatherApiKey, weatherCityId)) lastWeatherApiUpdate = now();
    Serial.println("got Weather Data");
	}
}

void displayWeather()
{
	refreshWeather();
//	int t = getTemp();
	String weatherStr = getWeatherString();
  strcpy(curMessage, weatherStr.c_str());
}

void refreshForecast(){
  Serial.printf("get Forecast Data now %ld and last %ld and then when to do %ld \n", now(), lastForecastApiUpdate, (lastForecastApiUpdate + FORECAST_POLL_INTERVAL));

	if (now() > (lastForecastApiUpdate + FORECAST_POLL_INTERVAL)){
    Serial.println("get it");
		if(getForecastData(forecastApiKey, forecastLat, forecastLon)) lastForecastApiUpdate = now();
    Serial.println("got Forecast Data");
    Serial.printf("got Forecast Data now %ld and last %ld and then when to do %ld \n", now(), lastForecastApiUpdate, (lastForecastApiUpdate + FORECAST_POLL_INTERVAL));
	}
}

void displayForecast()
{
	refreshForecast();
//	int t = getTemp();
	String forecastStr = getForecastString();
  strcpy(curMessage, forecastStr.c_str());
}

void refreshQuote()
{
	if (now() > (lastQuoteApiUpdate + QUOTE_POLL_INTERVAL))
	{
	  Serial.println("call Quote API");
		if(getQuoteData(quoteCategory[quoteCategoryIndex].c_str())) lastQuoteApiUpdate = now();
    if(++quoteCategoryIndex>quoteCategorySize)quoteCategoryIndex=0;
    Serial.println("got Quote Data");
	}
}
void displayQuote()
{
	refreshQuote();
//	int t = getTemp();
	String quoteStr = getQuoteString();
//strip out invalid chars
  std::string output;
  output.reserve(1024); // optional, avoids buffer reallocations in the loop
  for(size_t i = 0; i < MESG_SIZE; ++i){
    if(quoteStr[i] <= '~') output += quoteStr[i];
    if (quoteStr[i]==0x0) break;
  }

  strcpy(curMessage, output.c_str());
}

void displayDHT(){
	if (millis() - lastDHTRead > DHT_POLL_INTERVAL) // n ms since last poll so do it again....
	{
		Serial.println("Reading Temp");
		readDHT();			//read the temprature & RH
		lastDHTRead = millis(); //store the last poll so we can
    strcpy(curMessage, getDHTmessage().c_str());
	}
}
void displayClock()
{
	draw_clock(hour(), minute(), second());
}

void displayMessage(){
  strcpy(curMessage,webMessage);
  currentMsgCount=0;
}

void changeDisplayState(){
  int i = (int)displayState;
//  ++(int)displayState;
  if(++i>=DISPLAY_END) i=(int)DISPLAY_START;
  Serial.print("init inc to ");
  Serial.println(i);
  while(!getSectionEnabled(i)){
     Serial.print(i);
     Serial.println(" is not enabled");
     if(++i>=DISPLAY_END) i=(int)DISPLAY_START;
     Serial.print(i);
     Serial.println(i+" is incremented");
     if(i==(int)displayState){//we are back at the start!!
       Serial.println("Everything Disabled!");
        break;//don't get stuck if nothing enabled
     }
  }
  displayState=(DisplayStates)i;

    currentDisplayState=displayState;
    switch(currentDisplayState){
      case DISPLAY_DHT :
        Serial.println("DISPLAY_DHT");
        displayDHT();
        break;
      case DISPLAY_CLOCK :
        Serial.println("DISPLAY_CLOCK");
        clockStarted=millis();
        clockDisplayed=true;
        break;
      case DISPLAY_WEATHER : 
        Serial.println("DISPLAY_WEATHER");
        displayWeather();
        break;
      case DISPLAY_FORECAST : 
        Serial.println("DISPLAY_FORECAST");
        displayForecast();
        break;
      case DISPLAY_QUOTE : 
        Serial.println("DISPLAY_QUOTE");
        displayQuote();
        break;
      case DISPLAY_MESSAGE : 
        Serial.println("DISPLAY_MESSAGE");
        displayMessage();
        break;
      case DISPLAY_END :
        Serial.println("DISPLAY_END");
        break;
      case DISPLAY_RESET : break;
    }
    if(clockDisplayed && currentDisplayState!=DISPLAY_CLOCK){
        clockDisplayed=false;
        mx.clear();
    }
}

/*--------------------------
    New MD MAX72xxstuff!!!
  --------------------------*/
void scrollDataSink(uint8_t dev, MD_MAX72XX::transformType_t t, uint8_t col)
// Callback function for data that is being scrolled off the display
{
#if PRINT_CALLBACK
  Serial.print("\n cb ");
  Serial.print(dev);
  Serial.print(' ');
  Serial.print(t);
  Serial.print(' ');
  Serial.println(col);
#endif
}

uint8_t scrollDataSource(uint8_t dev, MD_MAX72XX::transformType_t t)
// Callback function for data that is required for scrolling into the display
{
  static enum { S_IDLE, S_NEXT_CHAR, S_SHOW_CHAR, S_SHOW_SPACE } state = S_IDLE;
  static uint16_t curLen, showLen;
  static uint8_t  cBuf[8];
  uint8_t colData = 0;

  // finite state machine to control what we do on the callback
  switch (state)
  {
  case S_IDLE: // reset the message pointer and check for new message to load
    if(displayState==DISPLAY_RESET) resetWiFi();//wifi reset messaged finished displaying so reset wifi and ESP
    PRINTS("\nS_IDLE");
    pCurMessage = curMessage;      // reset the pointer to start of message
    //always change display state unless we are displaying multiple x messages 
    if(displayState!=DISPLAY_MESSAGE || ++currentMsgCount==maxMsgCount){
      changeDisplayState();
    }
    Serial.println("idle");
    state = S_NEXT_CHAR;
    break;

  case S_NEXT_CHAR: // Load the next character from the font table
    PRINTS("\nS_NEXT_CHAR");
    if (*pCurMessage == '\0')
      state = S_IDLE;
    else
    {
      showLen = mx.getChar(*pCurMessage++, sizeof(cBuf) / sizeof(cBuf[0]), cBuf);
      curLen = 0;
      state = S_SHOW_CHAR;
    }
    break;

  case S_SHOW_CHAR: // display the next part of the character
    PRINTS("\nS_SHOW_CHAR");
    colData = cBuf[curLen++];
    if (curLen < showLen)
      break;

    // set up the inter character spacing
    showLen = (*pCurMessage != '\0' ? CHAR_SPACING : (MAX_DEVICES*COL_SIZE)/2);
    curLen = 0;
    state = S_SHOW_SPACE;
    // fall through

  case S_SHOW_SPACE:  // display inter-character spacing (blank column)
    PRINT("\nS_ICSPACE: ", curLen);
    PRINT("/", showLen);
    curLen++;
    if (curLen == showLen)
      state = S_NEXT_CHAR;
    break;

  default:
    state = S_IDLE;
  }

  return(colData);
}

void scrollText(void)
{
  static uint32_t	prevTime = 0;

  // Is it time to scroll the text?
  if (millis() - prevTime >= scrollDelay)
  {
    mx.transform(MD_MAX72XX::TSL);  // scroll along - the callback will load all the data
    prevTime = millis();      // starting point for next time
  }
}

void setup()
{
   // Display initialization
  mx.begin();
  mx.setShiftDataInCallback(scrollDataSource);
  mx.setShiftDataOutCallback(scrollDataSink);
  initMX(&mx);
  curMessage[0] = '\0';

	Serial.begin(9600);
	Serial.println();
	Serial.println("boot");
	Serial.println();

	// set flash button to config resetter
	pinMode(CONFIGRESETBUTTON, INPUT_PULLUP);

//initMAX7219();
//	drawStringClr(0, font, " Conn...");

	// Set WiFi parameters explicitly: autoconnect, station mode
  drawIP();
	setupWiFi();
  //sprintf(curMessage, "c", "Hellow World");

	ArduinoOTA.setHostname("matrixclock");
	ArduinoOTA.setPassword("matrixclockfirmware");
	ArduinoOTA.begin();

	NTPclient.begin("au.pool.ntp.org", AEST);
	setSyncInterval(SECS_PER_HOUR);
	setSyncProvider(getNTPtime);

//	drawStringClr(0, font, "link up");

	//init temp/humid sensor
	Serial.println("Init Temp Sensor");
	initDHT();
	Serial.println("Inited Temp Sensor");
  startWebServer();
}

void loop()
{
  if (currentDisplayState==DISPLAY_CLOCK) {
    displayClock();
    if ((millis()-clockStarted)>clockDuration) changeDisplayState();
  }
  else scrollText();

	if (digitalRead(CONFIGRESETBUTTON) == LOW)
	{
		if (reset_trigger == false)
		{
			reset_trigger = true;
			Serial.println("Config reset button pressed, taking action");
      displayState=DISPLAY_RESET;
      strcpy(curMessage, 
        "Reseting in 10"\
        "............9"\
        "............8"\
        "............7"\
        "............6"\
        "............5"\
        "............4"\
        "............3"\
        "............2"\
        "............1"\
        "............0");
        scrollDelay=9*SCROLL_DELAY_MULTIPLYER;//display it slowwwww.
        pCurMessage = curMessage;
		}
	}
  //set the web message to the local ip once only.
  if (!gotWebIP && WiFi.localIP().isSet()){
    gotWebIP=true;
    strcpy(webMessage, ("IP address:"+WiFi.localIP().toString()).c_str());
  }

  //handle web server requests.
  handleWiFi(now(), getTemp(), getHumidity(), getWeatherString(), getForecastString(), getQuoteString());

  //possible things freom web server. 1 - new message
  if(newWebMessage()){
    strcpy(webMessage,getWebMessage());
  }

  //possible things freom web server. 2 - change brightness
  if(getIntensity()!=currentIntensity)
  {
    currentIntensity=getIntensity();
    Serial.print("set bright=");
    Serial.println(currentIntensity);
    // Set the brightness of the led, 0= minimum, 15= maximum 
    mx.control(MD_MAX72XX::INTENSITY, currentIntensity);
  }

  if(getMsgCount()!=maxMsgCount)
  {
    maxMsgCount=getMsgCount();
    Serial.print("set MsgCount=");
    Serial.println(maxMsgCount);
  }

  if(getMsgSpeed()!=currentMsgSpeed)
  {
    currentMsgSpeed=getMsgSpeed();
    Serial.print("set MsgSpeed=");
    Serial.println(currentMsgSpeed);
    scrollDelay=currentMsgSpeed*SCROLL_DELAY_MULTIPLYER;
    clockDuration=currentMsgSpeed*CLOCK_DISP_DURATION;
  }

	ArduinoOTA.handle();
}