/*
 * Exercise 1-24. Write a program to check a C program for rudimentary syntax
 * errors like unmatched parentheses, brackets and braces. Don't forget about
 * quotes, both single and double, escape sequences, and comments. (This
 * program is hard if you do it in full generality.)
 *
 * TODO make the code work for ([)] situations, bracket depth or similar
 * required.
 */

#undef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdint.h>

#define MAXLINE		65535

static void checkEscapeChar(char input[], uint8_t status[], size_t);
static void checkComments(char input[], uint8_t status[], uint16_t len, size_t);
static void checkQuotes(char input[], uint8_t status[], uint16_t len, size_t);
static void checkBraces(char input[], uint8_t status[], uint16_t len, size_t);
static void checkBrackets(char input[], uint8_t status[], uint16_t len, size_t);
static uint16_t getline(char string[], uint16_t lim);

int main(void)
{
	char string[MAXLINE];
	uint8_t status[7];
	uint16_t len;
	size_t i;

	while ((len = getline(string, MAXLINE)) > 0)
	{
		/*
			status[0] = brackets;
			status[1] = semiColonRequired;
			status[2] = braces;
			status[3] = singleQuotes;
			status[4] = doubleQuotes;
			status[5] = comments;
			status[6] = escaped;
		*/
		for (i = 0; i < len; i++)
		{
			/*
			 * If the escape character is found and we are in
			 * quotes, then skip the next character.
			 */
			checkComments(string, status, len, i);
			if(status[5])
				continue;

			checkEscapeChar(string, status, i);
			if(status[6]) {
				status[6] = 0;
				i++;
				continue;
			}

			checkQuotes(string, status, len, i);
			if (status[3] || status[4])
				continue;

			checkBraces(string, status, len, i);
			checkBrackets(string, status, len, i);
		}
	}
}

static void checkEscapeChar(char input[], uint8_t status[], size_t i)
{
	if (input[i] == '\\')
		status[6] = 1;
}

static void checkComments(char input[], uint8_t status[], uint16_t len, size_t i)
{
	int c;
	uint8_t prev;
	static uint8_t comments;

	c = input[i];
	if (input[i-1] > 0)
		prev = input[i-1];

	if (c == '*' && prev == '/' && !comments)
		comments = 1;
	else if (c == '/' && prev == '*' && comments)
		comments = 0;

	status[5] = comments;

	if (i == len-2)
	{
		if (comments)
			printf("Your program requires a closing comment marker.\n");
	}
}

static void checkQuotes(char input[], uint8_t status[], uint16_t len, size_t i)
{
	int c;
	static uint8_t singleQuotes;
	static uint8_t doubleQuotes;

	c = input[i];

	if (c == '\'' && !singleQuotes && !doubleQuotes)
		singleQuotes = 1;
	else if (c == '\'' && singleQuotes)
		singleQuotes = 0;

	if (c == '"' && !singleQuotes && !doubleQuotes)
		doubleQuotes = 1;
	else if (c == '"' && doubleQuotes)
		doubleQuotes = 0;

	status[3] = singleQuotes;
	status[4] = doubleQuotes;

	if (i == len-2)
	{
		if (singleQuotes == 1)
		{
			printf("Your program is missing a closing single quote\n");
		}
		else if (doubleQuotes == 1)
		{
			printf("Your program is missing a closing double quote\n");
		}
	}
} 

/*
 * Check program for braces balance, opening and closing.
 */
static void checkBraces(char input[], uint8_t status[], uint16_t len, size_t i)
{
	int c;
	static uint8_t braces = 0;

	c = input[i];

	if (c == '{')
		++braces;
	if (c == '}')
		--braces;

	status[2] = braces;

	if (i == len-2)
	{
		if (braces > 0)
		{
			printf("Your program is missing %d closing braces.\n", braces);
		}
		else if (braces < 0)
		{
			braces = (braces * braces) / braces;
			printf("Your program is missing %d opening braces.\n", braces);
		}
	}
}

/*
 * Check program for bracket balance, opening and closing.
 */
static void checkBrackets(char input[], uint8_t status[], uint16_t len, size_t i)
{
	int c;
	static uint8_t brackets = 0;
	static uint8_t semiColonRequired = 0;

	c = input[i];

	if (c == '(')
	{
		++brackets;
		semiColonRequired = 1;
	}
	else if (c == ')')
		--brackets;

	if (c == ';' && semiColonRequired == 1)
		semiColonRequired = 0;

	status[0] = brackets;
	status[1] = semiColonRequired;

	if (i == len-2)
	{
		if (brackets > 0)
			printf("Your program is missing %d closing brackets.\n", brackets);
		else if (brackets < 0)
		{
			brackets = (brackets * brackets) / brackets;
			printf("Your program is missing %d opening brackets.\n", brackets);
		}

		if (semiColonRequired > 0)
			printf("Error, expected ';'.\n");
	}
}

/*
 * Get input line by line.
 */
static uint16_t getline(char str[], uint16_t lim)
{
	size_t i;
	int c;

	for (i = 0; i < lim-1 && (c = getchar()) != EOF; i++)
		str[i] = c;

	if (c == '\n')
		str[i++] = c;
	str[i] = '\0';
	return i;
}

