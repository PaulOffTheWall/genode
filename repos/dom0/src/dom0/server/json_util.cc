#include "json_util.h"
#include <string.h>
#include <stdlib.h>
#include <base/printf.h>
#include <jsmn.h>

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
	if (tok->type == JSMN_STRING && (int) strlen(s) == tok->end - tok->start && strncmp(json + tok->start, s, tok->end - tok->start) == 0) {
		return 0;
	}
	return -1;
}

char* jsongetstring(const char *json, jsmntok_t *tok) {
	char* c = (char*) malloc(sizeof(char) * (tok->end - tok->start + 1));
	memcpy(c, json + tok->start, tok->end - tok->start);
	c[tok->end - tok->start] = 0x00;
	return c;
}

int jsonreadstring(const char *json, jsmntok_t *tok, int* pos, const char *s, char** t) {
	if (jsoneq(json, tok + (*pos), s) != 0)
	{
		PINF("\"%s\" != \"%s\"\n", jsongetstring(json, tok + (*pos)), s);
		PINF("JSON ERROR!\n");
		return -1;
	}
	(*pos)++;
	*t = jsongetstring(json, tok + (*pos));
	(*pos)++;
	return 0;
}

int jsonreadint(const char *json, jsmntok_t *tok, int* pos, const char *s, int* t) {
	if (jsoneq(json, tok + (*pos), s) != 0)
	{
		PINF("\"%s\" != \"%s\"\n", jsongetstring(json, tok + (*pos)), s);
		PINF("JSON ERROR!\n");
		return -1;
	}
	(*pos)++;
	char* string_buf = jsongetstring(json, tok + (*pos));
	*t = strtoul(string_buf, NULL, 0);
	free(string_buf);
	(*pos)++;
	return 0;
}