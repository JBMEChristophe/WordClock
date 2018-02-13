
struct word* word_init(struct word* lut) {
	lut = (struct word*)malloc(sizeof(struct word));
	return lut;
}

uint8_t word_add(struct word* lut, const char* text, const size_t size) {
	struct word* header = lut;

	if (size > ROWLENGTH) 				{ return OUT_OF_BOUNDS_ERROR; }
	if (word_get(lut, text) == NULL) 	{ return ALREADY_PRESENT_ERROR; }
	if (lut == NULL) 					{ return NOT_INITIALISED_ERROR;	}

	lut->next = malloc(sizeof(struct word));
	lut = lut->next;
	lut = (word){
		.word = strcpy(lut->word, text);
		.pos = (uint16_t)memcpy(lut->pos, pos);
		.state = OFF;
		.colours = {0};
	}

	lut = header;
	return NO_ERROR;
}

struct word* word_get(struct word* lut, const char* text) {
	struct word* header = lut;
	while(lut != NULL && !strcmp(lut->word, text)) {
		lut = lut->next;
	}
	return lut
}