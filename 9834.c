/******************************************************/
/****Bare-Metal non OS Device AD9834 Driver ****/
/********Author: Andrew Androsovych***************/

#include "9834.h"
#include "stm32f1xx_hal.h"

/*the minimal frequency step = Fclock / 2^24 */
const float FREQUENCY_STEP = AD9834_CLOCK / 0x10000000;

//#define DRIVER_HAL
/*pointer to a HAL driver handle*/
extern SPI_HandleTypeDef hspi1;
 
/*buffer to save initialization word - you can configurate modes of the DDS chip.
It uses as a storage for configuration data - all the changes in the control register
 write here (we can`r read any data from the dds chip)*/
 unsigned short initWord9834;

/*set FSYNC pin to HIGH: implement this function for your own configuration*/
  void inline AD9834_TurnSyncHi (void) {
   GPIOA->BSRR = GPIO_BSRR_BS6;
}
/*set FSYNC pin to LOW: implement this function for your own configuration*/
  void inline AD9834_TurnSyncLo (void) {
  GPIOA->BSRR = GPIO_BSRR_BR6;	
}


/*dummy delay*/
 void inline  AD9834_Delay98(void){
  unsigned short y = 0;
  while(y <  200) {
    y++;
  }
}
 
/*write frequency to register in the DDS IC   freq-in Hz*/
void AD9834_WriteFrequency (char reg, float freq) {
  unsigned short mask  = 0;       
	 unsigned int rawData;
	/*define - in which register will be writing*/
	  mask = (reg == 0) ? (1 << 14) : (1 << 15);
	/*calculate frequency*/
	rawData =  (unsigned int)(freq / FREQUENCY_STEP );
	  unsigned short lsbBits = (rawData & 0x3fff);
    unsigned short msbBits = ((rawData >> 14) & 0x3fff);
    /*wite MSB bits -control word*/
    AD9834_WriteRawData(AD9834_HLB | initWord9834);
	  
	//data
    AD9834_WriteRawData(( msbBits | mask ));
	 
	/*write LSB bits - control word*/
    AD9834_WriteRawData(0x0000| initWord9834);
	 
	//data
    AD9834_WriteRawData(( lsbBits | mask ));
	 
}
/*write phase (in Radians) to register in the DDS IC*/
void AD9834_WritePhase (char reg, float phase) {
		unsigned short rawData;
	unsigned short mask = (reg == 0) ? (0xc000) : (0xE000);
	rawData =(unsigned short) (phase * 4096) / 6.28;
	AD9834_WriteRawData((rawData | mask ));
	
}

void AD9834_WriteRawData(unsigned short data) {
		unsigned char tmp[2];
 
	tmp[1] = (unsigned char) (data & 0x00ff);
	tmp[0] = (data >> 8);
	/*set FSYNC LOW before transmitting*/
	AD9834_TurnSyncLo();
	/*start transmitting using HAL library*/
     // #ifdef DRIVER_HAL
	 HAL_SPI_Transmit (&hspi1, tmp, 2, 1000);
      //#endif
	/*set FSYNC HI after transmitting*/
	AD9834_TurnSyncHi();
}

/**SLEEP1 entering**/
void AD9834_EnterIntoSleep1 (void){
	initWord9834 |= AD9834_SLEEP1;
	AD9834_WriteRawData(initWord9834);
}

void AD9834_WakeUpFromSleep1 (void) {
	initWord9834 &= (~AD9834_SLEEP1);
	AD9834_WriteRawData (initWord9834);
}

/*type of gignal on   SIGN BIT OUT pin 
@mode 0-sine wave, 1-SQUARE wave
@divider 0-by1, 1-by2  */
void AD9834_SetSignBitOutMode (char mode,char divider) {
	if (mode == 0) {
		initWord9834 &= (~AD9834_SIGN);
	} else {
		initWord9834 |= AD9834_SIGN;
	}
	if (divider == 0) {
		initWord9834 |= AD9834_DIV2;
	} else {
		initWord9834 &= (~AD9834_DIV2);
	}
	
	AD9834_WriteRawData (initWord9834);
}

/*ON/OFF Output buffer  
@TRUE-enable; FALSE-disable*/
void AD9834_EnableSignBitOut (bool state) {
	if (state) {
		initWord9834 |= AD9834_OPBITEN ;
	} else {
		initWord9834 &= (~AD9834_OPBITEN );
	}
		AD9834_WriteRawData (initWord9834);
}

/*It is to control what is output at the IOUT pin/IOUTB pin: 
@0-SINE 1-TRIANGLE*/
void AD9834_SetIoutMode (char mode) {
	if (mode == 0) {
		initWord9834 &= (~AD9834_MODE);
	} else {
		initWord9834 |= AD9834_MODE;
	}
		AD9834_WriteRawData (initWord9834);
	
}
/*choose the phase reg*/
void AD9834_ChoosePhaseReg (char x) {
	if (initWord9834 & AD9834_PIN) {
		/////if there a hardware control
		if (x == 0) {
			AD9834_TurnPhaseLow();
		} else {
			AD9834_TurnPhaseHi();
		}
		return;
	}
	
	if (x == 0) {
		initWord9834 &= (~AD9834_PSEL);
	} else {
		initWord9834 |= AD9834_PSEL;
	}
		AD9834_WriteRawData (initWord9834);
}


 

