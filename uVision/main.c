#include "Adc.h"
#include "Uart.h"
#include "Gpio.h"

extern uint8_t state;

extern uint8_t sensor;//Sa zicem ca 0 este pentru lumina, 1 pentru vibratii
extern uint8_t led_state_gpio;//am avea 0 = low = verde, 1 = medium = galben; 2 = high = rosu;
extern uint8_t led_state_adc;

uint64_t counter = 0;

int main() {
	
	UART0_Init(115200);
	OutputPIN_Init();
	RGBLed_Init();
	ADC0_Init();
	char buffer[6];
	while(1){
		
		//bootle timer
		if(counter < 9999)
			counter++;
		else
		{
			
			if(sensor == 0){
				ADC0_IRQHandler();
				led_state_gpio = led_state_adc;
			}else{
				int temp = state;
				itoa(temp, buffer, 6);
				UART0_Transmit_String("Vibration: ");
				UART0_Transmit_String(buffer);
				UART0_Transmit('\n');
				UART0_Transmit(0xD);	
				state = 0;
			}
			PORTD_IRQHandler();
			counter = 0;
		}
			
	}
	
}