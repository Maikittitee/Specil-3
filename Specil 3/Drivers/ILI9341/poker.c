#include "poker.h"
#include "draw_poker.h"
#include "usart.h"
#include "joystick.h"

const char *suits[] = {"Hearts", "Diamonds", "Clubs", "Spades"};
const char *ranks[] = {"2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K", "A"};
const int rankValues[] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14};
int NUM_PLAYERS = 4;


PokerHand pokerHands[9] = {
	HIGH_CARD,      // 0
	ONE_PAIR,       // 1
	TWO_PAIR,       // 2
	THREE_OF_KIND,  // 3
	STRAIGHT,       // 4
	FLUSH,          // 5
	FULL_HOUSE,     // 6
	FOUR_OF_KIND,   // 7
	STRAIGHT_FLUSH, // 8
};

const uint32_t SPADE[] = {
	0b000000000000010000000000000,
	0b000000000000110000000000000,
	0b000000000000111000000000000,
	0b000000000001111100000000000,
	0b000000000011111110000000000,
	0b000000000111111111000000000,
	0b000000001111111111100000000,
	0b000000011111111111110000000,
	0b000000111111111111111000000,
	0b000001111111111111111100000,
	0b000011111111111111111110000,
	0b000111111111111111111111000,
	0b001111111111111111111111100,
	0b001111111111111111111111100,
	0b011111111111111111111111110,
	0b011111111111111111111111110,
	0b111111111111111111111111111,
	0b111111111111111111111111111,
	0b111111111111111111111111111,
	0b111111111111111111111111111,
	0b111111111111111111111111111,
	0b011111111111111111111111110,
	0b011111111111111111111111110,
	0b001111111111111111111111100,
	0b000111111111111011111111000,
	0b000001111000111000111110000,
	0b000000000000111000000000000,
	0b000000000001111100000000000,
	0b000000000001111100000000000,
	0b000000000001111110000000000,
	0b000000000011111110000000000,
	0b000000000111111111000000000
};

const uint32_t DIAMOND[] = {
	0b00000000000000000000000000000000,
	0b00000000000000010000000000000000,
	0b00000000000000111000000000000000,
	0b00000000000000111100000000000000,
	0b00000000000001111110000000000000,
	0b00000000000011111110000000000000,
	0b00000000000011111111000000000000,
	0b00000000000111111111100000000000,
	0b00000000001111111111110000000000,
	0b00000000011111111111110000000000,
	0b00000000011111111111111000000000,
	0b00000000111111111111111100000000,
	0b00000001111111111111111110000000,
	0b00000001111111111111111110000000,
	0b00000011111111111111111111000000,
	0b00000111111111111111111111100000,
	0b00000011111111111111111111000000,
	0b00000001111111111111111110000000,
	0b00000001111111111111111110000000,
	0b00000000111111111111111100000000,
	0b00000000011111111111111000000000,
	0b00000000011111111111111000000000,
	0b00000000001111111111110000000000,
	0b00000000000111111111100000000000,
	0b00000000000011111111100000000000,
	0b00000000000011111111000000000000,
	0b00000000000001111110000000000000,
	0b00000000000000111110000000000000,
	0b00000000000000111100000000000000,
	0b00000000000000011000000000000000,
	0b00000000000000001000000000000000,
	0b00000000000000000000000000000000

};

const uint32_t HEART[] = {
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000001111100000000111110000000,
	0b00001111111111000011111111110000,
	0b00011111111111100111111111111000,
	0b00111111111111111111111111111100,
	0b00111111111111111111111111111100,
	0b01111111111111111111111111111110,
	0b01111111111111111111111111111110,
	0b11111111111111111111111111111111,
	0b11111111111111111111111111111111,
	0b11111111111111111111111111111111,
	0b01111111111111111111111111111110,
	0b01111111111111111111111111111110,
	0b01111111111111111111111111111110,
	0b00111111111111111111111111111100,
	0b00111111111111111111111111111100,
	0b00011111111111111111111111111000,
	0b00001111111111111111111111110000,
	0b00000111111111111111111111100000,
	0b00000011111111111111111111000000,
	0b00000000111111111111111110000000,
	0b00000000011111111111111100000000,
	0b00000000001111111111111000000000,
	0b00000000000111111111110000000000,
	0b00000000000011111111000000000000,
	0b00000000000001111110000000000000,
	0b00000000000000111100000000000000,
	0b00000000000000011000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000
};

