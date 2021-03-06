/***************************************************************************

 AmIDE - Amiga Integrated Development Environment

 Copyright (C) 1998-2002 by Jens Langner <Jens.Langner@light-speed.de>

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 AmIDE Official Support Site :  http://amide.light-speed.de/
 AmIDE OpenSource project    :  http://sourceforge.net/projects/amigaIDE/

 $Id$

***************************************************************************/

#ifndef _COMPCLIENT_H_
#define _COMPCLIENT_H_

#define MSG_CC_JOBREQUEST		0
#define MSG_CC_JOBFINISHED	1

// Strukturen
struct CompAmIDE_Message
{
  struct Message msg;
  Object *OB_Project;
	ULONG	data;
};

struct AmIDEComp_Message
{
  Object *OB_Project;
  Object *OB_Compiler;
};

// Prototypen
extern BOOL Comp_Client(void);
extern ULONG SafeSend(ULONG, struct CompAmIDE_Message *, STRPTR);

#endif
