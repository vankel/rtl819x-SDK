/*

error2.c

Error handling.

gSOAP XML Web services tools
Copyright (C) 2000-2005, Robert van Engelen, Genivia Inc. All Rights Reserved.
This part of the software is released under one of the following licenses:
GPL, the gSOAP public license, or Genivia's license for commercial use.
--------------------------------------------------------------------------------
gSOAP public license.

The contents of this file are subject to the gSOAP Public License Version 1.3
(the "License"); you may not use this file except in compliance with the
License. You may obtain a copy of the License at
http://www.cs.fsu.edu/~engelen/soaplicense.html
Software distributed under the License is distributed on an "AS IS" basis,
WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
for the specific language governing rights and limitations under the License.

The Initial Developer of the Original Code is Robert A. van Engelen.
Copyright (C) 2000-2004 Robert A. van Engelen, Genivia inc. All Rights Reserved.
--------------------------------------------------------------------------------
GPL license.

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place, Suite 330, Boston, MA 02111-1307 USA

Author contact information:
engelen@genivia.com / engelen@acm.org
--------------------------------------------------------------------------------
A commercial use license is available from Genivia, Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#include "soapcpp2.h"
#include "soapcpp2_yacc.h"

#define	MAXERR 10

extern char yytext[];	/* lexeme found by the lexical analyzer */

static int lexerrno = 0;
static int synerrno = 0;
static int semerrno = 0;
static int semwarno = 0;

char errbuf[1024];	/* to hold error messages */

/*
yyerror - called by parser from an error production with nonterminal `error'
*/
yyerror(char *s)
{	fprintf(stderr, "%s(%d): %s\n", filename, yylineno, s);
}

/*
lexerror - called by lexical analyzer upon failure to recognize a token
*/
lexerror(const char *s)
{	fprintf(stderr, "%s(%d): %s: %s\n", filename, yylineno, s, yytext);
	if (lexerrno++ >= MAXERR)
		execerror("too many syntactic errors, bailing out");
}

/*
synerror - called by a semantic action in the yacc grammar
*/
synerror(const char *s)
{	fprintf(stderr, "%s(%d): Syntax error: %s\n", filename, yylineno-1, s);
	if (synerrno++ >= MAXERR)
		execerror("too many syntactic errors, bailing out");
}

/*
semerror - report semantic error from static checking
*/
semerror(const char *s)
{	fprintf(stderr, "\n%s(%d): **ERROR**: %s\n\n", filename, yylineno, s);
	if (semerrno++ >= MAXERR)
		execerror("too many semantic errors, bailing out");
}

/*
semwarn - report semantic warning from static checking
*/
semwarn(const char *s)
{	fprintf(stderr, "\n**WARNING**: %s (detected at line %d in %s)\n\n", s, yylineno, filename);
	semwarno++;
}

/*
compliancewarn - report compliance warning
*/
compliancewarn(const char *s)
{	fprintf(stderr, "Compliance warning: %s\n", s);
}

/*
typerror - report type error (a semantic error)
*/
typerror(const char *s)
{	fprintf(stderr, "%s(%d): Type error: %s\n", filename, yylineno, s);
	if (semerrno++ >= MAXERR)
		execerror("too many semantic errors, bailing out");
}

/*
execerror - print error message and terminate execution
*/
execerror(const char *s)
{	fprintf(stderr, "Critical error: %s\n", s);
	exit(1);
}

/*
progerror - called when check(expr) failed, i.e. upon programming error
*/
progerror(const char *s, const char *f, int l)
{	fprintf(stderr, "Program failure: %s in file %s line %d\n", s, f, l);
	exit(1);
}

/*
errstat - show error statistics
*/
int errstat()
{	if (!lexerrno && !synerrno && !semerrno)
	{	fprintf(stderr, "\nCompilation successful ");
		if (semwarno)
			fprintf(stderr, "(%d warning%s)\n\n", semwarno, semwarno>1?"s":"");
		else
			fprintf(stderr, "\n\n");
		return 0;
	}
	fprintf(stderr, "\nThere were errors:\n");
	if (lexerrno)
		fprintf(stderr, "%d lexical error%s\n", lexerrno, lexerrno>1?"s":"");
	if (synerrno)
		fprintf(stderr, "%d syntax error%s\n", synerrno, synerrno>1?"s":"");
	if (semerrno)
		fprintf(stderr, "%d semantic error%s\n", semerrno, semerrno>1?"s":"");
	if (semwarno)
		fprintf(stderr, "%d warning%s\n", semwarno, semwarno>1?"s":"");
	fprintf(stderr, "\n");
	return -1;
}
