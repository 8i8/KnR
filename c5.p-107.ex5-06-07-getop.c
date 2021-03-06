/*
 * Exercise 5-6. Rewrite appropriate programs from earlier chapters and
 * exercises with pointers instead of array indexing. Good possibilities
 * include getline (Chapters 1 and 4), atoi, itoa, and their variants (Chapters
 * 2, 3, and 4), reverse (Chapter 3), and strindex and getop (Chapter 4).
 *
 * getop
 */
#include <stdio.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>	/* For atof */
#include <float.h>	/* For DBL_EPSILON, the precision limit of double */
#include <math.h>	/* fabs() the absolute floating point value of input */
			/* fmod() for the remainder of two doubles divided */

#define MAXOP	100
#define NUMBER	'0'	/* A signal that a number was found. */

#define NEG	999
#define SIN	1001
#define COS	1002
#define TAN	1003
#define EXP	1004
#define LOG	1005
#define POW	1006
#define COPY	1007
#define DEL	1008
#define CLEAR	1009
#define PRINT	1010
#define SWAP	1011

int16_t getop(char []);
void push(double);
double pop(void);
void printStack(void);
void swapStack(void);
void duplicate(void);
void emptyStack(void);

int main(void)
{
	int16_t type;
	char s[MAXOP];
	double op2;
	int sign;

	sign = 1;

	while ((type = getop(s)) != EOF) {
		switch (type) {
			case NEG:
				sign = -1;
				break;
			case NUMBER:
				push(atof(s) * sign);
				sign = 1;
				break;
			case '+':
				push(pop() + pop());
				break;
			case '*':
				push(pop() * pop());
				break;
			case '-':
				op2 = pop();
				push(pop() - op2);
				break;
			case '/':
				op2 = pop();
				/* DBL_EPSILON the smallest increment */
				if (fabs(op2 - 0) > DBL_EPSILON)
					push(pop() / op2);
				else
					printf("error: zero divisor\n");
				break;
			case '%':
				op2 = pop();
				if (fabs(op2 - 0) > DBL_EPSILON)
					/* math.h for mod of doubles */
					push(fmod(pop(), op2));
				else
					printf("error: zero modulo\n");
				break;
			case COPY:
			case 'c':
				duplicate();
				break;
			case DEL:
			case 'd':
				pop();
				break;
			case CLEAR:
			case 'e':
				emptyStack();
				break;
			case PRINT:
			case 'p':
				printStack();
				break;
			case SWAP:
			case 's':
				swapStack();
				break;
			case SIN:
				push(sin( pop() ));
				break;
			case COS:
				push(cos( pop() ));
				break;
			case TAN:
				push(tan( pop() ));
				break;
			case EXP:
				push(exp( pop() ));
				break;
			case LOG:
				push(log( pop() ));
				break;
			case '^':
			case POW:
				op2 = pop();
				push(pow( pop(), op2 ));
				break;
			case '\n':
				break;
			case 0:
				break;
			default:
				printf("error: unknown command %s\n", s);
				break;
		}
	}
	return 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Numerical stack operations
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define MAXVAL	100		/* Maximum depth of val stack */

size_t sp = 0;		/* Next free stack position */
double val[MAXVAL];	/* value stack */

/*
 * push: push f onto value stack
 */
void push(double f)
{
	if (sp < MAXVAL) {
		val[sp++] = f;
	}
	else
		printf("error: stack full, can't push %g\n", f);
}

/*
 * pop: pop and return top value from stack.
 */
double pop(void)
{
	if (sp > 0)
		return val[--sp];
	else {
		printf("error: stack empty\n");
		return 0.0L;
	}
}

/*
 * Output the contents of the stack to the terminal.
 */
void printStack(void)
{
	size_t i;

	for (i = 0; i < sp; i++)
		printf("%f\n", val[i]);
}

/*
 * Swap the two top most elements in the stack.
 */
void swapStack(void)
{
	double temp;

	if (sp > 0) {
		temp = val[sp-1];
		val[sp-1] = val[sp-2];
		val[sp-2] = temp;
	}
	else
		printf("error: to few elements on the stack.\n");

}

/*
 * Copy and make a new the top most stack value.
 */
void duplicate(void)
{
	if (sp > 0)
		push(val[sp-1]);
}

/*
 * Empty the stack.
 */
void emptyStack(void)
{
	sp = 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Input
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#include <ctype.h>
#include <string.h>

#define NUMBER	'0'	/* A signal that a number was found. */
#define BUFSIZE	100

char getch(void);
void ungetch(char);
int8_t isToken(void);
int16_t readToken(void);
char tokenBuffer(char c);

/*
 * getop: get next operator or numeric operand.
 */
int16_t getop(char *s)
{
	char c;

	if (isToken())
		return readToken();

	/* keep inputing char until c is neither a space nor a tab */
	while ((*s = c = getch()) == ' ' || c == '\t')
		;

	/* set a default end of string char at 2nd array index */
	*(s+1) = '\0';

	/* send letters to the string buffer to check for tokens */
	if (isalpha(c)) {
		return tokenBuffer(c);
	}

	/* if c is any operator other than '.' or '-' return it */
	if (!isdigit(c) && c != '.' && c != '-')
		return c;

	/*
	 * If c is the '-' sign, check the next char, if a digit or a point,
	 * continue else send char to store and return a minus
	 */
	if (c == '-') {
		if (isdigit(c = getch()) || c == '.') {
			ungetch(c);
			return NEG;
		} else {
			if (c != (char)EOF)
				ungetch(c);
			return '-';
		}
	}

	/* If it is a digit, start counting */
	if (isdigit(c))
		while (isdigit(*++s = c = getch()))
			;

	/* if c is a decimal point, start counting the fractional part */
	if (c == '.')
		while (isdigit(*++s = c = getch()))
			;

	*s = '\0';

	if (c != (char)EOF)
		ungetch(c);

	return NUMBER;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Input buffer
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
char buf[BUFSIZE];	/* buffer for ungetch */
size_t bufp = 0;		/* next free position in buf */

/*
 * Get a (possibly pushed back) character.
 */
char getch(void)
{
	return (bufp > 0) ? buf[--bufp] : (char)getchar();
}

/*
 * Push character back on input.
 */
void ungetch(char c)
{
	if (bufp >= BUFSIZE)
		printf("ungetch: too many characters\n");
	else
		buf[bufp++] = c;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Text token buffer
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
char bufText[BUFSIZE];
size_t bufpT = 0;
int8_t token = 0;

/*
 * Boolean switch showing availability of token.
 */
int8_t isToken(void)
{
	if (token) {
		token = 0;
		return 1;
	}
	return 0;
}

/*
 * Push text onto stack.
 */
void pushChar(char c)
{
	if (bufpT >= BUFSIZE)
		printf("pushChar: text buffer full\n");
	else {
		bufText[bufpT++] = c;
		bufText[bufpT] = '\0';
	}
}

/*
 * Empty text token buffer.
 */
void clearText(void)
{
	bufpT = 0;
	bufText[bufpT] = '\0';
}

/*
 * Read the text buffer.
 */
int16_t readToken(void)
{
	if (!strcmp(bufText, "sin")) {
		clearText();
		return SIN;
	} else if (!strcmp(bufText, "cos")) {
		clearText();
		return COS;
	} else if (!strcmp(bufText, "tan")) {
		clearText();
		return TAN;
	} else if (!strcmp(bufText, "exp")) {
		clearText();
		return EXP;
	} else if (!strcmp(bufText, "log")) {
		clearText();
		return LOG;
	} else if (!strcmp(bufText, "pow")) {
		clearText();
		return POW;
	} else if (!strcmp(bufText, "copy")) {
		clearText();
		return COPY;
	} else if (!strcmp(bufText, "del")) {
		clearText();
		return DEL;
	} else if (!strcmp(bufText, "print")) {
		clearText();
		return PRINT;
	} else if (!strcmp(bufText, "swap")) {
		clearText();
		return SWAP;
	}
	clearText();
	return 0;
}

/*
 * Buffer text input.
 */
char tokenBuffer(char c)
{
	char d;

	if ((d = getch()) == ' ' || d == '\t' || d == '\n') {
		if (bufpT <= 1) {
			return c;
		} else {
			pushChar(tolower(c));
			token = 1;
			return 0;
		}
	} else {
		pushChar(tolower(c));
		if(d != EOF)
			tokenBuffer(d);
	}

	return 0;
}

