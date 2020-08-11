#pragma once
#include <LiquidCrystal.h>

class Sous
{
public:
	Sous();//Constructor for initialization
	unsigned long GetCurrentTime(); //happens every loop
	//double GetCurrentTemp();	//happens every loop
	double UpdateHeat(unsigned long CurrentTime,unsigned long LastUpdateTime,double CurrentTemp,double TempTarget,double Kp,double Ki,double Kd,double FullPowerThreshold); //Happens only once every period
  void UpdateDisplay(double CurrentTemp,double TempTarget,double DutyCycle, LiquidCrystal lcd); //happens every loop
	void DutyCycleControl(unsigned long CurrentTime,unsigned long LastUpdateTime,unsigned long UpdatePeriod,double DutyCycle, int RelayPin, LiquidCrystal lcd); //happens every loop
	void delay(int DurationMs);
private:
	void InitializeTempDisplay();																											//Run during constructor
	double UpdateError(unsigned long CurrentTime,unsigned long LastUpdateTime,double CurrentTemp,double TempTarget,double kp, double ki, double kd);
	double CalculateDutyCycle(double error,double FullPowerThreshold);
	double RunningIntegral,LastError; //Variables for PID

};
