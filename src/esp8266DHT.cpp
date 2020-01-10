/*
 *
 * Tony Phibbs
 * DHT sensors on esp8266
 */

#include "Arduino.h"
#include "esp8266DHT.h"
#include "config.h"

#ifdef USE_DHT12
  #include <DHT12.h>
  DHT12 dhtXX(DHTPIN, true);
#else
   #include <DHT.h>
   DHT dhtXX(DHTPIN, DHT22);
#endif

String dhtMessage="";
float dhtTemp, dhtHumidity;
//The setup function is called once at startup of the sketch
void initDHT(){
	// Add your initialization code here
	dhtXX.begin();
}
String getDHTmessage(){
  return dhtMessage;
}
float getTemp(){
	return dhtTemp;
}
float getHumidity(){
	return dhtHumidity;
}
void readDHT()
{
	// Read temperature as Celsius (the default)
	dhtTemp = dhtXX.readTemperature();
	// Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
	dhtHumidity = dhtXX.readHumidity();

	// Check if any reads failed and exit early (to try again).
	if (isnan(dhtHumidity) || isnan(dhtTemp) )
	{
		dhtMessage = "Failed to read from DHT sensor!";
		Serial.println(dhtMessage);
	}
     else
	{
		// Compute heat index in Celsius (isFahreheit = false)
		float heatIndex = dhtXX.computeHeatIndex(dhtTemp, dhtHumidity, false);
		// Compute dew point in Celsius (isFahreheit = false)
//		float dewPoint = dht.dewPoint(dhtTemp, dhtHumidity, false);
		float dewPoint =  (dhtTemp - (14.55 + 0.114 * dhtTemp) * (1 - (0.01 * dhtHumidity)) - pow(((2.5 + 0.007 * dhtTemp) * (1 - (0.01 * dhtHumidity))),3) - (15.9 + 0.117 * dhtTemp) * pow((1 - (0.01 * dhtHumidity)), 14));
      dhtMessage = "Sensor Temperature:" + String(dhtTemp,1) + "c";
	  dhtMessage += " humidity:" + String(dhtHumidity) + "%"; 
	  dhtMessage += " heat index:" + String(heatIndex,1) + "c"; 
	  dhtMessage += " dew point:" + String(dewPoint,1) + "c";
      Serial.println(dhtMessage);
	}

}