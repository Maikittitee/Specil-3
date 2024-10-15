#include "poker.h"
#include "draw_poker.h"
#include "usart.h"
#include "stm32f7xx_it.h"

uint8_t count = 0;

#include "gpio.h"
#include "ILI9341_Touchscreen.h"
#include "ILI9341_STM32_Driver.h"
#include "ILI9341_GFX.h"

void resetScreen() {
	HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS3_GPIO_Port, CS3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS4_GPIO_Port, CS4_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(CS5_GPIO_Port, CS5_Pin, GPIO_PIN_SET);
}

void resetTouchScreen() {
	HAL_GPIO_WritePin(T_CS1_GPIO_Port, T_CS1_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(T_CS2_GPIO_Port, T_CS2_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(T_CS3_GPIO_Port, T_CS3_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(T_CS4_GPIO_Port, T_CS4_Pin, GPIO_PIN_SET);
}

void setScreen(int playerNumber) {
	switch (playerNumber) {
	case 1:
		HAL_GPIO_WritePin(CS1_GPIO_Port, CS1_Pin, GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(CS2_GPIO_Port, CS2_Pin, GPIO_PIN_RESET);
		break;
	case 3:
		HAL_GPIO_WritePin(CS3_GPIO_Port, CS3_Pin, GPIO_PIN_RESET);
		break;
	case 4:
		HAL_GPIO_WritePin(CS4_GPIO_Port, CS4_Pin, GPIO_PIN_RESET);
		break;
	case 5:
		HAL_GPIO_WritePin(CS5_GPIO_Port, CS5_Pin, GPIO_PIN_RESET);
	default:
		break;
	}
	ILI9341_Fill_Screen(WHITE);
}

void setTouchScreen(int screenNumber) {
	switch (screenNumber) {
	case 1:
		HAL_GPIO_WritePin(T_CS1_GPIO_Port, T_CS1_Pin, GPIO_PIN_RESET);
		break;
	case 2:
		HAL_GPIO_WritePin(T_CS2_GPIO_Port, T_CS2_Pin, GPIO_PIN_RESET);
		break;
	case 3:
		HAL_GPIO_WritePin(T_CS3_GPIO_Port, T_CS3_Pin, GPIO_PIN_RESET);
		break;
	case 4:
		HAL_GPIO_WritePin(T_CS4_GPIO_Port, T_CS4_Pin, GPIO_PIN_RESET);
		break;
	default:
		break;
	}
}

void drawPlayerCard(Player player) {
	ILI9341_Draw_FirstCard(player.hand[0].symbol, player.hand[0].rank, player.hand[0].colour);
	ILI9341_Draw_SecondCard(player.hand[1].symbol, player.hand[1].rank, player.hand[1].colour);
}

void displayHand(Player player, int playerNumber, int currentPlayer) {
	char message[100];
	resetScreen();
	setScreen(playerNumber);

	sprintf(message, "Player %d", playerNumber);
	if (playerNumber == currentPlayer + 1) {
		ILI9341_Draw_Text(message, 170, 10, PURPLE, 2, BLACK);
	} else {
		ILI9341_Draw_Text(message, 170, 10, BLACK, 2, WHITE);
	}

	if (player.folded) {
		ILI9341_Fill_Screen(LIGHTGREY);
		sprintf(message, "(FOLD)");
		ILI9341_Draw_Text(message, 120, 30, BLACK, 3, WHITE);
	}

	ILI9341_Draw_Symbol(5, 5, BLUE, MENU, 1);
	drawPlayerCard(player);
	sprintf(message, "Chips %d", player.chips);
	ILI9341_Draw_Text(message, 100, 200, BLACK, 2, WHITE);

	resetScreen();
}

void displayPokerHand(Player player, int playerNumber, const char *handName) {
	char message[50];
	resetScreen();
	setScreen(playerNumber);
	sprintf(message, "%s", handName);
	ILI9341_Draw_Text(message, 50, 20, BLACK, 3, WHITE);
	sprintf(message, "Chips %d", player.chips);
	ILI9341_Draw_Text(message, 100, 200, BLACK, 2, WHITE);
	drawPlayerCard(player);
	resetScreen();
}

void displayWinnerPlayer(Player player, int playerNumber, const char *handName) {
	char message[50];
	resetScreen();
	HAL_GPIO_WritePin(CS5_GPIO_Port, CS5_Pin, GPIO_PIN_RESET);
	ILI9341_Fill_Screen(WHITE);
	sprintf(message, "Player %d Win!!", playerNumber);
	ILI9341_Draw_Text(message, 50, 10, BLACK, 2, WHITE);
	sprintf(message, "%s", handName);
	ILI9341_Draw_Text(message, 70, 50, BLACK, 2, WHITE);
	drawPlayerCard(player);
	resetScreen();
}

void disPlayCommunityCard(Card card, int index) {
	resetScreen();
	HAL_GPIO_WritePin(CS5_GPIO_Port, CS5_Pin, GPIO_PIN_RESET);
	switch (index) {
	case 1:
		ILI9341_Draw_Symbol(10, 10, card.colour, card.symbol, 2);
		ILI9341_Draw_Text(card.rank, 80, 20, BLACK, 5, WHITE);
		break;
	case 2:
		ILI9341_Draw_Symbol(110, 10, card.colour, card.symbol, 2);
		ILI9341_Draw_Text(card.rank, 180, 20, BLACK, 5, WHITE);
		break;
	case 3:
		ILI9341_Draw_Symbol(10, 90, card.colour, card.symbol, 2);
		ILI9341_Draw_Text(card.rank, 80, 100, BLACK, 5, WHITE);
		break;
	case 4:
		ILI9341_Draw_Symbol(110, 90, card.colour, card.symbol, 2);
		ILI9341_Draw_Text(card.rank, 180, 100, BLACK, 5, WHITE);
		break;
	case 5:
		ILI9341_Draw_Symbol(10, 170, card.colour, card.symbol, 2);
		ILI9341_Draw_Text(card.rank, 80, 180, BLACK, 5, WHITE);
		break;
	default:
		break;
	}
	HAL_Delay(500);
	resetScreen();
}

void Process_Touch(int screenNumber, int *choice, Game *game, Player *player) {
	uint16_t x_pos, y_pos;
	char message[100];
	uint16_t position_array[2];
	resetTouchScreen();
	setTouchScreen(screenNumber);
	if(TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK)
	{
	  x_pos = position_array[0];
	  y_pos = position_array[1];

	  if (x_pos != 0 && y_pos != 0) {
		  sprintf(message, "Press Screen %d at (%d,%d)\r\n", screenNumber, x_pos, y_pos);
		  while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET) {}
		  HAL_UART_Transmit(&huart3, (uint8_t*) message, strlen(message), 10);
		  if ( x_pos >= 0 && x_pos < 90 && y_pos >= 200 && y_pos < 350) {
			  displayMenu(screenNumber, game, player);
		  }
		  *choice = selectedChoice(x_pos, y_pos);
	  }
	}
	resetTouchScreen();
}

uint8_t Check_Touch(int currentPlayer) {
	switch (currentPlayer) {
	case 1:
		return TP_Touchpad_Pressed();
		break;
	case 2:
		return TP_Touchpad2_Pressed();
		break;
	case 3:
		return TP_Touchpad3_Pressed();
		break;
	case 4:
		return TP_Touchpad4_Pressed();
		break;
	default:
		return 0;
		break;
	}
}

void displayMenu(int playerNumber, Game *game, Player *player) {
	resetScreen();
	setScreen(playerNumber);
	ILI9341_Fill_Screen(BLUE);
	if (game->currentBet == player->currentBet) {
		ILI9341_Draw_Text("Check", 80, 20, WHITE, 4, BLACK);
	}
	else {
		char message[50];
		sprintf(message, "Call %d", game->currentBet - player->currentBet);
		ILI9341_Draw_Text(message, 80, 20, WHITE, 4, BLACK);
	}
	ILI9341_Draw_Text("Fold", 110, 90, WHITE, 4, BLACK);
	ILI9341_Draw_Text("Bet", 120, 160, WHITE, 4, BLACK);

	resetScreen();
}

int selectedChoice(uint16_t x, uint16_t y) {
//	if (x >= 20 && x <= 80 && y >= 80 && y <= 150) {
	if (x >= 0 && x <= 300 && y >= 0 && y <= 300) {
		while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET) {}
		HAL_UART_Transmit(&huart3, (uint8_t*) "CHECK\r\n", strlen("CHECK\r\n"), 10);
		return 1;
	}
//	else if (0) {
//		while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET) {}
//		HAL_UART_Transmit(&huart3, (uint8_t*) "FOLD\r\n", strlen("FOLD\r\n"), 10);
//		return 2;
//	} else if (x >= 160 && x <= 200 && y >= 80 && y <= 150) {
//		while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET) {}
//		HAL_UART_Transmit(&huart3, (uint8_t*) "BET\r\n", strlen("BET\r\n"), 10);
//		return 3;
//	}
	return 0;
}

void displayBet(int playerNumber, int raiseAmount) {
	resetScreen();
	setScreen(playerNumber);
	ILI9341_Draw_Text("+", 40, 90, WHITE, 4, BLACK);
	char message[20];
	sprintf(message, "%d", raiseAmount);
	ILI9341_Draw_Text(message, 130, 90, BLACK, 4, WHITE);
	ILI9341_Draw_Text("-", 230, 90, WHITE, 4, BLACK);
	ILI9341_Draw_Text("Enter", 90, 190, WHITE, 4, BLACK);
	resetScreen();
}

void processBet(int screenNumber, int *endBet, int *raiseAmount) {
	uint16_t x_pos, y_pos;
	char message[100];
	uint16_t position_array[2];
	resetTouchScreen();
	setTouchScreen(screenNumber);
	if(TP_Read_Coordinates(position_array) == TOUCHPAD_DATA_OK && count % 2 == 0)
	{
		x_pos = position_array[0];
		y_pos = position_array[1];

		sprintf(message, "Press Bet Screen %d at (%d,%d)\r\n", screenNumber, x_pos, y_pos);
		while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET) {}
		HAL_UART_Transmit(&huart3, (uint8_t*) message, strlen(message), 10);

		if ( x_pos >= 90 && x_pos < 130 && y_pos >= 250 && y_pos < 300) {
		  // +
		  *raiseAmount += 10;
		} else if ( x_pos >= 90 && x_pos < 130 && y_pos >= 40 && y_pos < 100) {
		  // -
		  if (*raiseAmount > 0) {
			  *raiseAmount -= 10;
		  }
		} else if ( x_pos >= 190 && x_pos < 240 && y_pos >= 100 && y_pos < 230) {
		  // End Bet
		  *endBet = 1;
		}
		displayBet(screenNumber, *raiseAmount);
	}
	resetTouchScreen();
}
