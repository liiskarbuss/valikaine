// A simple CO2 meter using the Adafruit SCD30 breakout and the Adafruit 128x32 OLEDs
#include <Adafruit_SCD30.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Adafruit_MPL115A2.h>
#include "Adafruit_HT1632.h"

#define HT_DATA 2
#define HT_WR 3
#define HT_CS 4
#define HT_CS2 5

// use this line for single matrix
Adafruit_HT1632LEDMatrix matrix = Adafruit_HT1632LEDMatrix(HT_DATA, HT_WR, HT_CS);
// use this line for two matrices!
//Adafruit_HT1632LEDMatrix matrix = Adafruit_HT1632LEDMatrix(HT_DATA, HT_WR, HT_CS, HT_CS2);

Adafruit_MPL115A2 mpl115a2;
Adafruit_SCD30  scd30;
//Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
Adafruit_SSD1306 display(128, 32, &Wire, 8);

int sensor = 10;              // the pin that the sensor is atteched to
int state = LOW;             // by default, no motion detected
int val = 0;                 // variable to store the sensor status (value)
int cnt; //counter
int pir_status = 0; //determines whether motion is detected or not

void setup(void) {
    Serial.begin(9600);
    pinMode(sensor, INPUT);    // initialize sensor as an input
    while (!Serial) delay(10);     // will pause Zero, Leonardo, etc until serial console opens

    Serial.println("Getting barometric pressure ...");
    if (! mpl115a2.begin()) 
	{
        Serial.println("Sensor not found! Check wiring");
    }
    Serial.println("SCD30 OLED CO2 meter!");

    // Try to initialize!
    if (!scd30.begin()) 
	{
        Serial.println("Failed to find SCD30 sensor");
        {
            delay(10);
        }
    }
    Serial.println("SCD30 Found!");


    // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) 
	{ // Address 0x3C for 128x32
        Serial.println(F("SSD1306 allocation failed"));
    }

    if (!scd30.setMeasurementInterval(3)) 
	{
        Serial.println("Failed to set measurement interval");
    }
    Serial.print("Measurement Interval: ");
    Serial.print(scd30.getMeasurementInterval());
    Serial.println(" seconds");

    display.display();
    delay(500); // Pause for half second

    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setRotation(0);


    matrix.begin(ADA_HT1632_COMMON_16NMOS);
    matrix.fillScreen();
    delay(500);
    matrix.clearScreen();
    matrix.setTextWrap(false);
}


void loop() {
    Serial.println("Loop begin");
    int pirinput = analogRead(A0);
    if(pirinput > 500 && !pir_status)
    {
	    pir_status = 1;
    }
    if(pirinput < 300 && pir_status)
    {
	    pir_status = 0;
    }
  
    if (scd30.dataReady()) 
    {
    
        if (!scd30.read()) 
		{
            Serial.println("Error reading sensor data");
            display.println("READ ERR");
            display.display();
		}
		float pressureKPA = 0, temperatureC = 0;
		float CO2 = 0;
		CO2 = scd30.CO2;
		
		mpl115a2.getPT(&pressureKPA, &temperatureC);
		pressureKPA = mpl115a2.getPressure();
		temperatureC = mpl115a2.getTemperature();
		
		display.clearDisplay();
		display.setCursor(0, 0);

		if(CO2 > 2000)
		{
			display.setTextSize(2);
			display.print("CO2:      ");
			display.print(CO2, 2);	
		}
		else
		{
			display.setTextSize(1);
			display.print("CO2:      ");
			display.print(CO2, 2);
			// display.setCursor(100, 20);
			display.println(" ppm");
				
			display.print("Humidity: ");
			display.print(scd30.relative_humidity);
			// display.setCursor(100, 20);
			display.println(" %");

			display.print("Pressure: ");
			display.print(pressureKPA);
			display.println(" kPa");
				
			display.print("Temp:     ");
			display.print(temperatureC);
			display.println(" C");	
		}
			

		display.display();
		Serial.print("CO2 ");
		Serial.println(CO2);
		if(pir_status)
		{
			//turn on LED matrix if motion is detected
		/*
	  if (CO2 <= 440 && CO2 >= 250) {
		matrix.setCursor(0, 0);
		matrix.print(":)");
		matrix.writeScreen();
	  }
	  else if (CO2 < 250) {
		matrix.setCursor(0, 0);
		matrix.print("???");
		matrix.writeScreen();
	  }
	  else if(CO2 > 440 && CO2 < 1000) {

		matrix.setCursor(0, 0);
		matrix.print(":/");
		matrix.writeScreen();
	  }
	  else if(CO2 >= 1000 && CO2 <= 2000){
		matrix.setCursor(0, 0);
		matrix.print(":(");
		matrix.writeScreen();
		}
	  else if(CO2 > 2000){
		matrix.setCursor(0, 0);
		matrix.print("x(");
		matrix.writeScreen();
		}
	  }
	  val = digitalRead(sensor);   // read sensor value
	  if (val == HIGH) {           // check if the sensor is HIGh
		delay(500);                // delay 100 milliseconds 
		cnt = 0;
		/*if (state == LOW) {
		  Serial.println("Motion detected!"); 
		  state = HIGH;       // update variable state to HIGH'
		  
		}*/
	  }
  } 
  else {
  /*    
     // delay(500);             // delay 200 milliseconds 
      cnt++;
      if(cnt == 25){
        while(val == LOW){
          matrix.clearScreen(); // lülita maatrix välja
          val = digitalRead(sensor);
          // ei saanud debugida kas töötab aga teoorias kui ca 2 min pole liikumist siis lülitab maatrixi völja
           
          }
        }
      /*if (state == HIGH){
        Serial.println("Motion stopped!");
        state = LOW;       // update variable state to LOW
    }*/
  }
  /*
  delay(1000);
  matrix.clearScreen();
  */
  delay(1000);
}
