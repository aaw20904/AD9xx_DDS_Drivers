/******************************************************/
/****Bare-Metal non OS Device AD9834 Driver ****/
/********Author: Andrew Androsovych***************/
#include <stdbool.h>
 
/***CLOCK FREQUENCY FOR the DDS IC*/
#define AD9834_CLOCK 75000000.0



#define  AD9834_B28 (1 << 13)
#define AD9834_HLB (1 << 12)
#define AD9834_FSEL (1 << 11)
#define AD9834_PSEL (1 << 10)
#define AD9834_PIN (1 << 9)   /*  PIN/SW  */
#define AD9834_RESET (1 << 8)
#define AD9834_SLEEP1 (1 << 7)
#define AD9834_SLEEP12 (1 << 6)
#define AD9834_OPBITEN (1 << 5)
#define AD9834_SIGN (1 << 4)   /* SIGN/PIB */
#define AD9834_DIV2 (1 << 3)
#define AD9834_MODE (1 << 1)

 
 void   AD9834_Delay98(void);
/*turn HI   FSYNC pin*/
  void AD9834_TurnSyncHi(void);
  void AD9834_TurnSyncLo(void);
/*functions for hardware control*/	
	void AD9834_TurnPhaseHi(void);
	void AD9834_TurnPhaseLow(void);
	void AD9834_TurnFreqHigh(void);
	void AD9834_TurnFreqLow(void);
	void AD9834_TurnResetHi(void);
	void AD9834_TurnResetLow(void);

 
/*SLEEP1 = 1, the internal MCLK is disabled. The DAC output remains at its present value as the NCO is no longer
accumulating*/
void AD9834_EnterIntoSleep1 (void);
void AD9834_WakeUpFromSleep1(void);
/*type of gignal on   SIGN BIT OUT pin 
@mode 0-sine wave, 1-SQUARE wave
@divider 1-by1, 2-by2  */
void AD9834_SetSignBitOutMode(char mode,char divider);
/*ON/OFF Output buffer  
@TRUE-enable; FALSE-disable*/
void AD9834_EnableSignBitOut(bool state);
/*It is to control what is output at the IOUT pin/IOUTB pin: 
@0-SINE 1-TRIANGLE*/
void AD9834_SetIoutMode(char mode);
/*choose pase reg  0 or 1*/
void AD9834_ChoosePhaseReg(char x);
/*choose freq reg  0 or 1*/
void AD9834_ChooseFreqReg(char x);
/*write frequency to IC*/
void AD9834_WriteFrequency(char reg, float freq);
/*write phase to IC*/
void AD9834_WritePhase(char reg, float phase);
/**write data */
void AD9834_WriteRawData(unsigned short data);
/**/
/*fast direct write data into  the	FREQ register**/
void AD9834_FastFrequencySet(char reg, unsigned int content);
/**fast direct write data into the Phase register***/
void AD9834_FastPhaseSet(char reg, unsigned short content); 

/************B E G I N  initialization from these two functions:***/
/***I N I T I A L I Z A T I O N: software IC control
@ Freq - Hz, Phase - Rad
**/

void InitWithSoftwareControl (float Freq, float Phase); 
/***I N I T I A L I Z A T I O N: hardware IC control
@ Freq - Hz, Phase - Rad 
**/

void InitWithHardwareControl (float Freq, float Phase);