/***choose freq register**/
void AD9834_ChooseFreqReg(char x){
	
	if (initWord9834 & AD9834_PIN) {
		/*if there a hardware control*/
		if (x == 0) {
			AD9834_TurnFreqLow();
		} else {
			AD9834_TurnFreqHigh();
		}
		return;
	}
	
	if (x == 0) {
		initWord9834 &= (~AD9834_FSEL);
	} else {
		initWord9834 |= AD9834_FSEL;
	}
		AD9834_WriteRawData (initWord9834);
}

/*fast direct write data into  the	FREQ register**/
void AD9834_FastFrequencySet(char reg, unsigned int content){
	 unsigned short mask  = 0;       
	 
	/*define - in which register will be writing*/
	  mask = (reg == 0) ? (1 << 14) : (1 << 15);
     
    /*wite MSB bits -control word*/
    AD9834_WriteRawData(AD9834_HLB | initWord9834);
	  
	//data
    AD9834_WriteRawData(( ( content >> 14) & 0x3fff) | mask );
	 
	/*write LSB bits - control word*/
    AD9834_WriteRawData(0x0000| initWord9834);
	 
	//data
    AD9834_WriteRawData(( (content & 0x3fff) | mask ));
	
}


/**fast direct write data into the Phase register***/
void AD9834_FastPhaseSet(char reg, unsigned short content){

 
	unsigned short mask = (reg == 0) ? (0xc000) : (0xE000);
	 
	AD9834_WriteRawData((content| mask ));
}	




/*----------------- ! ! !   B E G I N  from HERE:-------------------------*/  
 
/************************************
BEGIN initialization of the DDS chip from this function
a) reset the chip b) assion software control (reset e.t.c) c)init FREQ0 and PHASE0 registers.
Before call of this function you can set bits in the variable initWord9834 to activate other DDS chip options: for example SIGN BIT OUT.
You can also init other chip options them after call of this function*/
/*initialization: in according to the datasheet p.24 */
void InitWithSoftwareControl (float Freq, float Phase){
	unsigned short temp;
	// write into configuration world RESET PHSE/FREQ REGISTER choose BY SOFTWARE
	initWord9834 &= (~AD9834_PIN);
	//1)setting reset bit
	AD9834_WriteRawData(AD9834_RESET);
 AD9834_Delay98();
	//2)frequency in reg 0
	AD9834_WriteFrequency (0, Freq);
	//
	//3) phase
	 AD9834_WritePhase(0,Phase);
	/*select registers - frequency and phase 
	in according to initializatin word*/
	AD9834_WriteRawData(initWord9834);
}


/************************************
BEGIN initialization of the DDS chip from this function (this function like a previous one)
a) reset the chip b) assion hardware control (reset e.t.c) c)init FREQ0 and PHASE0 registers */
/*initialization: in according to the datasheet p.24 */
void InitWithHardwareControl (float Freq, float Phase){
	unsigned short temp;
	// write into configuration world RESET PHSE/FREQ REGISTER choose BY SOFTWARE
	initWord9834 &= (~AD9834_PIN);
	/*1)setting reset bit*/
	AD9834_WriteRawData(AD9834_RESET);
 AD9834_Delay98();
	 /*write hardware a control bit PIN/SW
 and turn the DDS 	into hardware (RESET, FreqSEL, PhaseSEL) mode */
	AD9834_WriteRawData(initWord9834);
	/*2)frequency in reg 0*/
	AD9834_WriteFrequency (0, Freq);
	//
	/*3) phase*/
	 AD9834_WritePhase(0,Phase);
	/*select registers - frequency and phase 
	in according to initializatin word*/
	AD9834_WriteRawData(initWord9834);
}


/***function to hardware control FSEL PHSEL RESET pins of the DDS chip**********/
void AD9834_TurnPhaseHi(void){
	/**/
	
}
	void AD9834_TurnPhaseLow(void){
		
	}
	void AD9834_TurnFreqHigh(void){
		
	}
	void AD9834_TurnFreqLow(void){
		
	}
	void AD9834_TurnResetHi(void){
		
	}
	void AD9834_TurnResetLow(void){
		
	}
  
  /*****ATTENTION:****!! example of code***********
  //SPI1->CR1 = SPI_CR1_SPE;
	 z[0]=0x00;
	 z[1] = 0x00;
	 HAL_SPI_Transmit(&hspi1,(uint8_t*)z,2,2);
	 
 
   InitWithSoftwareControl(300.0,1.0);
   AD9834_EnterIntoSleep1();
	 AD9834_WakeUpFromSleep1();
	 AD9834_WritePhase(0,3.0);
	 AD9834_EnableSignBitOut(true);
	 AD9834_SetSignBitOutMode(1,0);
	 AD9834_SetSignBitOutMode(1,1);
	 AD9834_SetSignBitOutMode(0,0);
	  //AD9834_EnableSignBitOut(false);
		AD9834_SetIoutMode(1);
		AD9834_SetIoutMode(0);
		AD9834_WriteFrequency(1,4000);
		AD9834_ChooseFreqReg(1);
		AD9834_ChooseFreqReg(0);
		AD9834_FastFrequencySet(0,3579);//32212255
   ***********************************/
  

