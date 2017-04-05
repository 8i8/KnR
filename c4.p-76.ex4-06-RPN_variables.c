/*
 * Exercise 4-5. Add access to library functions like sin, exp, and pow. See
 * <math.h> in Appendix B, Section 4.
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
#define VARLIM	26

#define NEG	999
#define ALPHA	1000
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
#define EXIT	1012

/* Numerical stack */
static void push(double);
static double pop(void);
static void printStack(void);
static void swapStack(void);
static void duplicate(void);
static void emptyStack(void);
/* Input, get operation */
static int16_t getop(char []);
/* variable stack */
static void setVarToEmpty(void);
static void pushVar(char c);
static int twoValues(void);

int main(void)
{
	int16_t type;
	char s[MAXOP];
	double op2;
	char error[] = { "error: insufficient parameters" };
	int sign;

	setVarToEmpty();
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
			case ALPHA:
				pushVar(s[0]);
				break;
			case '+':
				if (twoValues())
					push(pop() + pop());
				else 
					printf("%s\n", error);
				break;
			case '*':
				if (twoValues())
					push(pop() * pop());
				else 
					printf("%s\n", error);
				break;
			case '-':
				if (twoValues()) {
					op2 = pop();
					push(pop() - op2);
				} else 
					printf("%s\n", error);
				break;
			case '/':
				if (twoValues()) {
					op2 = pop();
					/* DBL_EPSILON the smallest increment */
					if (fabs(op2) > DBL_EPSILON)
						push(pop() / op2);
					else {
						printf("error: zero divisor\n");
					}
				} else 
					printf("%s\n", error);
				break;
			case '%':
				if (twoValues()) {
					op2 = pop();
					if (fabs(op2) > DBL_EPSILON)
						/* math.h for mod of doubles */
						push(fmod(pop(), op2));
					else {
						printf("error: zero modulo\n");
					}
				} else 
					printf("%s\n", error);
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
				if (twoValues()) {
					op2 = pop();
					push(pow( pop(), op2 ));
				} else 
					printf("%s\n", error);
				break;
			case '\n':
				break;
			case 0:
				break;
			case -2:
				printf("error: unknown token \n");
				break;
			case EXIT:
			case 'q':
				goto exit;
				break;
			default:
				printf("error: unknown command %s\n", s);
				break;
		}
	}
exit:
	return 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Numerical stack
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#define MAXVAL	100		/* Maximum depth of val stack */
#define I_DEPTH	2		/* Depth of index table, type, index */
#define BOOL	2		/* Number of states for boolean values */
#define	SET	1
#define UNSET	0

#define REAL	0
#define VAR	1

#define TYPE	0
#define INDEX	1

static size_t fp = 0;		/* Next free stack position */
static double st_val[MAXVAL];	/* value stack */
static void set_index(int type, int index);

/*
 * push: push float onto numerical stack.
 */
