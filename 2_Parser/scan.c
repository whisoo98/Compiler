/****************************************************/
/* File: scan.c                                     */
/* The scanner implementation for the TINY compiler */
/* Compiler Construction: Principles and Practice   */
/* Kenneth C. Louden                                */
/****************************************************/

#include "globals.h"
#include "util.h"
#include "scan.h"

/* states in scanner DFA */
typedef enum
{
	START,
	INASSIGN,
	INCOMMENT,
	INNUM,
	INID,
	DONE,
	INEQ,
	INLT,
	INGT,
	INNE,
	INOVER,
	INCOMMENT_
} StateType;

/* lexeme of identifier or reserved word */
char tokenString[MAXTOKENLEN + 1];

/* BUFLEN = length of the input buffer for
   source code lines */
#define BUFLEN 256

static char lineBuf[BUFLEN]; /* holds the current line */
static int linepos = 0;		 /* current position in LineBuf */
static int bufsize = 0;		 /* current size of buffer string */
static int EOF_flag = FALSE; /* corrects ungetNextChar behavior on EOF */

/* getNextChar fetches the next non-blank character
   from lineBuf, reading in a new line if lineBuf is
   exhausted */
static int getNextChar(void)
{
	if (!(linepos < bufsize))
	{
		lineno++;
		if (fgets(lineBuf, BUFLEN - 1, source))
		{
			if (EchoSource)
				fprintf(listing, "%4d: %s", lineno, lineBuf);
			bufsize = strlen(lineBuf);
			linepos = 0;
			return lineBuf[linepos++];
		}
		else
		{
			EOF_flag = TRUE;
			return EOF;
		}
	}
	else
		return lineBuf[linepos++];
}

/* ungetNextChar backtracks one character
   in lineBuf */
static void ungetNextChar(void)
{
	if (!EOF_flag)
		linepos--;
}

/* lookup table of reserved words */
static struct
{
	char *str;
	TokenType tok;
} reservedWords[MAXRESERVED] = {
	{"if", IF},
	{"else", ELSE},
	{"while", WHILE},
	{"return", RETURN},
	{"int", INT},
	{"void", VOID},
};

/* lookup an identifier to see if it is a reserved word */
/* uses linear search */
static TokenType reservedLookup(char *s)
{
	int i;
	for (i = 0; i < MAXRESERVED; i++)
		if (!strcmp(s, reservedWords[i].str))
			return reservedWords[i].tok;
	return ID;
}

/****************************************/
/* the primary function of the scanner  */
/****************************************/
/* function getToken returns the
 * next token in source file
 */
TokenType getToken(void)
{ /* index for storing into tokenString */
	int tokenStringIndex = 0;
	/* holds current token to be returned */
	TokenType currentToken;
	/* current state - always begins at START */
	StateType state = START;
	/* flag to indicate save to tokenString */
	int save;
	while (state != DONE)
	{ // state가 done이 되면 토큰을 그만 받고 출력
		int c = getNextChar();
		save = TRUE;
		switch (state)
		{ // id, number, whitespace, other 19symbols
		  // 주석 /**/, /, * 구별 필요
		  // <, <=, >, >= 구별필요
		  // =, == 구별 필요
		case START:
			if (isdigit(c))
				state = INNUM;
			else if (isalpha(c))
				state = INID; //변수는 알파벳과 숫자 모두 가능
			else if (c == '=')
			{
				state = INEQ;
			}
			// else if ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
			else if ((c == ' ') || (c == '\t') || (c == '\n'))
				save = FALSE;
			else if (c == '/')
			{
				int temp = getNextChar();
				if (temp == '*')
				{ // 주석
					save = FALSE;
					state = INCOMMENT;
				}
				else
				{
					ungetNextChar(); // temp 되돌림
					state = DONE;
					save = TRUE;
					currentToken = OVER;
					break;
				}
			}
			else if (c == '!')
			{
				state = INNE;
			}
			else if (c == '<')
			{
				state = INLT;
			}
			else if (c == '>')
			{
				state = INGT;
			}
			else
			{
				state = DONE;
				switch (c)
				{
				case '\0':
					save = FALSE;
					currentToken = ENDFILE;
					break;
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					break;
					/*case '=': // => ==과 = 구분
					  currentToken = EQ;
					  break;*/
				case '+':
					currentToken = PLUS;
					break;
				case '-':
					currentToken = MINUS;
					break;
				case '*': //=> *와 주석 구분
					currentToken = TIMES;
					break;
					/*case '/': // => /과 주석 구분 /*~**
					  currentToken = OVER;
					  break;*/
				case '(':
					currentToken = LPAREN;
					break;
				case ')':
					currentToken = RPAREN;
					break;
				case '{':
					currentToken = LCURLY;
					break;
				case '}':
					currentToken = RCURLY;
					break;
				case '[':
					currentToken = LBRACE;
					break;
				case ']':
					currentToken = RBRACE;
					break;
				case ',':
					currentToken = COMMA;
					break;
				case ';':
					currentToken = SEMI;
					break;
				default:
					currentToken = ERROR;
					break;
				}
			}
			break; // 다음 토큰 읽어오기
		case INGT: // > or >= DONE
			state = DONE;
			if (c == '=')
			{
				currentToken = GE;
			}
			else
			{
				save = FALSE;
				currentToken = GT;
				ungetNextChar();
			}
			break;
		case INLT: // < or <= DONE
			state = DONE;
			if (c == '=')
			{
				currentToken = LE;
			}
			else
			{
				save = FALSE;
				currentToken = LT;
				ungetNextChar();
			}
			break;
		case INEQ: // = or == DONE
			state = DONE;
			if (c == '=')
			{
				currentToken = EQ;
			}
			else
			{
				save = FALSE;
				currentToken = ASSIGN;
				ungetNextChar();
			}
			break;
		case INNE: // != ONLY
			if (c == '=')
			{
				state = DONE;
				currentToken = NE;
			}
			else
			{
				state = DONE;
				save = FALSE;
				ungetNextChar();
				currentToken = ERROR;
			}
			break;
		case INCOMMENT: // 주석
			state = INCOMMENT;
			save = FALSE;
			if (c == EOF || c == '\0')
			{
				state = DONE;
				currentToken = ENDFILE;
			}
			else if (c == '*')
			{
				int temp = getNextChar();
				if (temp == '/')
					state = START;
				else
					ungetNextChar();
			}
			break;
		case INNUM:
			if (!isdigit(c))
			{ /* backup in the input */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = NUM;
			}
			break;
		case INID:
			if (!isalpha(c) && !isdigit(c))
			{ /* backup in the input */
				ungetNextChar();
				save = FALSE;
				state = DONE;
				currentToken = ID; // 예약어와 비교하기 위해서 ID enum 대입
			}
			break;
		case DONE:
		default: /* should never happen */
			fprintf(listing, "Scanner Bug: state= %d\n", state);
			state = DONE;
			currentToken = ERROR;
			break;
		}
		if ((save) && (tokenStringIndex <= MAXTOKENLEN))
			tokenString[tokenStringIndex++] = (char)c;
		if (state == DONE)
		{
			tokenString[tokenStringIndex] = '\0';
			if (currentToken == ID)
				currentToken = reservedLookup(tokenString); // 예약어인지 확인하는 작업
		}
	}
	if (TraceScan)
	{
		fprintf(listing, "\t%d: ", lineno);
		printToken(currentToken, tokenString);
	}
	return currentToken;
} /* end getToken */
