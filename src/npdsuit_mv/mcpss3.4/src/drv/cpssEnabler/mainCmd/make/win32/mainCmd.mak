# Microsoft Developer Studio Generated NMAKE File, Based on mainCmd.dsp
!IF "$(CFG)" == ""
CFG=mainCmd - Win32 Debug
!MESSAGE No configuration specified. Defaulting to mainCmd - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "mainCmd - Win32 Release" && "$(CFG)" != "mainCmd - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mainCmd.mak" CFG="mainCmd - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mainCmd - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mainCmd - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mainCmd - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\mainCmd.lib"


CLEAN :
	-@erase "$(INTDIR)\cmdBase.obj"
	-@erase "$(INTDIR)\cmdConsole.obj"
	-@erase "$(INTDIR)\cmdEngine.obj"
	-@erase "$(INTDIR)\cmdMain.obj"
	-@erase "$(INTDIR)\cmdParser.obj"
	-@erase "$(INTDIR)\cmdServer.obj"
	-@erase "$(INTDIR)\cmdStream.obj"
	-@erase "$(INTDIR)\cmdVersion.obj"
	-@erase "$(INTDIR)\cmdWrapUtils.obj"
	-@erase "$(INTDIR)\os_w32IO.obj"
	-@erase "$(INTDIR)\os_w32Mutex.obj"
	-@erase "$(INTDIR)\os_w32Serial.obj"
	-@erase "$(INTDIR)\os_w32Socket.obj"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(OUTDIR)\mainCmd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /ML /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /Fp"$(INTDIR)\mainCmd.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mainCmd.bsc" 
BSC32_SBRS= \
	
LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mainCmd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cmdBase.obj" \
	"$(INTDIR)\cmdVersion.obj" \
	"$(INTDIR)\cmdWrapUtils.obj" \
	"$(INTDIR)\os_w32IO.obj" \
	"$(INTDIR)\os_w32Mutex.obj" \
	"$(INTDIR)\os_w32Serial.obj" \
	"$(INTDIR)\os_w32Socket.obj" \
	"$(INTDIR)\cmdConsole.obj" \
	"$(INTDIR)\cmdEngine.obj" \
	"$(INTDIR)\cmdMain.obj" \
	"$(INTDIR)\cmdParser.obj" \
	"$(INTDIR)\cmdServer.obj" \
	"$(INTDIR)\cmdStream.obj"

"$(OUTDIR)\mainCmd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\mainCmd.lib" "$(OUTDIR)\mainCmd.bsc"


CLEAN :
	-@erase "$(INTDIR)\cmdBase.obj"
	-@erase "$(INTDIR)\cmdBase.sbr"
	-@erase "$(INTDIR)\cmdConsole.obj"
	-@erase "$(INTDIR)\cmdConsole.sbr"
	-@erase "$(INTDIR)\cmdEngine.obj"
	-@erase "$(INTDIR)\cmdEngine.sbr"
	-@erase "$(INTDIR)\cmdMain.obj"
	-@erase "$(INTDIR)\cmdMain.sbr"
	-@erase "$(INTDIR)\cmdParser.obj"
	-@erase "$(INTDIR)\cmdParser.sbr"
	-@erase "$(INTDIR)\cmdServer.obj"
	-@erase "$(INTDIR)\cmdServer.sbr"
	-@erase "$(INTDIR)\cmdStream.obj"
	-@erase "$(INTDIR)\cmdStream.sbr"
	-@erase "$(INTDIR)\cmdVersion.obj"
	-@erase "$(INTDIR)\cmdVersion.sbr"
	-@erase "$(INTDIR)\cmdWrapUtils.obj"
	-@erase "$(INTDIR)\cmdWrapUtils.sbr"
	-@erase "$(INTDIR)\os_w32IO.obj"
	-@erase "$(INTDIR)\os_w32IO.sbr"
	-@erase "$(INTDIR)\os_w32Mutex.obj"
	-@erase "$(INTDIR)\os_w32Mutex.sbr"
	-@erase "$(INTDIR)\os_w32Serial.obj"
	-@erase "$(INTDIR)\os_w32Serial.sbr"
	-@erase "$(INTDIR)\os_w32Socket.obj"
	-@erase "$(INTDIR)\os_w32Socket.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(OUTDIR)\mainCmd.bsc"
	-@erase "$(OUTDIR)\mainCmd.lib"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\mainDrv\h" /I "..\..\..\mainOs\h" /I ".\h" /I "..\..\..\mainExtDrv\h" /I "..\..\h" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR"$(INTDIR)\\" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /GZ /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\mainCmd.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\cmdBase.sbr" \
	"$(INTDIR)\cmdVersion.sbr" \
	"$(INTDIR)\cmdWrapUtils.sbr" \
	"$(INTDIR)\os_w32IO.sbr" \
	"$(INTDIR)\os_w32Mutex.sbr" \
	"$(INTDIR)\os_w32Serial.sbr" \
	"$(INTDIR)\os_w32Socket.sbr" \
	"$(INTDIR)\cmdConsole.sbr" \
	"$(INTDIR)\cmdEngine.sbr" \
	"$(INTDIR)\cmdMain.sbr" \
	"$(INTDIR)\cmdParser.sbr" \
	"$(INTDIR)\cmdServer.sbr" \
	"$(INTDIR)\cmdStream.sbr"

