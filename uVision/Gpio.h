#include "MKL25Z4.h"
#include "Uart.h"

#define LOW 0
#define HIGH 1

void OutputPIN_Init(void);
void PORTA_IRQHandler(void);
void PORTD_IRQHandler(void);
void RGBLed_Init(void);
void LedChanger(void);
