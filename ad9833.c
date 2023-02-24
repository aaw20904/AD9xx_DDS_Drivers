 
/**The library for AD9833 Analog Devices IC.It implements for the STM32 and HAL**/
/********Author: Andrii Androsovych***************/

#include "stm32f1xx_hal.h"
#include "main.h"
 
extern SPI_HandleTypeDef hspi1; 
#include "main.h"
 
extern SPI_HandleTypeDef hspi1; 
extern DMA_HandleTypeDef hdma_spi1_tx;
/** definition adresses of the AD9833 chip**/
#define AD9833_CONTROL_REGISTER 0
#define AD9833_FREQ1_REG (1 << 15)
#define AD9833_FREQ0_REG (1 << 14)
#define AD9833_PHASE0_REG (1 << 14)|(1 << 15)  
#define AD9833_PHASE1_REG (1 << 14)|(1 << 15)|(1 << 13)
/***define ad9833 control registr bits**/
#define AD9833_CTRL_B28 (1 << 13)
#define AD9833_CTRL_HLB (1 << 12)
#define AD9833_CTRL_FSELECT (1 << 11)
#define AD9833_CTRL_PSELECT (1 << 10)
#define AD9833_CTRL_RESET (1 << 8)
#define AD9833_CTRL_SLEEP (1 << 7)
#define AD9833_CTRL_SLEEP12 (1 << 6)
#define AD9833_CTRL_OPBITEN (1 << 5)
#define AD9833_CTRL_DIV2 (1 << 3)
#define AD9833_CTRL_HMODE (1 << 1)

/**the minimal step of the frequency tuning  Delta= Fcrystal / 2^28
in this case Fclock = 25MHz**/
const float AD9833_freqStep = 0.093132;

/*set FSYNC pin to HIGH: implement this function for your own configuration*/
  void   AD9834_TurnSyncHi (void) {
   GPIOA->BSRR = GPIO_BSRR_BS6;
}
/*set FSYNC pin to LOW: implement this function for your own configuration*/
  void   AD9834_TurnSyncLo (void) {
  GPIOA->BSRR = GPIO_BSRR_BR6;	
}


 void    AD9834_Delay98(void){
  unsigned short y = 0;
  while(y <  100) {
    y++;
  }
}


//calculate a raw data for writing into DDS IC`s registers 
unsigned int AD9833_calculateDividerCoeficient (unsigned int frequency) {
  float result = (float)frequency / AD9833_freqStep;
  return (unsigned int)result;
}
/**write to register FREQ**/
short AD9833_writeToFREQ(unsigned int value, unsigned short reg){
 /*firstly write in a control register B28*/
    
 unsigned short static transmitData;
 transmitData =  AD9833_CTRL_B28; 
	AD9834_TurnSyncLo();
 HAL_SPI_Transmit(&hspi1,(uint8_t*)&transmitData,1,2);
	AD9834_TurnSyncHi ();
	AD9834_Delay98();
 /**secondly transmit LOW 14bits*/
	AD9834_TurnSyncLo();
  transmitData = (value & 0x00003fff) | reg;
  HAL_SPI_Transmit(&hspi1, (uint8_t*)&transmitData,1,2);
	AD9834_TurnSyncHi ();
	AD9834_Delay98();
	AD9834_TurnSyncLo();
 /**thridly transmit HIGH 14bits*/
  transmitData = ((value & 0xfffc000) >> 14) | reg;
  HAL_SPI_Transmit(&hspi1,(uint8_t*)&transmitData,1,2);
	AD9834_TurnSyncHi ();
    
  return 0;
}



/**write to register PHASE**/
short AD9833_writeToPHASE(unsigned int value, unsigned short reg){
 /*write data in a phase0 register - first 12bits*/
 unsigned short static transmitData;
 transmitData =  (value & 0x00000fff) | reg; 
 HAL_SPI_Transmit(&hspi1,(uint8_t*)&transmitData,1,2);
 return 0;
}

/**DMA transmitting routines**/
short AD9833_writeToFREQ_DMA(unsigned int value, unsigned short reg) {
 
 /*firstly write in a control register B28*/
 unsigned short static transmitData[3];
 transmitData[0] =  AD9833_CTRL_B28; 
  /**secondly transmit LOW 14bits*/
  transmitData[1] = (value & 0x00003fff) | reg;
   /**thridly transmit HIGH 14bits*/
  transmitData[2] = ((value & 0xfffc000) >> 14) | reg;
   // GPIOA->BSRR=GPIO_BSRR_BS_10;
 return HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)transmitData, 3);
}



/*initializing the device with PHASE0 and FREQ0 registers**/
short ad9833_Init(void) {
  unsigned char buf[2];
  unsigned short static dataToTransmit;
  /***make reset and set B28 bit***/
  dataToTransmit = AD9833_CTRL_RESET | AD9833_CTRL_B28;///  0x2100;
	AD9834_TurnSyncLo();
		AD9834_Delay98();
     HAL_SPI_Transmit(&hspi1,(uint8_t*)&dataToTransmit,1,2);
		AD9834_Delay98();
		AD9834_TurnSyncHi ();
     HAL_Delay(1);
    /**write frequency in FREQ0- low14bits and high14bits consequently **/
       dataToTransmit = AD9833_FREQ0_REG | 0x3fff;///0x50C7;//
	AD9834_TurnSyncLo();
		AD9834_Delay98();
     HAL_SPI_Transmit(&hspi1,(uint8_t*)&dataToTransmit, 1, 2);
		AD9834_Delay98();
		AD9834_TurnSyncHi ();
     HAL_Delay(1);
     /**wrile high 14 bits to FRQ0 register***/
      dataToTransmit = AD9833_FREQ0_REG;
	AD9834_TurnSyncLo();
		AD9834_Delay98();
     HAL_SPI_Transmit(&hspi1,(uint8_t*)&dataToTransmit, 1, 2);
		 	AD9834_Delay98();
		AD9834_TurnSyncHi ();
     /**write in a phase register PHASE0*/
       dataToTransmit =  AD9833_PHASE0_REG; //0xC000
	AD9834_TurnSyncLo();
		AD9834_Delay98();
     HAL_SPI_Transmit(&hspi1, (uint8_t*)&dataToTransmit, 1, 2);
		 	AD9834_Delay98();
		 	AD9834_TurnSyncHi ();
     /***clear RST**/
       dataToTransmit = AD9833_CTRL_B28;///0x2000;//
			 AD9834_TurnSyncLo();
			 	AD9834_Delay98();
     HAL_SPI_Transmit(&hspi1,buf,1,2);
		 	AD9834_Delay98();
		 	AD9834_TurnSyncHi ();
      HAL_Delay(1);
      return 0;
}
    
