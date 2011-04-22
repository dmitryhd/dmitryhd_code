/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.
   
   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */


/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     STRING = 259,
     TEST = 260,
     PARENT = 261,
     ANYPARENT = 262,
     PATHMATCH = 263,
     TRUE = 264,
     FALSE = 265,
     FILE_ID = 266,
     PARENT_ID = 267,
     ISDIR = 268,
     RES_TYPE = 269,
     MINSIZE = 270,
     MAXSIZE = 271,
     HAS_RES_TYPE = 272,
     SUBSTR = 273,
     ERRSTR = 274,
     REGEXP = 275,
     ALL = 276,
     HOST_ID = 277,
     ONLINE = 278,
     WKG = 279,
     PROTO = 280,
     SORTORDER = 281,
     REVERSE = 282,
     HOSTPRIO = 283,
     HOSTSTATUS = 284,
     HOSTWKG = 285,
     HOSTNAME = 286,
     FILESIZE = 287,
     FILEDIR = 288,
     FILENAME = 289,
     FILENAMECASE = 290,
     FILEPATH = 291,
     OR = 292,
     AND = 293,
     NOT = 294
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
{

/* Line 1685 of yacc.c  */
#line 90 "qlang.ypp"

	long long m_integer;			/* integer value */
	char *m_string;			/* string */
	CHintedTest m_test;		/* file test */
	CHostTest *m_host_test;		/* host test */
	CItemCompareFunc *m_compare;	/* sort order */
	struct TestListItem *m_test_list;	/* linked list of tests */



/* Line 1685 of yacc.c  */
#line 101 "qlang.tab.hpp"
} YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif

extern YYSTYPE yylval;


