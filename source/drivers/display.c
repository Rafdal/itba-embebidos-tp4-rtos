/***************************************************************************//**
  @file     display.c
  @brief    Display driver
  @author   Agustin Fisher
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include "display.h"
#include "display_defs.h"
#include <os.h>

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

// TIMES
#define TIMER_TICK_TIME 5
#define REFRESHING_TIME	(5/TIMER_TICK_TIME)
#define SLIDER_TIME		(500/TIMER_TICK_TIME)
#define CURSOR_TIME 	(200/TIMER_TICK_TIME)

#define WINDOW_SIZE		4U					// Size of the window that the display shows

#define EOL				'\0'				// End of line

#ifndef NULL
#define NULL ((void *)0)
#endif

static const uint32_t segment_pins[SEGS] = {
  	SEG_DP_PIN,
	SEG_G_PIN,
	SEG_F_PIN,
	SEG_E_PIN,
	SEG_D_PIN,
	SEG_C_PIN,
	SEG_B_PIN,
	SEG_A_PIN,
};

/* static const uint8_t char2disp[ALPHABET_SIZE] =
{
	// Bin digits␍
	0b11111100, 0b01100000, 0b11011010, 0b11110010,   // 0123␍
	0b01100110, 0b10110110, 0b10111110, 0b11100000,   // 4567␍
	0b11111110, 0b11110110, 0b11101110, 0b00111110,   // 89AB␍
	0b00011010, 0b01111010, 0b10011110, 0b10001110,   // CDEF␍
	0b10111110, 0b01101110, 0b01100000, 0b01110000,   // GHIJ␍
	0b01101110, 0b00011100, 0b11101100, 0b11101100,   // KLMN␍
	0b11111100, 0b11001110, 0b11100110, 0b00001010,   // OPQR␍
	0b10110110, 0b00011110, 0b01111100, 0b01111100,   // STUV␍
	0b01111110, 0b01101110, 0b01001110, 0b11011010,   // WXYZ␍
	0b00000000   									  // SPACE␍
}; */

static uint8_t char_to_7segment(char c) {
    switch (c) {
        case '0': return 0b11111100;
        case '1': return 0b01100000;
        case '2': return 0b11011010;
        case '3': return 0b11110010;
        case '4': return 0b01100110;
        case '5': return 0b10110110;
        case '6': return 0b10111110;
        case '7': return 0b11100000;
        case '8': return 0b11111110;
        case '9': return 0b11110110;
        case 'A': return 0b11101110;
        case 'B': return 0b00111110;
		case 'C': return 0b00011010;
        case 'D': return 0b01111010;
        case 'E': return 0b10011110;
        case 'F': return 0b10001110;
        case 'G': return 0b10111110;
        case 'H': return 0b01101110;
        case 'I': return 0b01100000;
        case 'J': return 0b01110000;
        case 'K': return 0b01101110;
        case 'L': return 0b00011100;
        case 'M': return 0b11101100;
        case 'N': return 0b11101100;
        case 'O': return 0b11111100;
        case 'P': return 0b11001110;
        case 'Q': return 0b11100110;
        case 'R': return 0b00001010;
        case 'S': return 0b10110110;
        case 'T': return 0b00011110;
        case 'U': return 0b01111100;
        case 'V': return 0b01111100;
        case 'W': return 0b01111110;
        case 'X': return 0b01101110;
        case 'Y': return 0b01001110;
        case 'Z': return 0b11011010;
        case ' ': return 0b00000000;
		case '.': return 0b00000001;
		case '-': return 0b00000010;
		case '_': return 0b00010000;
        default: return 0b00000000; // Default case for unsupported characters
    }
}

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

/**
 * @brief Callback function to show the 4 seven segment displays
 * @param void
 * @return void
 */
static void dispRender(void *p_tmr, void *p_arg);

/**
 * @brief Input a given character in a seven segment display
 * @param character ascii character
 * @return void
 */
static void write_display(char c);

/**
 * @brief Sets virtual display indexes within the scope of the array
 * @param index index to normalize
 * @return normalized index
 */
static uint32_t getIndex(uint32_t index);

/**
 * @brief Callback function that moves the window to be shown
 * @param void
 * @return void
 */
static void slider(void *p_tmr, void *p_arg);

/**
 * @brief Callback function to turn off the segments of the display, making the brightness lower
 * @param void
 * @return void
 */
static void turn_off_segs(void *p_tmr, void *p_arg);

/**
 * @brief Callback function to show the cursor on the display
 * @param void
 * @return void
 */
static void show_cursor(void *p_tmr, void *p_arg);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static uint8_t virtual_display[VDISP_SIZE] = {'\0'};	  // Virtual display
static uint32_t cursor = 0;							  // Virtual display cursor
static bool is_cursor_shown = false;				  // Flag used to show the cursor in the display
static uint32_t window_offset = 0;					  // Window to be showcased in the display

//static uint32_t display_timer_id = 0;				  // Timer to refresh display
//static uint32_t slider_timer_id = 0;					  // Timer for sliding pace
//static uint32_t brightness_timer_id = 0;				  // Timer for brightness level
//static uint32_t cursor_timer_id = 0;

static bool slider_enable = false;					  // Checks if the sliding is enabled or not
static uint8_t brightness_timer = REFRESHING_TIME;


/* Example Timer*/
static OS_TMR display_TMR;					// Timer to refresh display
static OS_TMR slider_TMR;					// Timer for sliding pace
static OS_TMR brightness_TMR;				// Timer for brightness level
static OS_TMR cursor_TMR;

