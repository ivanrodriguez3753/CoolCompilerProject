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
%option reentrant

%%



[ \r\t]+            {}
\n+                 {}


[cC][lL][aA][sS][sS]				return CLASS;
[eE][lL][sS][eE]					return 0;
f[aA][lL][sS][eE]					return 0;
[fF][iI]							return 0;
[iI][fF]							return 0;
[iI][nN]							return 0;
[iI][nN][hH][eE][rR][iI][tT][sS]	return 0;
[iI][sS][vV][oO][iI][dD]			return 0;
[lL][eE][tT]						return 0;
[lL][oO][oO][pP]					return 0;
[pP][oO][oO][lL]					return 0;
[tT][hH][eE][nN]					return 0;
[wW][hH][iI][lL][eE]				return 0;
[cC][aA][sS][eE]			        return 0;
[eE][sS][aA][cC]			        return 0;
[nN][eE][wW]			       	 	return 0;
[oO][fF]			        		return 0;
[nN][oO][tT]			        	return 0;
t[rR][uU][eE]			        	return 0;
[0-9]+							    return 0;
[A-Z][a-zA-Z0-9_]*					return TYPE;
[a-z][a-zA-Z0-9_]*					return 0;
":"									return 0;
";"									return SEMI;
"("									return 0;
")"									return 0;
"{"									return LBRACE;
"}"									return RBRACE;
"<-"								return 0;
"."									return 0;
","									return 0;
"@"									return 0;
"+"									return 0;
"-"									return 0;
"*"									return 0;
"/"									return 0;
"~"									return 0;
"<"									return 0;
"<="								return 0;
"=>"                                return 0;
"="								    return 0;
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
                                        stringLiteral = "";
                                        BEGIN(0);
                                        return 0;
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