"$(OUTDIR)\mainCmd.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LIB32=link.exe -lib
LIB32_FLAGS=/nologo /out:"$(OUTDIR)\mainCmd.lib" 
LIB32_OBJS= \
	"$(INTDIR)\cmdBase.obj" \
	"$(INTDIR)\cmdVersion.obj" \
	"$(INTDIR)\cmdWrapUtils.obj" \
	"$(INTDIR)\os_w32IO.obj" \
	"$(INTDIR)\os_w32Mutex.obj" \
	"$(INTDIR)\os_w32Serial.obj" \
	"$(INTDIR)\os_w32Socket.obj" \
	"$(INTDIR)\cmdConsole.obj" \
	"$(INTDIR)\cmdEngine.obj" \
	"$(INTDIR)\cmdMain.obj" \
	"$(INTDIR)\cmdParser.obj" \
	"$(INTDIR)\cmdServer.obj" \
	"$(INTDIR)\cmdStream.obj"

"$(OUTDIR)\mainCmd.lib" : "$(OUTDIR)" $(DEF_FILE) $(LIB32_OBJS)
    $(LIB32) @<<
  $(LIB32_FLAGS) $(DEF_FLAGS) $(LIB32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("mainCmd.dep")
!INCLUDE "mainCmd.dep"
!ELSE 
!MESSAGE Warning: cannot find "mainCmd.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "mainCmd - Win32 Release" || "$(CFG)" == "mainCmd - Win32 Debug"
SOURCE=..\..\src\cmdShell\cmdDb\cmdBase.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\cmdBase.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\cmdBase.obj"	"$(INTDIR)\cmdBase.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\common\cmdVersion.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\cmdVersion.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\cmdVersion.obj"	"$(INTDIR)\cmdVersion.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\common\cmdWrapUtils.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\cmdWrapUtils.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\cmdWrapUtils.obj"	"$(INTDIR)\cmdWrapUtils.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\os\win32\os_w32IO.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\os_w32IO.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\os_w32IO.obj"	"$(INTDIR)\os_w32IO.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\os\win32\os_w32Mutex.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\os_w32Mutex.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\os_w32Mutex.obj"	"$(INTDIR)\os_w32Mutex.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\os\win32\os_w32Serial.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\os_w32Serial.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\os_w32Serial.obj"	"$(INTDIR)\os_w32Serial.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\os\win32\os_w32Socket.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\os_w32Socket.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\os_w32Socket.obj"	"$(INTDIR)\os_w32Socket.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\shell\cmdConsole.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\cmdConsole.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\cmdConsole.obj"	"$(INTDIR)\cmdConsole.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\shell\cmdEngine.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\cmdEngine.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\cmdEngine.obj"	"$(INTDIR)\cmdEngine.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\shell\cmdMain.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\cmdMain.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\cmdMain.obj"	"$(INTDIR)\cmdMain.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\shell\cmdParser.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\cmdParser.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\cmdParser.obj"	"$(INTDIR)\cmdParser.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\shell\cmdServer.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\cmdServer.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\cmdServer.obj"	"$(INTDIR)\cmdServer.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 

SOURCE=..\..\src\cmdShell\shell\cmdStream.c

!IF  "$(CFG)" == "mainCmd - Win32 Release"


"$(INTDIR)\cmdStream.obj" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ELSEIF  "$(CFG)" == "mainCmd - Win32 Debug"


"$(INTDIR)\cmdStream.obj"	"$(INTDIR)\cmdStream.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


!ENDIF 


!ENDIF 