const uint32_t CLUB[] = {
	0b000000000000111111000000000000,
	0b000000000011111111100000000000,
	0b000000000111111111111000000000,
	0b000000001111111111111000000000,
	0b000000001111111111111100000000,
	0b000000011111111111111100000000,
	0b000000011111111111111110000000,
	0b000000011111111111111110000000,
	0b000000011111111111111110000000,
	0b000000011111111111111110000000,
	0b000011111111111111111111110000,
	0b000111111111111111111111111000,
	0b001111111111111111111111111100,
	0b011111111111111111111111111110,
	0b011111111111111111111111111110,
	0b111111111111111111111111111111,
	0b111111111111111111111111111111,
	0b111111111111111111111111111111,
	0b111111111111111111111111111111,
	0b111111111111111111111111111111,
	0b011111111111111111111111111110,
	0b011111111111111111111111111110,
	0b001111111111111111111111111100,
	0b000111111111111111111111111000,
	0b000011111111111111111111110000,
	0b000000000001111111100000000000,
	0b000000000011111111100000000000,
	0b000000000011111111110000000000,
	0b000000000111111111111000000000,
	0b000000001111111111111000000000,
	0b000000011111111111111100000000,
	0b000000011111111111111110000000
};

const uint32_t MENU[] = {
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00011111111111111111111111111000,
	0b00111111111111111111111111111100,
	0b00111111111111111111111111111100,
	0b00011111111111111111111111111000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00011111111111111111111111111000,
	0b00111111111111111111111111111100,
	0b00111111111111111111111111111100,
	0b00011111111111111111111111111000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00011111111111111111111111111000,
	0b00111111111111111111111111111100,
	0b00111111111111111111111111111100,
	0b00011111111111111111111111111000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000,
	0b00000000000000000000000000000000
};

const char *handNames[] = {
    "HIGH CARD",
    "ONE PAIR",
    "TWO PAIR",
    "THREE OF KIND",
    "STRAIGHT",
    "FLUSH",
    "FULL HOUSE",
    "FOUR OF KIND",
    "STRAIGHT FLUSH"
};

void teraPrint(char message[]) {
	while (__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET) {}
	HAL_UART_Transmit(&huart3, (uint8_t*) message, strlen(message), 10);
}

void initializeGame(Game *game)
{
    game->pot = 0;
    game->currentBet = 0;
    game->state = PREFLOP;
    game->button = 0;
    game->numCommunityCards = 0;
}

void postBlinds(Player players[], Game *game)
{
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        players[i].chips -= BIG_BLIND;
        players[i].currentBet = BIG_BLIND;
        game->pot += BIG_BLIND;
    }
}

void createDeck(Card deck[])
{
    int index = 0;
    for (int suit = 0; suit < NUM_SUITS; suit++)
    {
        for (int rank = 0; rank < NUM_RANKS; rank++)
        {
            deck[index].rank = (char *)ranks[rank];
            deck[index].suit = (char *)suits[suit];
            deck[index].rankValue = rankValues[rank];
            if (suit == 0) {
            	deck[index].symbol = HEART;
            	deck[index].colour = RED;
            } else if (suit == 1) {
            	deck[index].symbol = DIAMOND;
            	deck[index].colour = RED;
            } else if (suit == 2) {
            	deck[index].symbol = CLUB;
            	deck[index].colour = BLACK;
            } else if (suit == 3) {
            	deck[index].symbol = SPADE;
            	deck[index].colour = BLACK;
            }
            index++;
        }
    }
}

void shuffleDeck(Card deck[])
{
    for (int i = 0; i < 52; i++)
    {
        int j = rand() % 52;
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void initializePlayers(Player players[], uint8_t n_players, uint8_t n_bots)
{
	int i = 0;
    while (i < n_players)
    {
        players[i].chips = STARTING_CHIPS;
        players[i].currentBet = 0;
        players[i].folded = 0;
        players[i].allIn = 0;
        players[i].is_bot = 0;
        i++;
    }
    while (i < NUM_PLAYERS){
        players[i].chips = STARTING_CHIPS;
        players[i].currentBet = 0;
        players[i].folded = 0;
        players[i].allIn = 0;
        players[i].is_bot = 1;
        i++;
    }
}

void dealInitialCards(Card deck[], Player players[], int *deckIndex)
{
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        for (int j = 0; j < 2; j++)
        {
            players[i].hand[j] = deck[(*deckIndex)++];
        }
    }
}

void dealCommunityCards(Card deck[], Game *game, int numCards, int *deckIndex)
{
    for (int i = 0; i < numCards; i++)
    {
        game->communityCards[game->numCommunityCards++] = deck[(*deckIndex)++];
    }
}

void displayPlayerStatus(Player players[], int currentPlayer)
{
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
    	char message[100];
        sprintf(message, "Player %d: Chips: %d, Bet: %d%s%s%s\r\n",
               i + 1,
               players[i].chips,
               players[i].currentBet,
               players[i].folded ? " (FOLDED)" : "",
               players[i].allIn ? " (ALL-IN)" : "",
               i == currentPlayer ? " <- CURRENT" : "");
        teraPrint(message);
        displayHand(players[i], i+1, currentPlayer);
    }
}

