/*
This is using the DHT11 temp/himidity sensor 
*/
//#include <Adafruit_Sensor.h>
#include <DHT.h>
#include "dht11.h"
#include "config.h"
//Constants
DHT dht(DHTPIN, DHTTYPE); //// Initialize DHT sensor for normal 16mhz Arduino

float tempCelcius;  
float humidity;     
long lastPoll;//current time of last poll

/*
 *  Start of setup
 */
void setup2()  
{                
  initDHT11();
}
/*
 *  Main loop
 */
void loop2()  // Start of loop
{
//every 1000 ms update the temp & humidity
  
  if ( millis()-lastPoll>2000)// 1000ms since last poll so do it again....
  {
    readStats();//read the temprature & RH
    lastPoll=millis();//store the last poll so we can 
  }
}

void initDHT11(){
/*----------------------
 * DHT11
 *----------------------
 */
DHT dht(DHTPIN, DHTTYPE);
    Serial.print("Init pin");
    Serial.println(DHTPIN);
    Serial.print("Init typ ");
    Serial.println(DHTTYPE);
  dht.begin();
}

void readStats(){
  //Read data and store it to variables hum and temp
  humidity = dht.readHumidity();
  tempCelcius= dht.readTemperature();
}

void displayStats(){
//Print temp and humidity values to serial monitor
  Serial.print("Humidity: ");
  Serial.print(humidity,1);
  Serial.print(" %, Temp: ");
  Serial.print(tempCelcius,1);
  Serial.println(" Celsius");
}  // End of loop