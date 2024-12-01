#include "gpio.h"
#include "../../SDK/CMSIS/MK64F12.h"
#include "../../SDK/startup/hardware.h"
#include <stdint.h>
#include <stdbool.h>

static PORT_Type * const ports [5] = PORT_BASE_PTRS;
static GPIO_Type * const gpios [5] = GPIO_BASE_PTRS;
static SIM_Type * const sim_ptr = SIM;

static pinIrqFun_t handlers[5][32];	//callbacks para los handlers
static uint32_t irq_pins[5];	// pins that have already registered interrupts

static uint8_t irq_ports[5] = PORT_IRQS;

static char clocked;
uint32_t port;
uint32_t num;

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// Gets the nth bit of variable n. n goes fron 0 to N-1
#define getBitn(x, n) ( ((x)>>(n)) & (0x1U) )

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Configures the specified pin to behave either as an input or an output
 * @param pin the pin whose mode you wish to set (according PORTNUM2PIN)
 * @param mode INPUT, OUTPUT, INPUT_PULLUP or INPUT_PULLDOWN.
 */
void gpioMode (pin_t pin, uint8_t mode){
    port = PIN2PORT(pin);
    num = PIN2NUM(pin);

    if(!clocked){
    	uint32_t bits_ON = SIM_SCGC5_PORTA(0x1U)+SIM_SCGC5_PORTB(0x1U)+SIM_SCGC5_PORTC(0x1U)+SIM_SCGC5_PORTD(0x1U)+SIM_SCGC5_PORTE(0x1U);
    	sim_ptr->SCGC5 |= bits_ON;
    	clocked = 1;
    }

    PORT_Type * portx = ports[port];
    GPIO_Type * gpiox = gpios[port];
    portx->PCR[num] = PORT_PCR_MUX(0x1);
    switch (mode){
        case INPUT:
            gpiox->PDDR &= ~(1<<num);    // pongo un 0 en el bit deseado para que sea input
            break;
        case OUTPUT:
            gpiox->PDDR |= (1<<num);
            break;
        case INPUT_PULLUP:
            gpiox->PDDR &= ~(1<<num);
            portx->PCR[num] |= (PORT_PCR_PE(0x1U) | PORT_PCR_PS(0x1U));
            break;
        case INPUT_PULLDOWN:
            gpiox->PDDR &= ~(1<<num);
            portx->PCR[num] |= PORT_PCR_PE(0x1U);
            portx->PCR[num] &= ~(PORT_PCR_PS(0x1U));
            break;
    }

}

/**
 * @brief Configures how the pin reacts when an IRQ event ocurrs
 * @param pin the pin whose IRQ mode you wish to set (according PORTNUM2PIN)
 * @param irqMode disable, risingEdge, fallingEdge or bothEdges
 * @param irqFun function to call on pin event
 * @return 1 if error (already asigned an interrupt to the pin), 0 if success
 */
bool gpioIRQ (pin_t pin, uint8_t irqMode, pinIrqFun_t irqFun){
	port = PIN2PORT(pin);
	num = PIN2NUM(pin);
	bool ret = ((irq_pins[port]) >> num) & (0x1U);		//shift to get the flag if the pin has already enabled interrupt

	if(!ret)			//if 1, the pin has enabled interrupts
	{
		PORT_Type * portx = ports[port];
		NVIC_EnableIRQ(irq_ports[port]);				//Enable port interrupts
		portx->PCR[num] |= PORT_PCR_IRQC(irqMode);     //Enable interrupts (mode defined by irqMode)

		//Save the callback and turn on pin number flag
		irq_pins[port] |= 1<<num;
		handlers[port][num] = irqFun;
	}
	return ret;
}