void displayCommunityCards(Game *game)
{
    printf("Community cards: ");
    for (int i = 0; i < game->numCommunityCards; i++)
    {
    	char message[100];
        printf("[%s of %s] ", game->communityCards[i].rank, game->communityCards[i].suit);
        sprintf(message, "[%s of %s] ", game->communityCards[i].rank, game->communityCards[i].suit);
        teraPrint(message);
        disPlayCommunityCard(game->communityCards[i],i + 1);
    }
    printf("\n");
    teraPrint("\r\n");
}

void displayGameState(Game *game) {
    const char *stateNames[] = {"PREFLOP", "FLOP", "TURN", "RIVER", "SHOWDOWN"};
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, 1);
    char message[100];
    printf("=== Game Status ===\n");
    teraPrint("=== Game Status ===\r\n");
    printf("State: %s\n", stateNames[game->state]);
    sprintf(message, "State: %s\r\n", stateNames[game->state]);
    teraPrint(message);
    printf("Pot: %d\n", game->pot);
    sprintf(message, "Pot: %d\r\n", game->pot);
	teraPrint(message);
    printf("Current bet: %d\n", game->currentBet);
    sprintf(message, "Current bet: %d\r\n", game->currentBet);
    teraPrint(message);
    printf("==================\n\n");
    teraPrint("==================\r\n\n");
}

Action getPlayerAction(Player *player, Game *game, int currentPlayer)
{
    printf("Your hand: [%s of %s] [%s of %s]\n",
           player->hand[0].rank, player->hand[0].suit,
           player->hand[1].rank, player->hand[1].suit);

//    printf("Choose action:\n");
//    if (game->currentBet == player->currentBet)
//    {
//        printf("1. Check\n");
//    }
//    else
//    {
//        printf("1. Call %d\n", game->currentBet - player->currentBet);
//    }
//    printf("2. Fold\n");
//    printf("3. Raise\n");
//
//    int choice;
//    scanf("%d", &choice);
    int choice = 0;

    while (!choice)
    {
    	if (Check_Touch(currentPlayer + 1)) {
			Process_Touch(currentPlayer + 1, &choice, game, player);
		}
    }

	switch (choice)
	{
	case 1:
		return CHECK;
	case 2:
		return FOLD;
	case 3:
		return RAISE;
	default:
		return CHECK;
	}
}

void handlePlayerAction(Player *player, Game *game, Action action, int currentPlayer)
{
    switch (action)
    {
    case CHECK:
        if (game->currentBet > player->currentBet)
        {
            int callAmount = game->currentBet - player->currentBet;
            if (callAmount >= player->chips)
            {
                callAmount = player->chips;
                player->allIn = 1;
            }
            player->chips -= callAmount;
            game->pot += callAmount;
            player->currentBet = game->currentBet;
            printf("Player calls %d\n", callAmount);
        }
        else
        {
            printf("Player checks\n");
        }
        break;
    case FOLD:
        player->folded = 1;
        printf("Player folds\n");
        break;
    case RAISE:
        if (!player->allIn)
        {
            int minRaise = game->currentBet * 2;
            int raiseAmount = 0;
            int endBet = 0;
            displayBet(currentPlayer + 1, raiseAmount);
            do
            {
                printf("Current bet: %d\n", game->currentBet);
                printf("Enter raise amount (minimum %d): ", minRaise - game->currentBet);
                // scanf("%d", &raiseAmount);
                if (Check_Touch(currentPlayer + 1)) {
                	processBet(currentPlayer + 1, &endBet, &raiseAmount);
				}
                // char message[50];
                // sprintf(message, "Current Bet : %d\r\n", raiseAmount);
                // teraPrint(message);
            } while (!endBet);
            raiseAmount += game->currentBet; // Total bet including call
            if (raiseAmount >= player->chips)
            {
                raiseAmount = player->chips;
                player->allIn = 1;
            }

            int actualRaise = raiseAmount - player->currentBet;
            player->chips -= actualRaise;
            game->pot += actualRaise;
            player->currentBet = raiseAmount;
            game->currentBet = raiseAmount;
            printf("Player raises to %d\n", raiseAmount);
        }
        break;
    }
}

