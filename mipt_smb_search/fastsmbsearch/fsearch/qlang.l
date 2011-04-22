%option nounput
%{
/* This file is part of Fast SMB Search
 *
 * Copyright 2001-2005   Alexander Vodomerov
 *
 * $Id: qlang.l,v 1.11 2005/09/11 12:36:36 shurikz Exp $
 *
 * Fast SMB Search is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * Fast SMB Search is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 */

#include <cstring>
#include "record.h"
#include "test.h"
#include "cacheitem.h"
#include "memalloc.h"
#include "qlang.tab.hpp"

void yyerror(char *);

static YY_BUFFER_STATE buffer_state;
static CSimpleAllocator *zz_allocator = 0;
%}

%%

\"[^"]+\" {
	yylval.m_string = (char *)zz_allocator->alloc(yyleng - 1);
	memcpy(yylval.m_string, yytext + 1, yyleng - 2);
	yylval.m_string[yyleng - 2] = 0; /* cut trailing quotes */
	return STRING;
	}

[0-9]+	{
	yylval.m_integer = atoll(yytext);
	return INTEGER;
	}
	
[(),]	{
	return *yytext;
	}

OR	return OR;

AND	return AND;

NOT	return NOT;

PARENT  return PARENT;
ANYPARENT  return ANYPARENT;
PATHMATCH return PATHMATCH;

PARENT_ID  return PARENT_ID;
FILE_ID return FILE_ID;
ISDIR	return ISDIR;
RES_TYPE	return RES_TYPE;
HAS_RES_TYPE	return HAS_RES_TYPE;
MINSIZE	return MINSIZE;
MAXSIZE	return MAXSIZE;
SUBSTR	return SUBSTR;
ERRSTR  return ERRSTR;
REGEXP	return REGEXP;
ALL	return ALL;
HOST_ID	return HOST_ID;
ONLINE	return ONLINE;
WKG	return WKG;
PROTO	return PROTO;
TRUE	return TRUE;
FALSE	return FALSE;
SORTORDER	return SORTORDER;
HOSTPRIO	return HOSTPRIO;
HOSTSTATUS	return HOSTSTATUS;
HOSTWKG		return HOSTWKG;
HOSTNAME	return HOSTNAME;
FILESIZE	return FILESIZE;
FILEDIR		return FILEDIR;
FILENAME	return FILENAME;
FILENAMECASE	return FILENAMECASE;
FILEPATH	return FILEPATH;
REVERSE		return REVERSE;

[ \t\n]+	; /* ignore whitespace */

.	yyerror("Unknown character");

%%

void zz_prepare_parsing(const char *string, CSimpleAllocator* allocator)
{
	buffer_state = yy_scan_string(string);
	zz_allocator = allocator;
}

void zz_finish_parsing()
{
	zz_allocator = 0;
	yy_delete_buffer(buffer_state);
}

void yyerror(char *s)
{
	printf("Parsing error: %s\n", s);
}

int yywrap(void)
{
	return 1;
}
