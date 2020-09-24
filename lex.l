%{
    //#include "Token.cpp"
    #include "Tag.cpp"
    #include <iostream>

    using namespace std;

    string stringLiteral = "";
    string comment = "";
%}

 /*some flex states/start conditions for weird stuff like string literals and comments */
%x STRLIT INLINECMT BLOCKCMT

%option noyywrap yylineno header-file="lex.yy.h"
%%
[ \n\t]+            {/*do nothing, yylineno is taking care of lineno*/}

 /*keywords are case insensitive, except for true/false which must begin with a lowercase in
 accordance with the rules for object types*/
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
[0-9]+							    return INTEGER;
[A-Z][a-zA-Z0-9_]*					return TYPEID;
[a-z][a-zA-Z0-9_]*					return OBJECTID;
":"									return COLON;
";"									return SEMICOLON;
"("									return LPAREN;
")"									return RPAREN;
"["									return LBRACKET;
"]"									return RBRACKET;
"{"									return LCURLY;
"}"									return RCURLY;
"<-"								return ASSIGNMENT;
"."									return DOT;
","									return COMMA;
"@"									return AT;
"+"									return PLUS;
"-"									return MINUS;
"*"									return TIMES;
"/"									return DIVIDE;
"~"									return TILDA;
"<"									return LT;
"<="								return LTE; 
"="									return EQ;
\"									{BEGIN(STRLIT); cout << "Entering <STRLIT> state\n";}
--                                  {BEGIN(INLINECMT);}
\(\*                                {BEGIN(BLOCKCMT);}

<STRLIT>{
	[A-Za-z \.,?!]*						{stringLiteral += yytext;}
    
    /*this rule is for typing \n within a string*/
    \\n 							    {stringLiteral += '\n';}

    /*rule for multiline strings. first we see a \ literal then an invisible newline. Net effect is adding a newline*/
    \\\n                                {stringLiteral += '\n';}

	\"								    {
	                                    stringLiteral = "";
	                                    BEGIN(0);
	                                    }
}								

<INLINECMT>{
    \n                                  BEGIN(0);
    .*                                  ;
}

<BLOCKCMT>{
    \*\)                                {BEGIN(0);}
    ^[\*\)]                             {}
    \n                                  {}
}
