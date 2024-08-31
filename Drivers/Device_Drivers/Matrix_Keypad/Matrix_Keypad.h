/*
 * Matrix_Keypad.h
 *
 *  Created on: Feb 17, 2024
 *      Author: Ebram Maher
 */

#ifndef DEVICE_DRIVERS_MATRIX_KEYPAD_MATRIX_KEYPAD_H_
#define DEVICE_DRIVERS_MATRIX_KEYPAD_MATRIX_KEYPAD_H_
#include "stm32f1xx_hal.h"
#include "stdint.h"

/// @struct keypad
/// @brief
///structure for configuring the keypad
typedef struct keypad {

	///
	/// attribute: the keypad rows
	uint8_t Row;
	///
	/// attribute: the keypad columns
	uint8_t Col;

	///
	/// holds the status of the buttons
	uint8_t Button_Status;

	///
	/// continue the documentation later
	GPIO_TypeDef *Row_Port;
	uint8_t Row_Start_Pin;
	GPIO_TypeDef *Col_Port;
	uint8_t Col_Start_Pin;

	uint32_t Input_Mask;
	uint32_t Output_Mask;

} Keypad_Matrix;

/// @fn void Keypad_Matrix_Init(Keypad_Matrix*)
/// @brief
///
/// @param kp
void Keypad_Matrix_Init(Keypad_Matrix *kp);

/// @fn void Keypad_Matrix_Refresh(Keypad_Matrix*)
/// @brief
///
/// @param kp
void Keypad_Matrix_Refresh(Keypad_Matrix *kp);

/// @fn uint8_t Keypad_Matrix_Read_Key(Keypad_Matrix*, uint8_t)
/// @brief
///
/// @param kp
/// @param button
/// @return
uint8_t Keypad_Matrix_Read_Key(Keypad_Matrix *kp, uint8_t key);

#endif /* DEVICE_DRIVERS_MATRIX_KEYPAD_MATRIX_KEYPAD_H_ */
