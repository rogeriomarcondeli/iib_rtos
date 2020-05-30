
/////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __INPUT_H__
#define __INPUT_H__

/////////////////////////////////////////////////////////////////////////////////////////////

void InputInit(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned char Gpdi1Read(void);
extern unsigned char Gpdi2Read(void);
extern unsigned char Gpdi3Read(void);
extern unsigned char Gpdi4Read(void);
extern unsigned char Gpdi5Read(void);
extern unsigned char Gpdi6Read(void);
extern unsigned char Gpdi7Read(void);
extern unsigned char Gpdi8Read(void);
extern unsigned char Gpdi9Read(void);
extern unsigned char Gpdi10Read(void);
extern unsigned char Gpdi11Read(void);
extern unsigned char Gpdi12Read(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned char Driver1TopErrorRead(void);
extern unsigned char Driver1BotErrorRead(void);
extern unsigned char Driver1OverTempRead(void);
extern unsigned char Driver2TopErrorRead(void);
extern unsigned char Driver2BotErrorRead(void);
extern unsigned char Driver2OverTempRead(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern unsigned char BoardAddressRead(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void Gpdi1Enable(void);
extern void Gpdi2Enable(void);
extern void Gpdi3Enable(void);
extern void Gpdi4Enable(void);
extern void Gpdi5Enable(void);
extern void Gpdi6Enable(void);
extern void Gpdi7Enable(void);
extern void Gpdi8Enable(void);
extern void Gpdi9Enable(void);
extern void Gpdi10Enable(void);
extern void Gpdi11Enable(void);
extern void Gpdi12Enable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void Gpdi1Disable(void);
extern void Gpdi2Disable(void);
extern void Gpdi3Disable(void);
extern void Gpdi4Disable(void);
extern void Gpdi5Disable(void);
extern void Gpdi6Disable(void);
extern void Gpdi7Disable(void);
extern void Gpdi8Disable(void);
extern void Gpdi9Disable(void);
extern void Gpdi10Disable(void);
extern void Gpdi11Disable(void);
extern void Gpdi12Disable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void Driver1TopErrorEnable(void);
extern void Driver1TopErrorDisable(void);

extern void Driver1BotErrorEnable(void);
extern void Driver1BotErrorDisable(void);

extern void Driver1OverTempEnable(void);
extern void Driver1OverTempDisable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

extern void Driver2TopErrorEnable(void);
extern void Driver2TopErrorDisable(void);

extern void Driver2BotErrorEnable(void);
extern void Driver2BotErrorDisable(void);

extern void Driver2OverTempEnable(void);
extern void Driver2OverTempDisable(void);

/////////////////////////////////////////////////////////////////////////////////////////////

#endif

/////////////////////////////////////////////////////////////////////////////////////////////



