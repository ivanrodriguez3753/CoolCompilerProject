%{
#include <iostream>
#include <string>
#include <sstream>
#include <climits>


using namespace std;

string stringLiteral = "";
string lexeme = "";
string comment = "";

#include "parse.hpp"
%}

/*some flex states/start conditions for weird stuff like string literals and comments */
%x STRLIT INLINECMT BLOCKCMT

%option noyywrap nounput noinput batch debug yylineno
%option reentrant extra-type="const char*"

%%



[ \r\t]+            {}
\n+                 {}


[cC][lL][aA][sS][sS]				return CLASS;
[eE][lL][sS][eE]					return ELSE;
f[aA][lL][sS][eE]					return FALSE;
[fF][iI]							return FI;
[iI][fF]							return IF;
[iI][nN]							return IN;
[iI][nN][hH][eE][rR][iI][tT][sS]	return INHERITS;
[iI][sS][vV][oO][iI][dD]			return ISVOID;
[lL][eE][tT]						return LET;
[lL][oO][oO][pP]					return LOOP;
[pP][oO][oO][lL]					return POOL;
[tT][hH][eE][nN]					return THEN;
[wW][hH][iI][lL][eE]				return WHILE;
[cC][aA][sS][eE]			        return CASE;
[eE][sS][aA][cC]			        return ESAC;
[nN][eE][wW]			       	 	return NEW;
[oO][fF]			        		return OF;
[nN][oO][tT]			        	return NOT;
t[rR][uU][eE]			        	return TRUE;
[0-9]+							    return INT;
[A-Z][a-zA-Z0-9_]*					return TYPE;
[a-z][a-zA-Z0-9_]*					return ID;
":"									return COLON;
";"									return SEMI;
"("									return LPAREN;
")"									return RPAREN;
"{"									return LBRACE;
"}"									return RBRACE;
"<-"								return LARROW;
"."									return DOT;
","									return COMMA;
"@"									return AT;
"+"									return PLUS;
"-"									return MINUS;
"*"									return TIMES;
"/"									return DIVIDE;
"~"									return NEG;
"<"									return LT;
"<="								return LE;
"=>"                                return RARROW;
"="								    return EQUALS;
\"									{BEGIN(STRLIT); }
--                                  {BEGIN(INLINECMT);}
\(\*                                {BEGIN(BLOCKCMT);}
<<EOF>>                             return 0;

<STRLIT>{
[0-9A-Za-z \.,?!\-\+_=/\\:;\<]*			{
stringLiteral += string(yytext);}


\\n 							    {
                                    stringLiteral += '\n';}
\\t                                 {
                                    stringLiteral += '\t';}
\"								    {
                                        lexeme = stringLiteral; //need to pass to the tokenizer but also reset it, so introduce "lexeme" as a temp
                                        yyextra = lexeme.c_str();

                                        stringLiteral = "";
                                        BEGIN(0);
                                        return STR;
                                    }
}

<INLINECMT>{
\n                                  {BEGIN(0); }
.*                                  ;
}

<BLOCKCMT>{
[^*\n]*                            {} //anything that is not a star
"*"+[^*\)\n]*                        {} //any star that is not followed by )
\n                                 {}
\*\)                                {BEGIN(0);}

}

%%
//keep the rules simple and throw errors in the helper functions that the rules call
