/***************************************************************************//**
  @file     lector.c
  @brief    Driver for magnetic card reader
  @author   Houses
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "lector.h"
#include "gpio.h"
#include "../config.h"

#include <stdint.h>
#include <stdbool.h>

#include  <os.h>
#ifndef NULL
#define NULL ((void *)0)
#endif

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define EN_SAMPLE_MS	5
#define LRC_VAL			0b0000		//the result expected from an xor of all data bits

enum {WAITING_SS, READING_DATA};


#define SETB_ADDRS (0x400FF044u)
#define SETB_PTR ((uint32_t *)SETB_ADDRS)
#define SETB_VAL (1<<9)
#define LECTOR_TIMER_PERIOD 10

#define CLRB_ADDRS (0x400FF048u)
#define CLRB_PTR ((uint32_t *)CLRB_ADDRS)
#define CLRB_VAL (1<<9)
#define DEBUG_TP_SET *(SETB_PTR) = SETB_VAL
#define DEBUG_TP_CLR	*(CLRB_PTR) = CLRB_VAL
/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
static void read_lector_data(void);
static void parse_data(void * p_arg);
/**
 * @brief Callback for the enable reading timer
 */
void read_enable(void);

/*Static Functions*/
static void lector_tmr_callback(void *p_tmr, void *p_arg);


/*******************************************************************************
 * 						PRIVATE GLOBAL VARIALBES
 ******************************************************************************/

static uint8_t bits[MAX_LENGTH*WORD_LENGTH];	//array for data bits
static uint8_t words[MAX_LENGTH];
static uint8_t count;							//counter for the actual bit number

static bool enable_state = true;				//state of the enable pin. Is active low
static uint8_t state;							//state, reading or waiting for start sentinel

static uint8_t data_status;						//State of the reading process, based on the enum DATA_READY, READ_ERR, DATA_NOT_READY
static DataTrk2_t data;


static OS_ERR os_error;
static OS_ERR os_err_TaskParse;
static OS_ERR os_err_tmr;


/* Example Timer*/
static OS_TMR lector_TMR;



static OS_SEM semLectorEnable;		//semaphor for lector enable function
static OS_SEM* initSemPtr;

/* Task 3 */
#define TASK_PARSE_STK_SIZE 			512u
#define TASK_PARSE_STK_SIZE_LIMIT	(TASK_PARSE_STK_SIZE / 2u)
#define TASK_PARSE_PRIO				4u
static OS_TCB TaskParseTCB;
static CPU_STK TaskParseStk[TASK_PARSE_STK_SIZE];


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void lector_tmr_callback(void *p_tmr, void *p_arg){
	read_enable();
}



void lector_Init (OS_SEM* SemPtr){

	//Config pins for data input
	gpioMode(PIN_LECTOR_CLK, INPUT);
	gpioMode(PIN_LECTOR_DATA, INPUT);
	gpioMode(PIN_LECTOR_EN, INPUT);
	initSemPtr = SemPtr;

	OSSemCreate(&semLectorEnable, "SemEn", 0u, &os_error);
	//Clock has its dedicated interrupt, enable is read periodically to check
	bool err1 = gpioIRQ(PIN_LECTOR_CLK, GPIO_IRQ_MODE_FALLING_EDGE, read_lector_data);
	//bool err2 = pisrRegister(read_enable, EN_SAMPLE_MS);
	OSTaskCreate(&TaskParseTCB,
	    			"TaLec",
					parse_data,
					0u,
					TASK_PARSE_PRIO,
					&TaskParseStk[0u],
					TASK_PARSE_STK_SIZE_LIMIT,
					TASK_PARSE_STK_SIZE,
					0u,
					0u,
					0u,
					(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),
					&os_err_TaskParse);
	// Initialize slider timer
	//read_enable_id = createTimer(EN_SAMPLE_MS, &read_enable, PERIODIC);
	//startTimer(read_enable_id);


}




/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/**
 * @brief Callback to read the enable pin.
 */
void read_enable(void){
	bool enable = gpioRead(PIN_LECTOR_EN);			//enable pin is active low!

	if(enable && (!enable_state)){									//If enable has a positive edge, data is parsed to find errors
		state = WAITING_SS;
		OSSemPost(&semLectorEnable, OS_OPT_POST_1, &os_error);
	}
	enable_state = enable;
}

/**
 * @brief Callback for the dedicated interrupt. Reads the data pin. Waits for SS to start saving data.
 */
