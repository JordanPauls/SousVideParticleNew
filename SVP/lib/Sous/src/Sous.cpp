#include "Sous.h"
#include <iostream>
#include <chrono>
#include <LiquidCrystal.h>

Sous::Sous(){
	void InitializeTempDisplay();																											//Run a short initialization message on the LCD and wait 3 seconds
	RunningIntegral = 0;
}

unsigned long Sous::GetCurrentTime() //Gets returns current time in milliseconds
{
	auto now = std::chrono::system_clock::now();
	auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);

	auto value = now_ms.time_since_epoch();
	unsigned long duration = value.count();

	return duration;
}
/*
double Sous::GetCurrentTemp()
{
	double CurrentTemp;
	CurrentTemp = thermocouple.readFahrenheit(ExternalTempOffsetFahrenheit);
	return CurrentTemp;
}
*/
double Sous::UpdateHeat(unsigned long CurrentTime,unsigned long LastUpdateTime,double CurrentTemp,double TempTarget,double Kp,double Ki,double Kd,double FullPowerThreshold)
{
	double PID_Out,DutyCycle;
	PID_Out = UpdateError(CurrentTime,LastUpdateTime,CurrentTemp,TempTarget,Kp,Ki,Kd);
	DutyCycle = CalculateDutyCycle(PID_Out,FullPowerThreshold);
	return DutyCycle;
}

void Sous::UpdateDisplay(double CurrentTemp,double TempTarget,double DutyCycle, LiquidCrystal lcd)
{
	lcd.setCursor(0, 0);
	if (isnan(CurrentTemp) || isnan(DutyCycle)) { //error case
    lcd.print("Thermocouple"); lcd.setCursor(0,1); lcd.print("Error!");
  }
  else{ // No error
    lcd.print("Temp:"); lcd.print((int)CurrentTemp);  if((int)CurrentTemp<100)lcd.print("  "); else lcd.print(" ");
    lcd.print("Set:"); lcd.print((int)TempTarget); if((int)TempTarget<100)lcd.print(" ");
    lcd.setCursor(0, 1);
    lcd.print("Duty Cycle: ");
		lcd.print(DutyCycle);
  	lcd.print("%  ");
	}
}

void Sous::DutyCycleControl(unsigned long CurrentTime,unsigned long LastUpdateTime,unsigned long UpdatePeriod,double DutyCycle,int RelayPin, LiquidCrystal lcd)
{
	if(isnan(CurrentTime) || isnan(LastUpdateTime) || isnan(UpdatePeriod) ||  isnan(DutyCycle)){
		lcd.clear()
		lcd.setCursor(0, 0);
    lcd.print("DC Ctrl Err");																											//error catch
	}
	else if (CurrentTime-LastUpdateTime < UpdatePeriod*DutyCycle){
		digitalWrite(RelayPin,LOW);																										//Turn heat on
	}
	else if (CurrentTime-LastUpdateTime > UpdatePeriod*DutyCycle){
		digitalWrite(RelayPin,HIGH)																										//Turn heat off
	}
}

void delay(int DurationMs){
	unsigned long start,now;
	start = GetCurrentTime();
	now = start;
	while(now > start + DurationMs) now = GetCurrentTime();
}






//private
void InitializeTempDisplay(){
	lcd.begin(16, 2);                                                                   						// set up the LCD's number of columns and rows:
	lcd.print("Sous Vide v2");                                                          						// Print a startup message to the LCD.
	delay(3000);
};

double Sous::UpdateError(unsigned long CurrentTime,unsigned long LastUpdateTime,double CurrentTemp,int TempTarget, double kp, double ki, double kd)
{
	double PID_Err,P,I,D,dt;
	dt = CurrentTime-LastUpdateTime;
	P = TempTarget-CurrentTemp; // Raw Error
	I = RunningIntegral + dt*P;
	D = P/dt;
  PID_Err = kp*P + ki*I + kd*D;
	return PID_Err;
}

double Sous::CalculateDutyCycle(double error,double FullPowerThreshold)
{
	double DutyCycle;
	if (isnan(error))
	{
		lcd.clear()
		lcd.setCursor(0, 0);
		lcd.print("PID NaN Error");
	}
	else if (error <= 0)
	{
		DutyCycle = 0; //Zero Duty Cycle
	}
	else if (error / FullPowerThreshold < 1)
	{
		DutyCycle = error / FullPowerThreshold; //Partial Duty Cycle
	}
	else
	{
		DutyCycle = 1; //Full Duty Cycle
	}
	return DutyCycle;
}
