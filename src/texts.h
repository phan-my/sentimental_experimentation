#pragma once
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

// join multiple strings into one string
// https://stackoverflow.com/questions/8465006/ddg#8465083
/*
char *concat(int argc, ...)
{
	
	int i;
	int sum = 0;
	
	va_list argv;
	va_start(argv, argc); 
	// get the sum of all strings
	for (i = 0; i < argc; i++)
		sum += strlen(va_arg(argv, char*));
	
	sum++; // +1 for the null-terminator

	char *result = malloc(sum);
	// in real code you would check for errors in malloc here

	strcpy(result, va_arg(argv, char*));
	for (i = 1; i < argc; i++)
		strcat(result, va_arg(argv, char*));
	va_end(argv);

	return result;
}
*/