int bettingRound(Player players[], Game *game, int startPlayer)
{
    int currentPlayer = startPlayer;
    int playersInHand = 0;
    int lastRaisePlayer = -1;
    int playersActed = 0;

    if (!game || !players)
    		return;
    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        if (!players[i].folded && !players[i].allIn)
        {
            playersInHand++;
        }
    }

    if (game->state == PREFLOP)
    {
        game->currentBet = BIG_BLIND;
    } else {
        game->currentBet = 0;
    }

    while (playersInHand > 1)
    {
        if (players[currentPlayer].folded || players[currentPlayer].allIn)
        {
            currentPlayer = (currentPlayer + 1) % NUM_PLAYERS;
            continue;
        }

        system("cls");

        printf("Current bet: %d\n", game->currentBet);
        printf("Pot: %d\n", game->pot);

        displayGameState(game);
        displayCommunityCards(game);
        displayPlayerStatus(players, currentPlayer);

        printf("\nPlayer %d's turn\n", currentPlayer + 1);
        Action action;
        if (!players[currentPlayer].is_bot){
        	show_suggest(players[currentPlayer], *game);
        	action = getPlayerAction(&players[currentPlayer], game, currentPlayer);
        }
        else {
        	action = show_suggest(players[currentPlayer], *game);
        }
        handlePlayerAction(&players[currentPlayer], game, action, currentPlayer);

        if (action == RAISE)
        {
            lastRaisePlayer = currentPlayer;
            playersActed = 0;
        }
        else if (action == FOLD)
        {
            playersInHand--;
        }

        playersActed++;

        if ((lastRaisePlayer == -1 && playersActed >= playersInHand) ||
            (lastRaisePlayer != -1 && currentPlayer == lastRaisePlayer && playersActed >= playersInHand))
        {
            break;
        }

        currentPlayer = (currentPlayer + 1) % NUM_PLAYERS;
    }

    for (int i = 0; i < NUM_PLAYERS; i++)
    {
        players[i].currentBet = 0;
    }

    return playersInHand;
}

void getFullHand(Card fullHand[], Player player, Card communityCards[])
{
    fullHand[0] = player.hand[0];
    fullHand[1] = player.hand[1];
    for (int i = 0; i < 5; i++)
    {
        fullHand[i + 2] = communityCards[i];
    }
}

int compareCards(const void *a, const void *b)
{
    return ((Card *)b)->rankValue - ((Card *)a)->rankValue;
}

int evaluateHand(Player player, Card communityCards[])
{
    Card fullHand[NUM_CARDS_IN_HAND];
    getFullHand(fullHand, player, communityCards);
    qsort(fullHand, NUM_CARDS_IN_HAND, sizeof(Card), compareCards);

    // Count occurrences of ranks and suits
    int rankCount[NUM_RANKS] = {0};
    int suitCount[NUM_SUITS] = {0};

    for (int i = 0; i < NUM_CARDS_IN_HAND; i++)
    {
        rankCount[fullHand[i].rankValue - 2]++;
        for (int j = 0; j < NUM_SUITS; j++)
        {
            if (strcmp(fullHand[i].suit, suits[j]) == 0)
            {
                suitCount[j]++;
                break;
            }
        }
    }

    // Check for flush
    int flush = 0;
    for (int i = 0; i < NUM_SUITS; i++)
    {
        if (suitCount[i] >= 5)
        {
            flush = 1;
            break;
        }
    }

    // Check for straight
    int straight = 0;
    int consecutive = 0;
    for (int i = NUM_RANKS - 1; i >= 0; i--)
    {
        if (rankCount[i] > 0)
        {
            consecutive++;
            if (consecutive >= 5)
            {
                straight = 1;
                break;
            }
        }
        else
        {
            consecutive = 0;
        }
    }

    // Special case for Ace-low straight (A, 2, 3, 4, 5)
    if (rankCount[12] > 0 && rankCount[0] > 0 && rankCount[1] > 0 &&
        rankCount[2] > 0 && rankCount[3] > 0)
    {
        straight = 1;
    }

    // Count pairs, three of a kind, four of a kind
    int pairs = 0, threeOfKind = 0, fourOfKind = 0;
    for (int i = 0; i < NUM_RANKS; i++)
    {
        if (rankCount[i] == 2)
            pairs++;
        if (rankCount[i] == 3)
            threeOfKind++;
        if (rankCount[i] == 4)
            fourOfKind++;
    }

    // Determine hand rank
    if (straight && flush)
        return 9; // Straight Flush
    if (fourOfKind)
        return 8; // Four of a Kind
    if (threeOfKind && pairs)
        return 7; // Full House
    if (flush)
        return 6; // Flush
    if (straight)
        return 5; // Straight
    if (threeOfKind)
        return 4; // Three of a Kind
    if (pairs == 2)
        return 3; // Two Pair
    if (pairs == 1)
        return 2; // One Pair
    return 1;     // High Card
}

