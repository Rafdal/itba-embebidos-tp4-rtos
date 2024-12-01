/***************************************************************************//**
  @file     display.h
  @brief    Display driver
  @author   Agustin Fisher
 ******************************************************************************/

#ifndef _DISPLAY_H_
#define _DISPLAY_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdbool.h>
#include <stdint.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

#define VDISP_SIZE		30U

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initializes display timers and pins setup
 * @param void
 * @return void
 */
void dispInit(void);

/**
 * @brief Set virtual display cursor position
 * @param pos where you want the cursor to be
 * @return void
 */
void setCursor(uint32_t pos);

/**
 * @brief Sets the window offset for the virtual display rolling
 * @param pos where you want the window to start
 * @return void
 */
void setWinOffset(uint32_t pos);

/**
 * @brief Load a word in the virtual display
 * @param word string to be displayed
 * @return void
 */
void dispWord(char* word);

/**
 * @brief Put a char in the virtual display
 * @param character ascii character from the supported set
 * @return void
 */
void dispChar(char character);

/**
 * @brief Blank the 7 segment displays
 * @param void
 * @return void
 */
void dispBlank(void);

/**
 * @brief Delete character the cursor on
 * @param void
 * @return void
 */
void dispBlankCursor(void);

/**
 * @brief Enable/Disable rolling the text on the display
 * @param a true / false
 * @return void
 */
void dispEnableSlider(bool a);

/**
 * @brief Set the 7 segment display brightness
 * @param brightness brightness level
 * @return void
 */
void setBrightness(uint32_t brightness);

void disp_reset_slider(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _	DISPLAY_H_
