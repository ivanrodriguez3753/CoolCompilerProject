%{
    //#include "Token.cpp"
    #include "Tag.h"
    #include <iostream>
    #include <string>

    extern YYLVAL_T* yylval;
    extern char* yytext;

    using namespace std;

    string stringLiteral = "";
    string lexeme = "";
    string comment = "";
%}

 /*some flex states/start conditions for weird stuff like string literals and comments */
%x STRLIT INLINECMT BLOCKCMT

%option noyywrap yylineno header-file="lex.yy.h"
%%
[ \n\t]+            {return 999;/*do nothing, yylineno is taking care of lineno*/}

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
[0-9]+							    {lexeme = string(yytext);
                                    yylval->lexeme = lexeme; return INTEGER;}
[A-Z][a-zA-Z0-9_]*					{lexeme = string(yytext);
                                    yylval->lexeme = lexeme; return TYPE;}
[a-z][a-zA-Z0-9_]*					{lexeme = string(yytext);
                                    yylval->lexeme = lexeme; return IDENTIFIER;}
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
"~"									return TILDE;
"<"									return LT;
"<="								return LE;
"=>"                                return RARROW;
"="								    return EQUALS;
\"									{BEGIN(STRLIT); yylval->lexeme = "";}
--                                  {BEGIN(INLINECMT);}
\(\*                                {BEGIN(BLOCKCMT);}

<STRLIT>{
	[0-9A-Za-z \.,?!\-\+_=/\\:;]*					{yylval->lexeme = string(yytext);
	                                    stringLiteral += string(yytext);}
    
    /*this rule is for typing \n within a string*/
    /*
    \\n 							    {yylval->lexeme += "\\n";
                                        stringLiteral += '\n';}
    \\t                                 {yylval->lexeme += "\\t";
                                        stringLiteral += '\t';}
    */
	\"								    {
	                                    //yylval.lexeme += "";
	                                    stringLiteral = "";
	                                    BEGIN(0);
	                                    return STRING;
	                                    }
}								

<INLINECMT>{
    \n                                  BEGIN(0);
    .*                                  ;
}

<BLOCKCMT>{
    \*\)                                {BEGIN(0);}
    [\n\t ]                             {}
    ^[\*\)]                             {/*yylval->lexeme =*/ "";}
}
