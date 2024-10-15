#include "joystick.h"
#include "ILI9341_STM32_Driver.h"
#include <stdio.h>

static JoystickConfig joystick_config;
static uint32_t previous_x_val = 0;
static uint32_t previous_y_val = 0;

void Joystick_Init(JoystickConfig* config) {
    joystick_config = *config;
}

static void UpdateDisplay(uint8_t n_players, uint8_t n_bots, const char* status_message) {
    char buffer[50];

    sprintf(buffer, "PLAYERS: %d", n_players);
    ILI9341_Draw_Text(buffer, 30, 30, BLACK, 3, WHITE);

    sprintf(buffer, "BOTS: %d", n_bots);
    ILI9341_Draw_Text(buffer, 30, 50, BLACK, 3, WHITE);

    if (status_message) {
        ILI9341_Draw_Text(status_message, 30, 100, BLACK, 3, WHITE);
    }

    ILI9341_Draw_Text("Play Game", 40, 140, WHITE, 3, BLACK);
}

uint32_t Joystick_GetXValue(void) {
    HAL_ADC_Start(joystick_config.adc_x);
    HAL_ADC_PollForConversion(joystick_config.adc_x, HAL_MAX_DELAY);
    return HAL_ADC_GetValue(joystick_config.adc_x);
}

uint32_t Joystick_GetYValue(void) {
    HAL_ADC_Start(joystick_config.adc_y);
    HAL_ADC_PollForConversion(joystick_config.adc_y, HAL_MAX_DELAY);
    return HAL_ADC_GetValue(joystick_config.adc_y);
}

void Joystick_Update(uint8_t* n_players, uint8_t* n_bots, JoystickState* current_state, int *playGame) {
    uint32_t x_val = Joystick_GetXValue();
    uint32_t y_val = Joystick_GetYValue();
    char status_message[50] = {0};

    // Check for mode switching based on y_val
    if (y_val > joystick_config.mode_switch_high) {
        *current_state = STATE_MODIFY_PLAYERS;
        sprintf(status_message, "Switched to Player Mode");
    } else if (y_val < joystick_config.mode_switch_low) {
        *current_state = STATE_MODIFY_BOTS;
        sprintf(status_message, "Switched to Bot Mode");
    }

    // Modify players or bots based on current state
    if (*current_state == STATE_MODIFY_PLAYERS) {
        if (x_val > joystick_config.threshold_increase && (*n_players + *n_bots < 4)) {
            (*n_players)++;
            sprintf(status_message, "Players increased: %d", *n_players);
        } else if (x_val < joystick_config.threshold_decrease && *n_players > 0) {
            (*n_players)--;
            sprintf(status_message, "Players decreased: %d", *n_players);
        }
    } else if (*current_state == STATE_MODIFY_BOTS) {
        if (x_val > joystick_config.threshold_increase && (*n_players + *n_bots < 4)) {
            (*n_bots)++;
            sprintf(status_message, "Bots increased: %d", *n_bots);
        } else if (x_val < joystick_config.threshold_decrease && *n_bots > 0) {
            (*n_bots)--;
            sprintf(status_message, "Bots decreased: %d", *n_bots);
        }
    } else if (*current_state == STATE_PLAYGAME) {

    }

    // Update display
    UpdateDisplay(*n_players, *n_bots, status_message);

    // Update previous values
    previous_x_val = x_val;
    previous_y_val = y_val;

    HAL_Delay(300); // Delay for debouncing
}
