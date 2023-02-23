

/**adresses definition**/
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
short ad9833_Init(void);
unsigned int AD9833_calculateDividerCoeficient (unsigned int frequency);
short AD9833_writeToFREQ(unsigned int value,unsigned short reg);
short AD9833_writeToPHASE(unsigned int value,unsigned short reg);
short AD9833_writeToFREQ_DMA(unsigned int value, unsigned short reg);
 void   AD9834_TurnSyncHi (void);
/*set FSYNC pin to LOW: implement this function for your own configuration*/
  void   AD9834_TurnSyncLo (void);


 void    AD9834_Delay98(void);