bool gpioIRQEnable(pin_t pin, uint8_t irqMode){
	port = PIN2PORT(pin);
	num = PIN2NUM(pin);
	bool ret = ((irq_pins[port]) >> num) & (0x1U);		//shift to get the flag if the pin has enabled interrupt

	if(!ret)			//if 1, the pin has enabled interrupts
	{
		PORT_Type * portx = ports[port];
		portx->PCR[num] |= PORT_PCR_IRQC(irqMode);     //Enable interrupts (mode defined by irqMode)
		irq_pins[port] |= 1<<num;
	}
	return ret;
}

bool gpioIRQDisable(pin_t pin){
	port = PIN2PORT(pin);
	num = PIN2NUM(pin);
	bool ret = ((irq_pins[port]) >> num) & (0x1U);		//shift to get the flag if the pin has enabled interrupt

	if(ret)			//if 1, the pin has enabled interrupts
	{
		PORT_Type * portx = ports[port];
		portx->PCR[num] &= ~((uint32_t)PORT_PCR_IRQC_MASK);     //Disable interrupts

		//Turn off pin number flag
		irq_pins[port] &= ~(1<<num);
	}
	return ret;
}

/**
 * @brief Write a HIGH or a LOW value to a digital pin
 * @param pin the pin to write (according PORTNUM2PIN)
 * @param val Desired value (HIGH or LOW)
 */
void gpioWrite (pin_t pin, bool value){
    port = PIN2PORT(pin);
    num = PIN2NUM(pin);
    GPIO_Type * gpiox = gpios[port];
    if(value){
        gpiox->PSOR = (1<<num);
    }
    else{
        gpiox->PCOR = (1<<num);
    }

}

/**
 * @brief Toggle the value of a digital pin (HIGH<->LOW)
 * @param pin the pin to toggle (according PORTNUM2PIN)
 */
void gpioToggle (pin_t pin){
    port = PIN2PORT(pin);
    num = PIN2NUM(pin);
    GPIO_Type * gpiox = gpios[port];

    gpiox->PTOR = (1<<num);
}


/**
 * @brief Reads the value from a specified digital pin, either HIGH or LOW.
 * @param pin the pin to read (according PORTNUM2PIN)
 * @return HIGH or LOW
 */
bool gpioRead (pin_t pin){
	port = PIN2PORT(pin);
	num = PIN2NUM(pin);
	GPIO_Type * gpiox = gpios[port];
	bool output = ((1<<num) & gpiox->PDIR);
	return output;
}


__ISR__ PORTA_IRQHandler(void)
{
	uint32_t isfra= PORTA->ISFR;
	for(int i=0; i<32; i++){
		if( getBitn(isfra,i) ){
			PORTA->PCR[i] |= PORT_PCR_ISF_MASK;  // Clear interrupt flag
			handlers[PA][i]();
		}
	}
}

__ISR__ PORTB_IRQHandler(void)
{
	uint32_t isfrb= PORTB->ISFR;
	for(int i=0; i<32; i++){
		if( getBitn(isfrb,i) ){
			PORTB->PCR[i] |= PORT_PCR_ISF_MASK;  // Clear interrupt flag
			handlers[PB][i]();
		}
	}
}

__ISR__ PORTC_IRQHandler(void)
{

	uint32_t isfrc= PORTC->ISFR;
	for(int i=0; i<32; i++){
		if( getBitn(isfrc,i) ){
			PORTC->PCR[i] |= PORT_PCR_ISF_MASK;  // Clear interrupt flag
			handlers[PC][i]();
		}
	}
}

__ISR__ PORTD_IRQHandler(void)
{
	uint32_t isfrd= PORTD->ISFR;
	for(int i=0; i<32; i++){
		if( getBitn(isfrd,i) ){
			PORTD->PCR[i] |= PORT_PCR_ISF_MASK;  // Clear interrupt flag
			handlers[PD][i]();
		}
	}

}

__ISR__ PORTE_IRQHandler(void)
{
	uint32_t isfre= PORTE->ISFR;
	for(int i=0; i<32; i++){
		if( getBitn(isfre,i) ){
			PORTE->PCR[i] |= PORT_PCR_ISF_MASK;  // Clear interrupt flag
			handlers[PE][i]();
		}
	}
}
