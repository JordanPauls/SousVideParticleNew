SYSTEM_THREAD(ENABLED);
STARTUP(WiFi.selectAntenna(ANT_INTERNAL)); // selects the CHIP antenna

#include <SPI.h>
#include "Adafruit_MAX31855.h"
#include <LiquidCrystal.h>
#include "Sous.h"

#define MAXDO   A0                                                                     						//Thermocouple Board Pin
#define MAXCS   A1                                                                    						//Thermocouple Board Pin
#define MAXCLK  A2                                                                    						//Thermocouple Board Pin
#define RELAY_PIN A3                                                                  						//Relay Logical Output Pin
#define TEMP_UP A4                                                                  						  //Increase Temp Button Pin
#define TEMP_DOWN A5                                                                   						//Decrease Temp Button Pin

Adafruit_MAX31855 thermocouple(MAXCLK, MAXCS, MAXDO);                                 						// initialize the Thermocouple
Sous Sous1;                                                                                      // Run Sous initialization

const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;                          						//LCD Pins
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                                            						//Initialize LCD given above pins

//const int rs = 0, en = 1, d4 = 2, d5 = 3, d6 = 4, d7 = 5;                          						    //LCD Pins
//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                                            						//Initialize LCD given above pins

const double Kp = 1,Ki = 0.1,Kd = 0;                                                              //PID coefficients

unsigned long UpdatePeriod = 20000;                                                               //20 seconds between each Duty Cycle Calulation
unsigned long LastUpdateTime = 0;                                                                 //Initialize at zero so update will happen immediately the first time
double DutyCycle;
double TempTarget =  140;
double FullPowerThreshold = 20;

double ExternalTempOffsetFahrenheit = 0;
void setup()
{

}

void loop()
{
  unsigned long CurrentTime;
  double CurrentTemp;
  CurrentTime = Sous1.GetCurrentTime();
  CurrentTemp = thermocouple.readFahrenheit(ExternalTempOffsetFahrenheit);
  if (CurrentTime > LastUpdateTime+UpdatePeriod)                                                  // Time to update the Duty Cycle
  {
    DutyCycle = Sous1.UpdateHeat(CurrentTime,LastUpdateTime,CurrentTemp,TempTarget,Kp,Ki,Kd,FullPowerThreshold);
    LastUpdateTime = CurrentTime;
  }
  Sous1.DutyCycleControl(CurrentTime,LastUpdateTime,UpdatePeriod,DutyCycle,RELAY_PIN,lcd);
  Sous1.UpdateDisplay(CurrentTemp,TempTarget,DutyCycle,lcd);
}


















//
