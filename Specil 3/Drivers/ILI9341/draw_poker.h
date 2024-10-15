/*
 * draw_poker.h
 *
 *  Created on: Oct 6, 2024
 *      Author: LENOVO
 */

#ifndef ILI9341_DRAW_POKER_H_
#define ILI9341_DRAW_POKER_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

void resetScreen();
void resetTouchScreen();
void setScreen(int playerNumber);
void setTouchScreen(int screenNumber);
void drawPlayerCard(Player player);
void displayHand(Player player, int playerNumber, int currentPlayer);
void displayPokerHand(Player player, int playerNumber, const char *handName);
void displayWinnerPlayer(Player player, int playerNumber, const char *handName);
void disPlayCommunityCard(Card card, int index);
void Process_Touch(int screenNumber, int *choice, Game *game, Player *player);
uint8_t Check_Touch(int currentPlayer);
void displayMenu(int playerNumber, Game *game, Player *player);
int selectedChoice(uint16_t x, uint16_t y);
void displayBet(int playerNumber, int raiseAmount);
void processBet(int screenNumber, int *endBet, int *raiseAmount);

#endif /* ILI9341_DRAW_POKER_H_ */
