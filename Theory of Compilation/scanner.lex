%{
  #include <stdio.h>
  #include "compsrc.h"
  #include "hw3_output.hpp"
  #include "parser.tab.hpp"
  using namespace output;
%}

%option yylineno
%option noyywrap
whitespace		[ \n\r\t]

%%


void								{return VOID	;}
int									{return INT		;}
byte								{return BYTE	;}
bool								{return BOOL    ;}
b									{return B       ;}
set									{return SET     ;}
and									{return AND     ;}
or									{return OR      ;}
not									{return NOT     ;}
true								{return TRUE    ;}
false								{return FALSE   ;}
return								{return RETURN  ;}
if									{return IF      ;}
else								{return ELSE    ;}
while								{return WHILE   ;}
break								{return BREAK   ;}
continue							{return CONTINUE;}
;									{return SC		;}
,									{return COMMA   ;}
\(									{return LPAREN  ;}
\)									{return RPAREN  ;}
\{									{return LBRACE  ;}
\}									{return RBRACE  ;}
\[									{return LBRACKET;}
\]									{return RBRACKET;}
=									{return ASSIGN  ;}
==|!=								{yylval = new Node(yytext);
                                     return EQUALS  ;}
\<|\>|\<=|\>=						{yylval = new Node(yytext);
									 return RELOP   ;}
in									{return IN	    ;}
\+|-								{yylval = new Node(yytext);
									 return ADD     ;}
\*|\/								{yylval = new Node(yytext);
									 return MULT    ;}
\.\.								{return DOTS    ;}
[a-zA-Z][a-zA-Z0-9]*				{yylval = new Node(yytext);
									 return ID      ;}
0|[1-9][0-9]*						{yylval = new Node(yytext);
									 return NUM     ;}
\"([^\n\r\"\\]|\\[rnt\"\\])+\"		{yylval = new Node(yytext);
									 return STRING	;}
\/\/[^\r\n]*[\r|\n|\r\n]? 			;
{whitespace}                        ;
.   								{errorLex(yylineno);exit(0);}
%%