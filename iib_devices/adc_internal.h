
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __ADC_INTERNAL_H__
#define __ADC_INTERNAL_H__

/////////////////////////////////////////////////////////////////////////////////////////////

extern void AdcsInit(void);
extern void sample_adc(void);
extern float CurrentRange(float nFstCurr, float nSecCurr, float nBurden, float MaxVoltInput);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void ConfigPolVoltCh1(unsigned char sts);
extern void ConfigPolVoltCh2(unsigned char sts);
extern void ConfigPolVoltCh3(unsigned char sts);
extern void ConfigPolVoltCh4(unsigned char sts);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void VoltageCh1Init(float nValue, unsigned char Delay);
extern void VoltageCh2Init(float nValue, unsigned char Delay);
extern void VoltageCh3Init(float nValue, unsigned char Delay);
extern void VoltageCh4Init(float nValue, unsigned char Delay);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void ConfigPolCurrCh1(unsigned char sts);
extern void ConfigPolCurrCh2(unsigned char sts);
extern void ConfigPolCurrCh3(unsigned char sts);
extern void ConfigPolCurrCh4(unsigned char sts);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void CurrentCh1Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms);
extern void CurrentCh2Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms);
extern void CurrentCh3Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms);
extern void CurrentCh4Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void ConfigPolLvCurrCh1(unsigned char sts);
extern void ConfigPolLvCurrCh2(unsigned char sts);
extern void ConfigPolLvCurrCh3(unsigned char sts);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void LvCurrentCh1Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms);
extern void LvCurrentCh2Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms);
extern void LvCurrentCh3Init(float nFstCurr, float nSecCurr, float nBurden, unsigned char delay_ms);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void DriverVoltageInit(void);
extern void DriverCurrentInit(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void VoltageCh1Sample(void);
extern void VoltageCh2Sample(void);
extern void VoltageCh3Sample(void);
extern void VoltageCh4Sample(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void CurrentCh1Sample(void);
extern void CurrentCh2Sample(void);
extern void CurrentCh3Sample(void);
extern void CurrentCh4Sample(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void LvCurrentCh1Sample(void);
extern void LvCurrentCh2Sample(void);
extern void LvCurrentCh3Sample(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void DriverVoltageSample(void);
extern void Driver1CurrentSample(void);
extern void Driver2CurrentSample(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern float VoltageCh1Read(void);
extern float VoltageCh2Read(void);
extern float VoltageCh3Read(void);
extern float VoltageCh4Read(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern float CurrentCh1Read(void);
extern float CurrentCh2Read(void);
extern float CurrentCh3Read(void);
extern float CurrentCh4Read(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern float LvCurrentCh1Read(void);
extern float LvCurrentCh2Read(void);
extern float LvCurrentCh3Read(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern float DriverVoltageRead(void);
extern float Driver1CurrentRead(void);
extern float Driver2CurrentRead(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned char VoltageCh1AlarmStatusRead(void);
extern unsigned char VoltageCh1TripStatusRead(void);
extern unsigned char VoltageCh2AlarmStatusRead(void);
extern unsigned char VoltageCh2TripStatusRead(void);
extern unsigned char VoltageCh3AlarmStatusRead(void);
extern unsigned char VoltageCh3TripStatusRead(void);
extern unsigned char VoltageCh4AlarmStatusRead(void);
extern unsigned char VoltageCh4TripStatusRead(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void VoltageCh1AlarmLevelSet(float nValue);
extern void VoltageCh1TripLevelSet(float nValue);
extern void VoltageCh2AlarmLevelSet(float nValue);
extern void VoltageCh2TripLevelSet(float nValue);
extern void VoltageCh3AlarmLevelSet(float nValue);
extern void VoltageCh3TripLevelSet(float nValue);
extern void VoltageCh4AlarmLevelSet(float nValue);
extern void VoltageCh4TripLevelSet(float nValue);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void VoltageCh1Enable(void);
extern void VoltageCh2Enable(void);
extern void VoltageCh3Enable(void);
extern void VoltageCh4Enable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void VoltageCh1Disable(void);
extern void VoltageCh2Disable(void);
extern void VoltageCh3Disable(void);
extern void VoltageCh4Disable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned char CurrentCh1AlarmStatusRead(void);
extern unsigned char CurrentCh1TripStatusRead(void);
extern unsigned char CurrentCh2AlarmStatusRead(void);
extern unsigned char CurrentCh2TripStatusRead(void);
extern unsigned char CurrentCh3AlarmStatusRead(void);
extern unsigned char CurrentCh3TripStatusRead(void);
extern unsigned char CurrentCh4AlarmStatusRead(void);
extern unsigned char CurrentCh4TripStatusRead(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void CurrentCh1AlarmLevelSet(float nValue);
extern void CurrentCh1TripLevelSet(float nValue);
extern void CurrentCh2AlarmLevelSet(float nValue);
extern void CurrentCh2TripLevelSet(float nValue);
extern void CurrentCh3AlarmLevelSet(float nValue);
extern void CurrentCh3TripLevelSet(float nValue);
extern void CurrentCh4AlarmLevelSet(float nValue);
extern void CurrentCh4TripLevelSet(float nValue);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void CurrentCh1Enable(void);
extern void CurrentCh2Enable(void);
extern void CurrentCh3Enable(void);
extern void CurrentCh4Enable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void CurrentCh1Disable(void);
extern void CurrentCh2Disable(void);
extern void CurrentCh3Disable(void);
extern void CurrentCh4Disable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned char LvCurrentCh1AlarmStatusRead(void);
extern unsigned char LvCurrentCh1TripStatusRead(void);
extern unsigned char LvCurrentCh2AlarmStatusRead(void);
extern unsigned char LvCurrentCh2TripStatusRead(void);
extern unsigned char LvCurrentCh3AlarmStatusRead(void);
extern unsigned char LvCurrentCh3TripStatusRead(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void LvCurrentCh1AlarmLevelSet(float nValue);
extern void LvCurrentCh1TripLevelSet(float nValue);
extern void LvCurrentCh2AlarmLevelSet(float nValue);
extern void LvCurrentCh2TripLevelSet(float nValue);
extern void LvCurrentCh3AlarmLevelSet(float nValue);
extern void LvCurrentCh3TripLevelSet(float nValue);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void LvCurrentCh1Enable(void);
extern void LvCurrentCh2Enable(void);
extern void LvCurrentCh3Enable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void LvCurrentCh1Disable(void);
extern void LvCurrentCh2Disable(void);
extern void LvCurrentCh3Disable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned char DriverVoltageAlarmStatusRead(void);
extern unsigned char DriverVolatgeTripStatusRead(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void DriverVoltageAlarmLevelSet(float nValue);
extern void DriverVoltageTripLevelSet(float nValue);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void DriverVoltageEnable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void DriverVoltageDisable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void DriverVoltageDelay(unsigned char Delay_Set);

/////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned char Driver1CurrentAlarmStatusRead(void);
extern unsigned char Driver1CurrentTripStatusRead(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void Driver1CurrentAlarmLevelSet(float nValue);
extern void Driver1CurrentTripLevelSet(float nValue);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void Driver1CurrentEnable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void Driver1CurrentDisable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned char Driver2CurrentAlarmStatusRead(void);
extern unsigned char Driver2CurrentTripStatusRead(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void Driver2CurrentAlarmLevelSet(float nValue);
extern void Driver2CurrentTripLevelSet(float nValue);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void Driver2CurrentEnable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void Driver2CurrentDisable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void DriverCurrentDelay(unsigned char Delay_Set);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void AdcClearAlarmTrip(void);

/////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////
