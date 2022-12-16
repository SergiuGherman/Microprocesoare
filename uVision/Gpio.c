#include "Gpio.h"

#define VIB_PIN (12) // PORT A
#define SWITCH_PIN_D (4) // PORT D
#define RED_LED_PIN (18) // PORT B
#define GREEN_LED_PIN (19) // PORT B
#define BLUE_LED_PIN (1) // PORT D

uint8_t state = 0;
uint8_t old_led_state = 100;
uint8_t led_state_gpio = 0;
uint8_t leds_on[3];


void OutputPIN_Init(void){
	///PORTD
	// Activarea semnalului de ceas pentru a putea folosi GPIO cu ajutorul pinului 1 de pe portul C
	SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
	
	// Utilizarea GPIO impune selectarea variantei de multiplexare cu valorea 1
	PORTD->PCR[SWITCH_PIN_D] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[SWITCH_PIN_D] |= PORT_PCR_MUX(1);
	
	// Activare întreruperi pe rising edge
	//PORTD->PCR[SWITCH_PIN] |= PORT_PCR_IRQC(0x0b) | PORT_PCR_PE_MASK;//pentru relese si push
	PORTD->PCR[SWITCH_PIN_D] |= PORT_PCR_IRQC(0x09) | PORT_PCR_PE_MASK;//pentru push
	//PORTD->PCR[SWITCH_PIN_D] |= PORT_PCR_IRQC(0x0a) | PORT_PCR_PE_MASK;//pentru relese 

	
	// Activare întrerupere pentru a folosi switch-u
	NVIC_ClearPendingIRQ(PORTD_IRQn);
	NVIC_SetPriority(PORTD_IRQn, 128);
	NVIC_EnableIRQ(PORTD_IRQn);
	
	//PORT A
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	
	PORTA->PCR[VIB_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTA->PCR[VIB_PIN] |= PORT_PCR_MUX(1);
	
	//GPIOA->PDDR &= (0<<VIB_PIN);
	
	// Activare întreruperi pe rising edge
	PORTA->PCR[VIB_PIN] |= PORT_PCR_IRQC(0x0A) | PORT_PCR_PE_MASK;
	
	NVIC_ClearPendingIRQ(PORTA_IRQn);
	NVIC_SetPriority(PORTA_IRQn, 128);
	NVIC_EnableIRQ(PORTA_IRQn);
	
}

void RGBLed_Init(void){
	
	// Activarea semnalului de ceas pentru pinii folositi în cadrul led-ului RGB
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTD_MASK;
	
	// --- RED LED ---
	
	// Utilizare GPIO ca varianta de multiplexare
	PORTB->PCR[RED_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[RED_LED_PIN] |= PORT_PCR_MUX(1);
	
	// Configurare pin pe post de output
	GPIOB_PDDR |= (1<<RED_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOB_PCOR |= (1<<RED_LED_PIN);
	
	// --- GREEN LED ---
	
	// Utilizare GPIO ca varianta de multiplexare
	PORTB->PCR[GREEN_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTB->PCR[GREEN_LED_PIN] |= PORT_PCR_MUX(1);
	
	// Configurare pin pe post de output
	GPIOB_PDDR |= (1<<GREEN_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOB_PSOR |= (1<<GREEN_LED_PIN);
	
	// --- BLUE LED ---
	
		// Utilizare GPIO ca varianta de multiplexare
	PORTD->PCR[BLUE_LED_PIN] &= ~PORT_PCR_MUX_MASK;
	PORTD->PCR[BLUE_LED_PIN] |= PORT_PCR_MUX(1);
	
	// Configurare pin pe post de output
	GPIOD_PDDR |= (1<<BLUE_LED_PIN);
	
	// Stingerea LED-ului (punerea pe 0 logic)
	GPIOD_PSOR |= (1<<BLUE_LED_PIN);
	
	GPIOB_PTOR |= (1<<RED_LED_PIN);
	
	leds_on[0] = 1;
	leds_on[1] = 0;
	leds_on[2] = 0;
}


void PORTD_IRQHandler() {	
	
	if(led_state_gpio == 0){
		//green state
		if(leds_on[0] == 0)
			GPIOB_PTOR |= (1<<RED_LED_PIN);
		if(leds_on[1] == 0)
			GPIOB_PTOR |= (1<<GREEN_LED_PIN);
		
		leds_on[0] = 1;
		leds_on[1] = 1;
			
	}else if(led_state_gpio == 1){
		//yellow state
		if(leds_on[0] == 1)
			GPIOB_PTOR |= (1<<RED_LED_PIN);
		if(leds_on[1] == 0)
			GPIOB_PTOR |= (1<<GREEN_LED_PIN);
		
		leds_on[0] = 0;
		leds_on[1] = 1;
	}else{
		//red State
		if(leds_on[0] == 1)
			GPIOB_PTOR |= (1<<RED_LED_PIN);
		if(leds_on[1] == 1)
			GPIOB_PTOR |= (1<<GREEN_LED_PIN);
		
		leds_on[0] = 0;
		leds_on[1] = 0;
	}
	
	
	PORTD_ISFR = (1<<SWITCH_PIN_D);
}

void PORTA_IRQHandler() {
	state++;
	
	if(state < 100)
		led_state_gpio = 0;
	else if(state < 200)
		led_state_gpio = 1;
	else 
		led_state_gpio = 2;
	
	PORTA_ISFR = (1<<VIB_PIN);
}
