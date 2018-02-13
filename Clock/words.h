#ifndef WORDS_H
#define WORDS_H

typedef struct clockFace{
    uint8_t y;
    uint8_t x;
    uint8_t len;
} clockFace;

#define LANG_DUTCH

#ifdef LANG_DUTCH
const clockFace w_IT       = {0, 9, 3};       //HET
const clockFace w_IS       = {4, 9, 2};       //IS
const clockFace m_FIVE     = {7, 9, 4};       //VIJF
const clockFace m_TEN      = {0, 8, 4};       //TIEN
const clockFace m_QUARTER  = {6, 7, 5};       //KWART
const clockFace s_TO       = {7, 8, 4};       //VOOR
const clockFace s_PAST     = {0, 7, 4};       //OVER
const clockFace s_HALF     = {0, 6, 4};       //HALF
const clockFace h_PAST     = {7, 6, 4};       //OVER
const clockFace h_TO       = {0, 5, 4};       //VOOR
const clockFace w_OCLOCK   = {8, 0, 3};       //UUR

const clockFace h_ONE      = {7, 5, 3};       //ÉÉN
const clockFace h_TWO      = {0, 4, 4};       //TWEE
const clockFace h_THREE    = {7, 4, 4};       //DRIE
const clockFace h_FOUR     = {0, 3, 4};       //VIER
const clockFace h_FIVE     = {4, 3, 4};       //VIJF
const clockFace h_SIX      = {8, 3, 3};       //ZES
const clockFace h_SEVEN    = {0, 2, 5};       //ZEVEN
const clockFace h_EIGHT    = {0, 1, 4};       //ACHT
const clockFace h_NINE     = {6, 2, 5};       //NEGEN
const clockFace h_TEN      = {4, 1, 4};       //TIEN
const clockFace h_ELEVEN   = {8, 1, 3};       //ELF
const clockFace h_TWELVE   = {0, 0, 6};       //TWAALF
#endif //LANG_DUTCH

const clockFace NUMBERS[] = {
	h_ONE,
	h_TWO,
	h_THREE,
	h_FOUR,
	h_FIVE,
	h_SIX,
	h_SEVEN,
	h_EIGHT,
	h_NINE,
	h_TEN,
	h_ELEVEN,
	h_TWELVE
};

#endif //WORDS_H
