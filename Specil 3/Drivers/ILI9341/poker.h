#ifndef POKER_H
#define POKER_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define NUM_SUITS 4
#define NUM_RANKS 13
#define NUM_CARDS_IN_HAND 7
#define STARTING_CHIPS 1000
#define SMALL_BLIND 10
#define BIG_BLIND 20

#define BLACK       0x0000
#define NAVY        0x000F
#define DARKGREEN   0x03E0
#define DARKCYAN    0x03EF
#define MAROON      0x7800
#define PURPLE      0x780F
#define OLIVE       0x7BE0
#define LIGHTGREY   0xC618
#define DARKGREY    0x7BEF
#define BLUE        0x001F
#define GREEN       0x07E0
#define CYAN        0x07FF
#define RED         0xF800
#define MAGENTA     0xF81F
#define YELLOW      0xFFE0
#define WHITE       0xFFFF
#define ORANGE      0xFD20
#define GREENYELLOW 0xAFE5
#define PINK        0xF81F

typedef enum
{
	PREGAME, // Wait for assign Player and Bot
	DEALCARD,
    PREFLOP,
    FLOP,
    TURN,
    RIVER,
	FINISH, // Wait for Continue or New Game or Exit
    SHOWDOWN // Exit
} GameState;

typedef enum
{
    CHECK,
    FOLD,
    RAISE
} Action;

typedef struct
{
    char *rank;
    char *suit;
    int rankValue;
    const uint32_t *symbol;
    uint16_t colour;
} Card;

typedef struct
{
    Card hand[2];
    int chips;
    int currentBet;
    int folded;
    int allIn;
    int is_bot;
} Player;

typedef struct
{
    int pot;
    int currentBet;
    GameState state;
    int button;
    Card communityCards[5];
    int numCommunityCards;
} Game;

typedef enum {
    HIGH_CARD,      // 0
    ONE_PAIR,       // 1
    TWO_PAIR,       // 2
    THREE_OF_KIND,  // 3
    STRAIGHT,       // 4
    FLUSH,          // 5
    FULL_HOUSE,     // 6
    FOUR_OF_KIND,   // 7
    STRAIGHT_FLUSH, // 8
} PokerHand;

extern const uint32_t SPADE[];
extern const uint32_t DIAMOND[];
extern const uint32_t CLUB[];
extern const uint32_t HEART[];
extern const uint32_t MENU[];

extern Card deck[52];
extern Player players[10];
extern Card communityCards[5];

extern PokerHand pokerHands[9];
extern const char *handNames[];

// Function declarations
void teraPrint(char message[]);
void initializeGame(Game *game);
void postBlinds(Player players[], Game *game);
void createDeck(Card deck[]);
void shuffleDeck(Card deck[]);
void initializePlayers(Player players[], uint8_t n_players, uint8_t n_bots);
void dealInitialCards(Card deck[], Player players[], int *deckIndex);
void dealCommunityCards(Card deck[], Game *game, int numCards, int *deckIndex);
void displayPlayerStatus(Player players[], int currentPlayer);
void displayCommunityCards(Game *game);
void displayGameState(Game *game);
Action getPlayerAction(Player *player, Game *game, int currentPlayer);
void handlePlayerAction(Player *player, Game *game, Action action, int currentPlayer);
int bettingRound(Player players[], Game *game, int startPlayer);
void getFullHand(Card fullHand[], Player player, Card communityCards[]);
int compareCards(const void *a, const void *b);
int evaluateHand(Player player, Card communityCards[]);
int getHighestCard(Card fullHand[]);
void findWinner(Player players[], Card communityCards[]);
void playPokerGame(uint8_t n_players, uint8_t n_bots);
Action show_suggest(Player player, Game game);

#endif // POKER_H
