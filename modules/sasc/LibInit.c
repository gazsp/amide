/*
**      $VER: LibInit.c 37.32 (2.3.99)
**
**      Library initializers and functions to be called by StartUp.c
**
**      (C) Copyright 1996-99 Andreas R. Kleinert
**      All Rights Reserved.
**
**      Modifications for the AmIDE_API v1.0
**      (C) Copyright 2001 Jens Langner
**      All Rights Reserved.
*/

#define __USE_SYSBASE        // perhaps only recognized by SAS/C

#include <exec/types.h>
#include <exec/memory.h>
#include <exec/libraries.h>
#include <exec/execbase.h>
#include <exec/resident.h>
#include <exec/initializers.h>

#ifdef __MAXON__
#include <clib/exec_protos.h>
#else
#include <proto/exec.h>
#endif
#include "compiler.h"
#include "sasc.module.h"

#include <amide/amide_api_base.h>

ULONG __saveds __stdargs L_OpenLibs(struct AmIDE_API_Base *API_Base);
void  __saveds __stdargs L_CloseLibs(void);

struct ExecBase      *SysBase       = NULL;
struct IntuitionBase *IntuitionBase = NULL;
struct GfxBase       *GfxBase       = NULL;
struct UtilityBase   *UtilityBase   = NULL;
struct DOSBase       *DOSBase       = NULL;

#define VERSION  0
#define REVISION 1

char __aligned LibName [] = LIBNAME;
char __aligned LibID   [] = LIBNAME LIBVER;
char __aligned Copyright [] = "(C)opyright 2001 by Jens Langner. All rights reserved.";

char __aligned VERSTRING [] = "\0$VER: " LIBNAME LIBVER;

/* ----------------------------------------------------------------------------------------
   ! ROMTag and Library inilitalization structure:
   !
   ! Below you find the ROMTag, which is the most important "magic" part of a library
   ! (as for any other resident module). You should not need to modify any of the
   ! structures directly, since all the data is referenced from constants from somewhere else.
   !
   ! You may place the ROMTag directly after the LibStart (-> StartUp.c) function as well.
   !
   ! Note, that the data initialization structure may be somewhat redundant - it's
   ! for demonstration purposes.
   !
   ! EndResident can be placed somewhere else - but it must follow the ROMTag and
   ! it must not be placed in a different SECTION.
   ---------------------------------------------------------------------------------------- */

extern ULONG InitTab[];
extern APTR EndResident; /* below */

struct Resident __aligned ROMTag =     /* do not change */
{
 RTC_MATCHWORD,
 &ROMTag,
 &EndResident,
 RTF_AUTOINIT,
 VERSION,
 NT_LIBRARY,
 0,
 &LibName[0],
 &LibID[0],
 &InitTab[0]
};

APTR EndResident;

struct MyDataInit                      /* do not change */
{
 UWORD ln_Type_Init;      UWORD ln_Type_Offset;      UWORD ln_Type_Content;
 UBYTE ln_Name_Init;      UBYTE ln_Name_Offset;      ULONG ln_Name_Content;
 UWORD lib_Flags_Init;    UWORD lib_Flags_Offset;    UWORD lib_Flags_Content;
 UWORD lib_Version_Init;  UWORD lib_Version_Offset;  UWORD lib_Version_Content;
 UWORD lib_Revision_Init; UWORD lib_Revision_Offset; UWORD lib_Revision_Content;
 UBYTE lib_IdString_Init; UBYTE lib_IdString_Offset; ULONG lib_IdString_Content;
 ULONG ENDMARK;
} DataTab =
#ifdef VBCC
{
        0xe000,8,NT_LIBRARY,
        0x0080,10,(ULONG) &LibName[0],
        0xe000,LIBF_SUMUSED|LIBF_CHANGED,
        0xd000,20,VERSION,
        0xd000,22,REVISION,
        0x80,24,(ULONG) &LibID[0],
        (ULONG) 0
};
#else
{
 INITBYTE(OFFSET(Node,         ln_Type),      NT_LIBRARY),
 0x80, (UBYTE) OFFSET(Node,    ln_Name),      (ULONG) &LibName[0],
 INITBYTE(OFFSET(Library,      lib_Flags),    LIBF_SUMUSED|LIBF_CHANGED),
 INITWORD(OFFSET(Library,      lib_Version),  VERSION),
 INITWORD(OFFSET(Library,      lib_Revision), REVISION),
 0x80, (UBYTE) OFFSET(Library, lib_IdString), (ULONG) &LibID[0],
 (ULONG) 0
};
#endif


/* ----------------------------------------------------------------------------------------
   ! L_OpenLibs:
   !
   ! Since this one is called by InitLib, libraries not shareable between Processes or
   ! libraries messing with RamLib (deadlock and crash) may not be opened here.
   !
   ! You may bypass this by calling this function fromout LibOpen, but then you will
   ! have to a) protect it by a semaphore and b) make sure, that libraries are only
   ! opened once (when using global library bases).
   ---------------------------------------------------------------------------------------- */

ULONG __saveds __stdargs L_OpenLibs(struct AmIDE_API_Base *API_Base)
{
 SysBase = (*((struct ExecBase **) 4));

 IntuitionBase = (struct IntuitionBase *) OpenLibrary("intuition.library", 37);
 if(!IntuitionBase) return(FALSE);

 GfxBase = (struct GfxBase *) OpenLibrary("graphics.library", 37);
 if(!GfxBase) return(FALSE);

 UtilityBase = (struct UtilityBase *) OpenLibrary("utility.library", 37);
 if(!UtilityBase) return(FALSE);

 DOSBase = (struct DOSBase *) OpenLibrary("dos.library", 37);
 if(!DOSBase) return(FALSE);

 // Creation of the Class
 if( cl = MakeClass( NULL,  ROOTCLASS, NULL, sizeof(struct MODS_InstData), 0))
 {
    /* Fill in the callback hook */
    cl->cl_Dispatcher.h_Entry = (ULONG (*) ())MODULE_Dispatcher;
    cl->cl_UserData = NULL;
 }

 API_Base->exb_IntuitionBase = IntuitionBase;

 API_Base->exb_GfxBase       = GfxBase;
 API_Base->exb_SysBase       = SysBase;

 return(TRUE);
}

/* ----------------------------------------------------------------------------------------
   ! L_CloseLibs:
   !
   ! This one by default is called by ExpungeLib, which only can take place once
   ! and thus per definition is single-threaded.
   !
   ! When calling this fromout LibClose instead, you will have to protect it by a
   ! semaphore, since you don't know whether a given CloseLibrary(foobase) may cause a Wait().
   ! Additionally, there should be protection, that a library won't be closed twice.
   ---------------------------------------------------------------------------------------- */

void __saveds __stdargs L_CloseLibs(void)
{
 if( cl )          FreeClass(cl);
 if(DOSBase)       CloseLibrary((struct Library *) DOSBase);
 if(UtilityBase)   CloseLibrary((struct Library *) UtilityBase);
 if(GfxBase)       CloseLibrary((struct Library *) GfxBase);
 if(IntuitionBase) CloseLibrary((struct Library *) IntuitionBase);
}
