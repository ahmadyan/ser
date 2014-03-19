// ****************************************************************************
// ****************************************************************************
// parserFunctions.h
//
// Header file included by the yacc generated parser.
//
// *****************************************************************************


#ifndef __PARSER_H__
#define __PARSER_H__

extern "C"
{
	int yyparse(void);
//	int yylex(void);
//	int _yylex(void);
	int yywrap(void);
	int yyerror(char *str);
}

//extern liberty::Library __y_library;

#endif
