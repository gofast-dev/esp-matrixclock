#define WEB_TITLE "Liam\'s Weather Station"

//#define DHTPIN D3         // (in DHT11 this was 0 - D3?) what pin we're connected to D0 (A16 or GPIO16)
//#define DHTTYPE DHT12     // DHT 11

// For Jack USE_DHT12
//#define USE_DHT12

#ifndef DHTPIN
// for jack D0
//#define DHTPIN D0         // what pin we're connected to D0 (A16 or GPIO16)
#define DHTPIN D1         // what pin we're connected to D1 
//for Jack DHT12
//#define DHTTYPE DHT12     // DHT 11
#define DHTTYPE DHT22     // DHT 11
#endif


#define DHT_POLL_INTERVAL 15000   // in ms

#define CONFIGRESETBUTTON D3	    // flash button GPIO 0 (was D5 - GPIO 14 on nodemcu hw)

// for Jack  HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
//#define HARDWARE_TYPE MD_MAX72XX::PAROLA_HW
//#define HARDWARE_TYPE MD_MAX72XX::GENERIC_HW
//#define HARDWARE_TYPE MD_MAX72XX::ICSTATION_HW
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
/*--------------------------
    New MD MAX72xx stuff!!!
  --------------------------*/
#define MAX_DEVICES 4 // MAX display modules
#define CLK_PIN   D6 //=0x or SCK
#define DATA_PIN  D8// or MOSI
#define CS_PIN    D7 // or SS

#define WEATHER_POLL_INTERVAL 300 // 5 minutes in seconds
#define FORECAST_POLL_INTERVAL 5400 // 1.5 hours in seconds
#define QUOTE_POLL_INTERVAL 3600 // 1 hour in seconds

// Speed parameters

//display the clock in ms default 15 seconds - 5*3000ms
#define CLOCK_DISP_DURATION 3000  //ms - how long to display the clock for - this times currentMsgSpeed
//scroll speed default is 35ms seconds - 5*7ms
#define SCROLL_DELAY_MULTIPLYER 7//number of ms delay scolling times message speed

#define AEST 10 //Aust Eastern Standard Time

#define NO_DST false //if ture Don't use daylight savings (e.g. QLD)

