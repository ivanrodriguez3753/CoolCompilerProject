%{
    //#include "Token.cpp"
    #include "Tag.cpp"


    //using namespace std;

    //string stringLiteral = "";
%}

 /*some flex states/start conditions for weird stuff like string literals and comments */
%x STRLIT COMMENT

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
\"[^\n"]+\"							return STRING;

 /* String literal has to be handled carefully, make use of Flex states/start conditions */

 /*
\"									{BEGIN(STRLIT); cout << "read start \"\n"; stringLiteral.clear();}
<STRLIT>{
	[A-Za-z ]*							{cout << "Read some letters\n"; stringLiteral += YYText();}







	\"								{BEGIN(0); 
									char* cstr; 
									strcpy(cstr, stringLiteral.c_str());
									yytext = cstr; 
									cout << "read ending \"\n"; 
									return STRING;}
}								
 */
 
