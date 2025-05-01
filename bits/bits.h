#pragma once

/**
 * @brief Returns a specific bit
 * 
 * WORD  : The original value
 * 
 * BIT   : The bit (LSB is 1, MSB is 8 for uint8)
 * 
 * COUNT : The word' size in bits
 */
#define GET_BIT(WORD, BIT, BIT_COUNT) ((WORD << (BIT_COUNT - BIT)) >> (BIT_COUNT - 1))