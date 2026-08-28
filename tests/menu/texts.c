/*
 * texts.c includes tools for ingame dialogue, as well as string manip for code.
 * Copyright (c) 2026 phan-my <manhhung.phan at proton.me>.
 *
 * This software is licenced under the terms of BSD-2-Clause.
 * See LICENCE for further information.
 */


/* INCLUDES */

#include <stdarg.h>
#include <stdlib.h>
#include <string.h>


/* FUNCTIONS */

// join twos trings into one string
char *_concat_2(const char *s1, const char *s2)
{
	// https://stackoverflow.com/questions/8465006/ddg#8465083
	//
	// +1 for the null-terminator
	char *result = malloc(strlen(s1) + strlen(s2) + 1);
	// in real code you would check for errors in malloc here
	strcpy(result, s1);
	strcat(result, s2);
	return result;
}

// join multiple strings into one string
char* concat(int count, ...)
{
	// https://stackoverflow.com/posts/11394336/revisions
	va_list ap;
	int i;

	// Find required length to store merged string
	int len = 1; // room for NULL
	va_start(ap, count);
	for(i = 0; i < count ; i++)
		len += strlen(va_arg(ap, char*));
	va_end(ap);

	// Allocate memory to concat strings
	char *merged = calloc(sizeof(char), len);
	int null_pos = 0;

	// Actually concatenate strings
	va_start(ap, count);
	for(i = 0; i < count; i++) {
		char *s = va_arg(ap, char*);
		strcpy(merged+null_pos, s);
		null_pos += strlen(s);
	}
	va_end(ap);

	return merged;
}
