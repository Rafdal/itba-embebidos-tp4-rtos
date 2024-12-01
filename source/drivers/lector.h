/***************************************************************************//**
  @file     lector.h
  @brief    Driver for the magnetic card reader
  @author   Houses
 ******************************************************************************/

#ifndef _LECTOR_
#define _LECTOR_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include  <os.h>
/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/
#define MAX_LENGTH 40
#define WORD_LENGTH 5
#define NUM_MAX_LENGTH 19
#define START_SENT 0xB
#define FIELD_SEP 0XD
#define END_SENT 0xF


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
enum {DATA_READY, READ_ERR, DATA_NOT_READY};

typedef struct {
	uint64_t number;
	uint16_t expiration_YYMM;
	uint8_t service_code;
	uint64_t discr_data;
}DataTrk2_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initialize driver
 *
 */
void lector_Init (OS_SEM* SemPtr);

/**
 * @brief Read the track 2 data from the magnetic reader
 * @return structure containing all track2 data
 */
DataTrk2_t get_trk2(void);

/**
 * @brief Get the card number. Does not check if data is ready, be careful
 * @return uint64_t card number, at most 19 digits.
 */
uint64_t get_card_number(void);



void card_number_to_string(uint64_t card_number, char *str, uint8_t length);
uint64_t string_to_card_number(char *str, uint8_t length);

/**
 * @brief Actual status of the reading process.
 * @return Returns the sate of the reading based on the enum: DATA_READY, READ_ERR, DATA_NOT_READY
 */
uint8_t get_lector_status(void);


/*******************************************************************************
 ******************************************************************************/

#endif // _LECTOR_
