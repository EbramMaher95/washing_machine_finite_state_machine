/*
 * Matrix_Keypad.c
 *
 *  Created on: Feb 17, 2024
 *      Author: Ebram MAher
 */

#include "Matrix_Keypad.h"
/// @fn void Hardware_intialize(Keypad_Matrix*)
/// @brief
///
/// @param kp
static void Hardware_intialize(Keypad_Matrix *kp) {

	GPIO_InitTypeDef GP = {

	.Mode = GPIO_MODE_OUTPUT_PP, .Speed = GPIO_SPEED_LOW,

	};

	uint32_t Pins = 0;

	for (int x = 0; x < kp->Row; x++) {

		Pins |= (1 << ((kp->Row_Start_Pin) + x));
	}

	GP.Pin = Pins;

	HAL_GPIO_Init(kp->Row_Port, &GP);

	kp->Output_Mask = Pins;

	GP.Mode = GPIO_MODE_INPUT;
	GP.Pull = GPIO_PULLDOWN;

	Pins = 0;

	for (int x = 0; x < kp->Col; x++) {

		Pins |= (1 << ((kp->Col_Start_Pin) + x));
	}

	GP.Pin = Pins;

	HAL_GPIO_Init(kp->Col_Port, &GP);

	kp->Input_Mask = Pins;

}

/// @fn void Hardware_Generate_Delay(uint16_t)
/// @brief
///
/// @param delay
static void Hardware_Generate_Delay(uint16_t delay) {

	HAL_Delay(delay);

}

static uint8_t Hardware_Get_Inputs(Keypad_Matrix *kp) {

	uint32_t m = kp->Col_Port->IDR & kp->Input_Mask;

	m = (m >> (kp->Col_Start_Pin));

	return (uint8_t) m;

}

static void Hardware_Set_outputs(Keypad_Matrix *kp, uint8_t status) {

	kp->Row_Port->ODR &= ~(kp->Output_Mask);

	kp->Row_Port->ODR |= ((uint32_t) status) << (kp->Row_Start_Pin);
}

void Keypad_Matrix_Init(Keypad_Matrix *kp) {

	Hardware_intialize(kp);

	Hardware_Set_outputs(kp, 0);

}

/// @fn void Keypad_Matrix_Refresh(Keypad_Matrix*)
/// @brief
///
/// @param kp
void Keypad_Matrix_Refresh(Keypad_Matrix *kp) {

	uint8_t scan = 0b00000001, x;

	kp->Button_Status = 0;

	for (x = 0; x < kp->Row; x++) {

		Hardware_Set_outputs(kp, scan);

		Hardware_Generate_Delay(1);

		kp->Button_Status |= (Hardware_Get_Inputs(kp) << (x * (kp->Col)));

		scan = scan << 1;
	}

}

uint8_t Keypad_Matrix_Read_Key(Keypad_Matrix *kp, uint8_t key) {

	if (kp->Button_Status & (1 << (key))) {
		return 1;
	}

	else {
		return 0;
	}

}