static OS_ERR os_error;

void dispInit(void) {

	// Initialize display timer
	OSTmrCreate(&display_TMR, "dT", 0, REFRESHING_TIME, OS_OPT_TMR_PERIODIC, &dispRender, NULL, &os_error);
	OSTmrStart(&display_TMR, &os_error);

	//display_timer_id = createTimer(REFRESHING_TIME, &dispRender, PERIODIC);
	//startTimer(display_timer_id);

	// Initialize slider timer
	OSTmrCreate(&slider_TMR, "sT", SLIDER_TIME * 2, SLIDER_TIME, OS_OPT_TMR_PERIODIC, &slider, NULL, &os_error);
	OSTmrStart(&slider_TMR, &os_error);
	//slider_timer_id = createTimer(SLIDER_TIME, &slider, PERIODIC);
	//startTimer(slider_timer_id);

	// Initialize brightness timer
	OSTmrCreate(&brightness_TMR, "bT", 0, 0, OS_OPT_TMR_ONE_SHOT, &turn_off_segs, NULL, &os_error);
	//brightness_timer_id = createTimer(brightness_timer, &turn_off_segs, SINGLE);

	// Initialize cursor timer
	OSTmrCreate(&cursor_TMR, "sT", 0, CURSOR_TIME, OS_OPT_TMR_PERIODIC, &show_cursor, NULL, &os_error);
	OSTmrStart(&cursor_TMR, &os_error);
	//cursor_timer_id = createTimer(CURSOR_TIME, &show_cursor, PERIODIC);
	//startTimer(cursor_timer_id);

	// Initialize segment pins
	int i;
	for(i=0; i<SEGS; ++i)
		gpioMode(segment_pins[i], OUTPUT);

	// Initialize enconder
	gpioMode(ENC1_PIN, OUTPUT);
	gpioMode(ENC2_PIN, OUTPUT);

	dispBlank();
}

void setCursor(uint32_t pos) {
	cursor = (pos < VDISP_SIZE) ? pos : cursor;
}

void setWinOffset(uint32_t pos) {
	window_offset = (pos < VDISP_SIZE) ? pos : window_offset;
}

void dispWord(char* word) {
	if (word == NULL)
		return;
	while(*word != EOL)
	{
		dispChar(*word);
		cursor = (cursor == VDISP_SIZE-1) ? 0 : cursor+1;
		++word;
	}
	for(int i=0; i<4; i++){
		dispChar(' ');
		cursor = (cursor == VDISP_SIZE-1) ? 0 : cursor+1;
		++word;
	}

}

void dispChar(char character) {
	virtual_display[cursor] = character;
}

void dispBlank(void) {
	int i;
	for(i=0; i<VDISP_SIZE; ++i)
		virtual_display[i] = EOL;
}

void dispBlankCursor(void) {
	virtual_display[cursor] = 0;
}

void dispEnableSlider(bool a) {
	slider_enable = a;
}

void setBrightness(uint32_t brightness) {
	brightness_timer = brightness/20;
	//setTimerTime(brightness_timer_id, brightness_timer);
}

void move_window(uint32_t pos) {
	window_offset = getIndex(pos);
}

void disp_reset_slider(void) {
	window_offset = 0;
	OSTmrStart(&slider_TMR, &os_error);
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

static void dispRender(void *p_tmr, void *p_arg) {

	static uint8_t display_num = 0;											// Number of display to be written
	uint32_t num_index = getIndex(window_offset+display_num);		// Position in array

	bool enc1_val = (display_num & 0b10) >> 1;								// Logic value of encoder´s MSB
	bool enc2_val = display_num & 0b01;										// Logic value of encoder´s LSB

	gpioWrite(ENC1_PIN, enc1_val);
	gpioWrite(ENC2_PIN, enc2_val);


	// Show the corresponding character in the seven segment display
	bool show_cursor = (cursor == num_index) && (is_cursor_shown == true);
	write_display(show_cursor ? ' ' : virtual_display[num_index]);

	display_num = (display_num == WINDOW_SIZE-1) ? 0 : display_num+1;					// Get the next character to show
}

static void write_display(char c) {

	c = (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c; // Convert to uppercase if it is a lowercase letter
	uint8_t char2segs = char_to_7segment(c);

	int i;
	for(i=0; i<8; ++i)
		gpioWrite(segment_pins[i], char2segs & (1 << i));

	if(brightness_timer != REFRESHING_TIME){
		OSTmrStart(&brightness_TMR, &os_error);
		//startTimer(brightness_timer_id);
	}
}

static uint32_t getIndex(uint32_t index) {
	return index % (VDISP_SIZE);
}

static void slider(void *p_tmr, void *p_arg) {
	if(slider_enable)
	{
		window_offset = getIndex(window_offset+1);
		if (virtual_display[getIndex(window_offset+3)] == EOL)
		{
			//setTimerCurrentTime(slider_timer_id, SLIDER_TIME * 4);
			window_offset = 0;
		}
	}
}

static void turn_off_segs(void *p_tmr, void *p_arg) {
	int i;
	for(i=0; i<8; ++i)
		gpioWrite(segment_pins[i], LOW);
}

static void show_cursor(void *p_tmr, void *p_arg) {
	is_cursor_shown = is_cursor_shown ? false : true;
}

/*******************************************************************************
 ******************************************************************************/