static void push(double f)
{
	if (fp < MAXVAL) {
		st_val[fp] = f;
		set_index(REAL, fp);
		fp++;
	}
	else
		printf("error: Float stack full, can't push %g\n", f);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Var stack
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
static size_t vp = 0;		/* Next free var stack position */
static char st_var[MAXVAL];	/* Var stack */

/*
 * pushVar: push c onto value stack
 */
static void pushVar(char v)
{
	if (vp < MAXVAL) {
		st_var[vp] = v;
		set_index(VAR, vp);
		vp++;
	}
	else
		printf("error: Variable stack full, can't push %c\n", v);
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Var value
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
static double var_values[VARLIM];	/* The variables a-z */
static int var_state[VARLIM];

static void setVar(int i, double value)
{
	var_values[i - 'a'] = value;
	var_state[i - 'a'] = SET;
}

static void setVarToEmpty(void)
{
	size_t i = VARLIM;

	while (--i > 0)
		var_values[i] = '\0';
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Index stack
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

static size_t ip = 0;			/* Next free index position. */
static int st_ind[MAXVAL][I_DEPTH];	/* The stack index, records type. */

/*
 * index: keep track of the stack index and the type of value at that index.
 */
static void set_index(int t, int i)
{
	if (ip < MAXVAL) {
		st_ind[ip][TYPE] = t;
		st_ind[ip][INDEX] = i;
		ip++;
	}
	else
		printf("error: Index stack full, can't push\n");
}

static int twoValues(void)
{
	if (ip >= 2)
		return 1;
	return 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Stack operations
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */

/*
 * pop: pop and return top value from stack.
 */
static double pop(void)
{
	if (ip > 0) {
		if (st_ind[--ip][TYPE] == REAL) {
			fp--;
			return st_val[st_ind[ip][INDEX]];
		}
		else if (st_ind[--ip][TYPE] == VAR) {
			vp--;
			return var_values[st_ind[ip][INDEX]];
		}
		else
			printf("error: pop failed ");
	}
	else {
		printf("error: stack empty\n");
	}
	return 0;
}

/*
 * Output the contents of the stack to the terminal.
 */
static void printStack(void)
{
	size_t i;

	for (i = 0; i < ip; i++) {
		if(st_ind[i][TYPE] == REAL) {
			printf("%f\n", st_val[st_ind[i][INDEX]]);
		} else if (st_ind[i][TYPE] == VAR) {
			puts("VAR");
			printf("%c\n", st_var[st_ind[i][INDEX]]);
		}
	}
}

/*
 * Swap the two top most elements in the stack.
 */
static void swapStack(void)
{
	int i = 0;
	int temp[I_DEPTH];

	if (ip > 0) {
		while (i < I_DEPTH) {
			temp[i] = st_ind[ip-1][i];
			st_ind[ip-1][i] = st_ind[ip-2][i];
			st_ind[ip-2][i] = temp[i];
			i++;
		}
	}
	else
		printf("error: to few elements on the stack.\n");

}

/*
 * Copy and make a new the top most stack value.
 */
static void duplicate(void)
{
	if (ip > 0) {
		if(st_ind[ip][TYPE] == REAL)
			push(st_val[st_ind[ip][INDEX]-1]);
		else if (st_ind[ip][TYPE] == VAR)
			pushVar(st_var[st_ind[ip][INDEX]-1]);
	}
}

/*
 * Empty the stack.
 */
static void emptyStack(void)
{
	ip = 0;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Input
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
#include <ctype.h>
#include <string.h>

#define BUFSIZE	100

static char getch(void);
static void ungetch(char);
static int8_t isToken(void);
static int16_t readToken(void);
static char tokenBuffer(char c);

/*
 * getop: get next operator or numeric operand.
 */
static int16_t getop(char s[])
{
	size_t i;
	char c;

	/* If a token has been set; read it. */
	if (isToken())
		return readToken();

	/* keep inputing char until c is neither a space nor a tab */
	while ((s[0] = c = getch()) == ' ' || c == '\t' || c == '\n')
		;

	/* set a default end of string char at 2nd array ī*/
	s[1] = '\0';

	/*
	 * Send letters to the string buffer, to check for tokens, if true is
	 * returned, there was no token and the letter is a single character
	 * that is then sent for streat ment as a possible variable.
	 */
	if (isalpha(c)) {
		return tokenBuffer(c);
	//	if (tokenBuffer(c))
	//		return tokenBuffer(c); // TODO Put variable code here.
	//	else {
	//		return 0;
	//	}
	}

	/* if c is any operator other than '.' or '-' return it */
	if (!isdigit(c) && c != '.' && c != '-')
		return c;

	/*
	 * If c is the '-' sign, check the next char, if a digit or a point,
	 * continue else send char to store and return a minus
	 */
	i = 0;
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
		while (isdigit(s[++i] = c = getch()))
			;

	/* if c is a decimal point, start counting the fractional part */
	if (c == '.')
		while (isdigit(s[++i] = c = getch()))
			;

	s[i] = '\0';

	if (c != (char)EOF)
		ungetch(c);

	return NUMBER;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Input buffer
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
static char buf[BUFSIZE];		/* buffer for ungetch */
static size_t buf_p = 0;		/* next free position in buf */

/*
 * Get a (possibly pushed back) character.
 */
static char getch(void)
{
	return (buf_p > 0) ? buf[--buf_p] : (char)getchar();
}

/*
 * Push character back on input.
 */
static void ungetch(char c)
{
	if (buf_p >= BUFSIZE)
		printf("ungetch: too many characters\n");
	else
		buf[buf_p++] = c;
}

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *  Text token buffer
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 */
static char bufText[BUFSIZE];
static size_t bufT_p = 0;
static int8_t token = 0;		/* Boolean, is a token set */

/*
 * Boolean switch showing availability of token.
 */
static int8_t isToken(void)
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
static void pushChar(char c)
{
	if (bufT_p >= BUFSIZE)
		printf("pushChar: text buffer full\n");
	else {
		bufText[bufT_p++] = c;
		bufText[bufT_p] = '\0';
	}
}

/*
 * Empty text token buffer.
 */
static void clearText(void)
{
	bufT_p = 0;
	bufText[bufT_p] = '\0';
}

/*
 * Read the text buffer.
 */
static int16_t readToken(void)
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
	} else if (!strcmp(bufText, "exit")) {
		clearText();
		return EXIT;
	}
	clearText();
	return -2;
}

/*
 * Buffer text input.
 */
static char tokenBuffer(char c)
{
	char d;

	if ((d = getch()) == ' ' || d == '\t' || d == '\n') {
		if (bufT_p <= 1) {
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

