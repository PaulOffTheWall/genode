#ifndef _JSON_UTIL_H_
#define _JSON_UTIL_H_

#include <jsmn/jsmn.h>

/* json helper functions */
int jsoneq(const char *json, jsmntok_t *tok, const char *s);
char* jsongetstring(const char *json, jsmntok_t *tok);
int jsonreadstring(const char *json, jsmntok_t *tok, int* pos, const char *s, char** t);
int jsonreadint(const char *json, jsmntok_t *tok, int* pos, const char *s, int* t);

#endif