static void read_lector_data(void){

	static uint8_t current_char=0;
	uint8_t bit_read = !gpioRead(PIN_LECTOR_DATA);

	/*if(!enable){
	bits[count++] = !(gpioRead(PIN_LECTOR_DATA));
	}
	if(count>199){
		count =0;
	}*/
	if(!enable_state){			//Enable active low!!!!
		switch (state){
		case WAITING_SS:		//waits until SS is received

			current_char = (current_char >> 1) | (bit_read << (WORD_LENGTH-1));		//FIFO but bitwise, last bit is lost, new one enters at bit number 4 (5th bit).
			if (current_char == START_SENT){
				state = READING_DATA;
				//bits[0] = current_char;
				count = 0;
				current_char =0;
				words[0]=(START_SENT & 0b1111);
			}

			break;

		case READING_DATA:
			if(count < (MAX_LENGTH*WORD_LENGTH)){				//if counter exceeds array size, dont write
				bits[count++] = !(gpioRead(PIN_LECTOR_DATA));		//collect data
			}
			break;
		}
	}

}

/**
 * @brief Parses the read bits. Detects parity errors. Sets error flag if any error occurs, and if End Sentinel is not found
 */

static void parse_data(void* p_arg)
{
	OSTmrCreate(&lector_TMR, "T1", 0, LECTOR_TIMER_PERIOD, OS_OPT_TMR_PERIODIC, lector_tmr_callback, NULL, &os_err_tmr);
	OSTmrStart(&lector_TMR, &os_err_tmr);


	while(1){

	OSSemPend(&semLectorEnable, 0, OS_OPT_PEND_BLOCKING, NULL, &os_err_TaskParse);
	uint8_t word =0;
	uint8_t bit =0;
	uint8_t bit_parity =0;
	uint8_t lrc_parity = 0;
	lrc_parity ^= words[0];
	uint8_t err = 0;
	bool found = false;
	int i,j;
	for(i=0; i<(count/WORD_LENGTH); i++){				//count/5 should be an integer, and is the amount of words read
		for(j=0; j < WORD_LENGTH-1; j++){				// WORD_LENGTH-1 because the last bit is the parity bit, only considered when checking data validity
			bit = bits[i*WORD_LENGTH +j];				//read bit
			bit_parity^=bit;							//check parity for all bits
			word = (word >> 1) | (bit << (WORD_LENGTH-2));		//Add each bit to the word. -2 because the msb is bit 3
		}
		bit_parity^= (bits[i*WORD_LENGTH + WORD_LENGTH-1]);		//parity bit, skipped at the previous for
		lrc_parity^=word;								//total word parity. Doesnt include parity bit

		if(bit_parity){									//parity should be 1
			words[i+1] = word;							//+1 because Start Sentinel is already saved

		}
		else{											//Set the error flag
			err =1;										//1 if error
			break;

		}
		if(word == END_SENT && !found){
			count = (i+2)*WORD_LENGTH;					//If end sentinel is detected, then the following word is the LRC and then trash can appear, not interested in trash
			found = true;								//LRC can be the same as End Sentinel!!!! I discovered it by luck xd
		}
		bit_parity =0;
	}

	if((!found) || (lrc_parity != LRC_VAL)){							//set error flag if LRC is not 0xF, or the End sentinel is not found
		err = 1;
	}
	data_status = err;				//0 means DATA_READY, 1 means READ_ERR. So err flag coincides with the enum
	OSSemPost(initSemPtr, OS_OPT_POST_1, &os_err_TaskParse);		//Post for data ready
	}
}


void card_number_to_string(uint64_t card_number, char *str, uint8_t length)
{
	int i = 0;
	while ((card_number != 0) && (i<length))
	{
		int digit = card_number % 10;
		str[length-i-1] = (char)('0' + digit);
		card_number /= 10;
		i++;
	}
	str[length] = '\0'; // Null-terminate the string
	// Now str contains the number as a string, but in reverse order
}

uint64_t string_to_card_number(char *str, uint8_t length)
{
	uint64_t number = 0;
	for (int i = 0; i < length; i++)
	{
		number = number * 10 + (str[i] - '0');
	}
	return number;
}


/**
 * @brief Get the card number. Does not check if data is ready, be careful
 * @return uint64_t card number, at most 19 digits.
 */
uint64_t get_card_number(void){
	uint64_t number =0;
	for(int i =1; i<=NUM_MAX_LENGTH; i++){
		if(words[i] == FIELD_SEP){
			break;
		}
		else{
			number = number*10 + words[i];
		}

	}
	data.number = number;
	return number;
}

/**
 * @brief Read the track 2 data from the magnetic reader
 * @return structure containing all track2 data
 */
DataTrk2_t get_trk2(void){
	return data;
}


