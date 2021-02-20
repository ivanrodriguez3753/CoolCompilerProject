%{
    #include <iostream>
    #include <string>
    #include <sstream>
    #include <climits>


    using namespace std;

    string stringLiteral = "";
    string lexeme = "";
    string comment = "";

    //include driver and parser to get the set of defined tokens
    #include "ParserDriver.hh"
    #include "parser.hh"
%}

 /*some flex states/start conditions for weird stuff like string literals and comments */
%x STRLIT INLINECMT BLOCKCMT

%option noyywrap nounput noinput batch debug yylineno header-file="lexer.hh"

%{
    //utility to convert string denoting an integer into INTEGER token
	yy::parser::symbol_type make_INTEGER(const std::string &s, const yy::parser::location_type& loc);

	yy::parser::symbol_type make_FALSE(const std::string &s, const yy::parser::location_type& loc);
	yy::parser::symbol_type make_TRUE(const std::string &s, const yy::parser::location_type& loc);
%}

%{
    //code run each time a pattern is matched
    #define YY_USER_ACTION loc.columns(yyleng);
%}

%%

%{
    //Just a shortcut to the location held by the driver
    yy::location& loc = drv.location;
    //code run each time yylex is called
    loc.step();
%}


[ \r\t]+            {loc.step();}
\n+                 {loc.lines(yyleng); loc.step();}


 /*keywords are case insensitive, except for true/false which must begin with a lowercase in
 accordance with the rules for object types*/
[cC][lL][aA][sS][sS]				return yy::parser::make_CLASS(loc);
[eE][lL][sS][eE]					return yy::parser::make_ELSE(loc);
f[aA][lL][sS][eE]					return make_FALSE(yytext, loc);
[fF][iI]							return yy::parser::make_FI(loc);
[iI][fF]							return yy::parser::make_IF(loc);
[iI][nN]							return yy::parser::make_IN(loc);
[iI][nN][hH][eE][rR][iI][tT][sS]	return yy::parser::make_INHERITS(loc);
[iI][sS][vV][oO][iI][dD]			return yy::parser::make_ISVOID(loc);
[lL][eE][tT]						return yy::parser::make_LET(loc);
[lL][oO][oO][pP]					return yy::parser::make_LOOP(loc);
[pP][oO][oO][lL]					return yy::parser::make_POOL(loc);
[tT][hH][eE][nN]					return yy::parser::make_THEN(loc);
[wW][hH][iI][lL][eE]				return yy::parser::make_WHILE(loc);
[cC][aA][sS][eE]			        return yy::parser::make_CASE(loc);
[eE][sS][aA][cC]			        return yy::parser::make_ESAC(loc);
[nN][eE][wW]			       	 	return yy::parser::make_NEW(loc);
[oO][fF]			        		return yy::parser::make_OF(loc);
[nN][oO][tT]			        	return yy::parser::make_NOT(loc);
t[rR][uU][eE]			        	return make_TRUE(yytext, loc);
[0-9]+							    return make_INTEGER(yytext, loc);
[A-Z][a-zA-Z0-9_]*					return yy::parser::make_TYPE(yytext, loc);
[a-z][a-zA-Z0-9_]*					return yy::parser::make_IDENTIFIER(yytext, loc);
":"									return yy::parser::make_COLON(loc);
";"									return yy::parser::make_SEMI(loc);
"("									return yy::parser::make_LPAREN(loc);
")"									return yy::parser::make_RPAREN(loc);
"{"									return yy::parser::make_LBRACE(loc);
"}"									return yy::parser::make_RBRACE(loc);
"<-"								return yy::parser::make_LARROW(loc);
"."									return yy::parser::make_DOT(loc);
","									return yy::parser::make_COMMA(loc);
"@"									return yy::parser::make_AT(loc);
"+"									return yy::parser::make_PLUS(loc);
"-"									return yy::parser::make_MINUS(loc);
"*"									return yy::parser::make_TIMES(loc);
"/"									return yy::parser::make_DIVIDE(loc);
"~"									return yy::parser::make_TILDE(loc);
"<"									return yy::parser::make_LT(loc);
"<="								return yy::parser::make_LE(loc);
"=>"                                return yy::parser::make_RARROW(loc);
"="								    return yy::parser::make_EQUALS(loc);
\"									{BEGIN(STRLIT); /*yylval->lexeme = "";*/}
--                                  {BEGIN(INLINECMT);}
\(\*                                {BEGIN(BLOCKCMT);}
<<EOF>>                             return yy::parser::make_YYEOF(loc);

<STRLIT>{
[0-9A-Za-z \.,?!\-\+_=/\\:;\<]*			{
	                                    stringLiteral += string(yytext);}
    
    /*this rule is for typing \n within a string*/
    /*
    \\n 							    {yylval->lexeme += "\\n";
                                        stringLiteral += '\n';}
    \\t                                 {yylval->lexeme += "\\t";
                                        stringLiteral += '\t';}
    */
   	\"								    {
	                                    lexeme = stringLiteral; //need to pass to the tokenizer but also reset it, so introduce "lexeme" as a temp
	                                    stringLiteral = "";
	                                    BEGIN(0);
	                                    return yy::parser::make_STRING(lexeme, loc);
	                                    }
}								

<INLINECMT>{
    \n                                  {BEGIN(0); loc.lines(yyleng);}
    .*                                  ;
}

<BLOCKCMT>{
    [^*\n]*                            {loc.step();} //anything that is not a star
    "*"+[^*\)\n]*                        {loc.step();} //any star that is not followed by )
    \n                                 {loc.lines(yyleng); loc.step();}
    \*\)                                {BEGIN(0);}

}

%%
//keep the rules simple and throw errors in the helper functions that the rules call


yy::parser::symbol_type
make_INTEGER(const std::string& s, const yy::parser::location_type& loc) {
    errno = 0;
    long n = strtol(s.c_str(), NULL, 10);
    if (! (INT_MIN <= n && n <= INT_MAX && errno != ERANGE))
        	throw yy::parser::syntax_error (loc, "integer is out of range: " + s);
    return yy::parser::make_INTEGER ((int) n, loc);
}

yy::parser::symbol_type
make_FALSE(const std::string& s, const yy::parser::location_type& loc) {
    //don't need to convert the string like we did the integer helper
    return yy::parser::make_FALSE(false, loc);
}

yy::parser::symbol_type
make_TRUE(const std::string& s, const yy::parser::location_type& loc) {
    //don't need to convert the string like we did the integer helper
    return yy::parser::make_TRUE(true, loc);
}

void
ParserDriver::scan_begin() {
     yy_flex_debug = trace_scanning;
      if (file.empty () || file == "-")
        yyin = stdin;
      else if (!(yyin = fopen (file.c_str (), "r")))
        {
          std::cerr << "cannot open " << file << ": " << strerror (errno) << '\n';
          exit (EXIT_FAILURE);
        }
}

void
ParserDriver::scan_end ()
{
  fclose (yyin);
}