int getHighestCard(Card fullHand[])
{
    int highest = -1;
    for (int i = 0; i < NUM_CARDS_IN_HAND; i++)
    {
        if (fullHand[i].rankValue > highest)
        {
            highest = fullHand[i].rankValue;
        }
    }
    return highest;
}

void findWinner(Player players[], Card communityCards[])
{
    int bestPlayer = 0;
    Card bestFullHand[NUM_CARDS_IN_HAND];
    getFullHand(bestFullHand, players[0], communityCards);
    int bestHandRank = evaluateHand(players[0], communityCards);

    char message[100];
    printf("Player 1's hand rank: %d\n", bestHandRank);
    sprintf(message, "Player 1's hand rank: %d\r\n", bestHandRank);
    teraPrint(message);
    displayPokerHand(players[0], 1, handNames[bestHandRank - 1]);

    for (int i = 1; i < NUM_PLAYERS; i++)
    {
        Card currentFullHand[NUM_CARDS_IN_HAND];
        getFullHand(currentFullHand, players[i], communityCards);
        int handRank = evaluateHand(players[i], communityCards);
        printf("Player %d's hand rank: %d\n", i + 1, handRank);
        sprintf(message, "Player %d's hand rank: %d\r\n", i + 1, handRank);
		teraPrint(message);
        displayPokerHand(players[i], i + 1, handNames[handRank - 1]);

        if (handRank > bestHandRank)
        {
            bestHandRank = handRank;
            bestPlayer = i;
            memcpy(bestFullHand, currentFullHand, sizeof(Card) * NUM_CARDS_IN_HAND);
        }
        else if (handRank == bestHandRank)
        {
            qsort(bestFullHand, NUM_CARDS_IN_HAND, sizeof(Card), compareCards);
            qsort(currentFullHand, NUM_CARDS_IN_HAND, sizeof(Card), compareCards);

            for (int j = 0; j < NUM_CARDS_IN_HAND; j++)
            {
                if (currentFullHand[j].rankValue > bestFullHand[j].rankValue)
                {
                    bestPlayer = i;
                    memcpy(bestFullHand, currentFullHand, sizeof(Card) * NUM_CARDS_IN_HAND);
                    break;
                }
                else if (currentFullHand[j].rankValue < bestFullHand[j].rankValue)
                {
                    break;
                }
            }
        }
    }

    printf("Player %d wins with hand rank: %d!\n", bestPlayer + 1, bestHandRank);
    sprintf(message, "Winner is Player %d hand rank: %d\r\n", bestPlayer + 1, bestHandRank);
    teraPrint(message);
    displayWinnerPlayer(players[bestPlayer], bestPlayer + 1, handNames[bestHandRank - 1]);
}

Action show_suggest(Player player, Game game){
  if (evaluateHand(player, game.communityCards) > HIGH_CARD){
    // if can find pair -> raise
	teraPrint("[suggest] player should raise\n");
    return (RAISE);
  }

  else if (evaluateHand(player, game.communityCards) <= HIGH_CARD && player.chips > player.currentBet){
    // if have only one card and get pare -> check
	  teraPrint("[suggest] player should check\n");
    return (CHECK);
  }
  else {
	  teraPrint("[suggest] player should fold\n");
    return (FOLD);
  }
}

void playPokerGame(uint8_t n_players, uint8_t n_bots)
{
    Card deck[52];
    NUM_PLAYERS = n_players + n_bots;
    Player players[NUM_PLAYERS];
    Game game;
    int deckIndex = 0;

    int playGame = 0;

    int amountCard = (NUM_PLAYERS * 2) + 5;
    Card rfdi_deck[amountCard];

    JoystickState current_state = STATE_IDLE;

    JoystickConfig config = {
        .adc_x = &hadc2,
        .adc_y = &hadc1,
        .threshold_increase = 3000,
        .threshold_decrease = 1000,
        .mode_switch_high = 3000,
        .mode_switch_low = 1000
    };
    Joystick_Init(&config);

    initializeGame(&game);
    createDeck(deck);
    shuffleDeck(deck);
    initializePlayers(players, n_players, n_bots);

    // postBlinds(players, &game);

    dealInitialCards(deck, players, &deckIndex);

	for (int i = 0; i < 5; i++) {
		disPlayCommunityCard(deck[i], i + 1);
	}

    // Main game loop
    while (game.state != SHOWDOWN)
    {
    	HAL_Delay(200);
        int activePlayers;
        int startPlayer;

        switch (game.state)
        {
        case PREGAME:
        	while (!playGame) {
        		Joystick_Update(&n_players, &n_bots, &current_state, &playGame);
        	}
        	break;

        case DEALCARD:
        	processRFID(amountCard, rfdi_deck);
        	dealInitialCards(rfdi_deck, players, &deckIndex);
        	break;

        case PREFLOP:
            startPlayer = 0;
            activePlayers = bettingRound(players, &game, startPlayer);
            if (activePlayers > 1)
            {
                dealCommunityCards(rfdi_deck, &game, 3, &deckIndex);
                game.state = FLOP;
            }
            else
            {
                game.state = FINISH;
            }
            break;

        case FLOP:
        case TURN:
        case RIVER:
            startPlayer = 0;
            activePlayers = bettingRound(players, &game, startPlayer);
            if (activePlayers > 1 && game.state != RIVER)
            {
                dealCommunityCards(rfdi_deck, &game, 1, &deckIndex);
                game.state++;
            }
            else
            {
                game.state = FINISH;
            }
            break;

        case FINISH:
        	system("cls");
			printf("\n=== SHOWDOWN ===\n");
			displayGameState(&game);
			displayCommunityCards(&game);

			// Display all player hands
			for (int i = 0; i < NUM_PLAYERS; i++)
			{
				if (!players[i].folded)
				{
					printf("\nPlayer %d's hand: [%s of %s] [%s of %s]\n",
						   i + 1,
						   players[i].hand[0].rank, players[i].hand[0].suit,
						   players[i].hand[1].rank, players[i].hand[1].suit);
				}
			}

			findWinner(players, game.communityCards);

			int winningPlayer = -1;
			int bestHandRank = -1;
			Card bestFullHand[NUM_CARDS_IN_HAND];

			for (int i = 0; i < NUM_PLAYERS; i++)
			{
				if (!players[i].folded)
				{
					int handRank = evaluateHand(players[i], game.communityCards);
					if (handRank > bestHandRank)
					{
						bestHandRank = handRank;
						winningPlayer = i;
						getFullHand(bestFullHand, players[i], game.communityCards);
					}
					else if (handRank == bestHandRank)
					{
						Card currentFullHand[NUM_CARDS_IN_HAND];
						getFullHand(currentFullHand, players[i], game.communityCards);
						qsort(bestFullHand, NUM_CARDS_IN_HAND, sizeof(Card), compareCards);
						qsort(currentFullHand, NUM_CARDS_IN_HAND, sizeof(Card), compareCards);

						for (int j = 0; j < NUM_CARDS_IN_HAND; j++)
						{
							if (currentFullHand[j].rankValue > bestFullHand[j].rankValue)
							{
								winningPlayer = i;
								memcpy(bestFullHand, currentFullHand, sizeof(Card) * NUM_CARDS_IN_HAND);
								break;
							}
							else if (currentFullHand[j].rankValue < bestFullHand[j].rankValue)
							{
								break;
							}
						}
					}
				}
			}

			if (winningPlayer != -1)
			{
				players[winningPlayer].chips += game.pot;
				printf("\nPlayer %d wins the pot of %d chips!\n", winningPlayer + 1, game.pot);
				printf("New chip count for Player %d: %d\n", winningPlayer + 1, players[winningPlayer].chips);
			}

			printf("\nFinal chip counts:\n");
			for (int i = 0; i < NUM_PLAYERS; i++)
			{
				printf("Player %d: %d chips\n", i + 1, players[i].chips);
			}
        	break;

        default:
            break;
        }
    }
}
