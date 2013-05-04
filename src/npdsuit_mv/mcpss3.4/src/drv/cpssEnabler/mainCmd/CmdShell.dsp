# Microsoft Developer Studio Project File - Name="CmdShell" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=CmdShell - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "CmdShell.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "CmdShell.mak" CFG="CmdShell - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CmdShell - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "CmdShell - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CmdShell - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "CmdShell - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "v:\Galileo\Prestera\mainOs\h" /I "v:\Galileo\Prestera\mainCmd\h" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /D "FIELD_PROMPT" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib wsock32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "CmdShell - Win32 Release"
# Name "CmdShell - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Group "OSWrappers"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\cmdShell\os\win32\os_w32IO.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdShell\os\win32\os_w32Mutex.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdShell\os\win32\os_w32Serial.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdShell\os\win32\os_w32Socket.c
# End Source File
# Begin Source File

SOURCE=..\mainOs\src\gtOs\win32\osWin32Intr.c
# End Source File
# Begin Source File

SOURCE=..\mainOs\src\gtOs\win32\osWin32Io.c
# End Source File
# Begin Source File

SOURCE=..\mainOs\src\gtOs\win32\osWin32Mem.c
# End Source File
# Begin Source File

SOURCE=..\mainOs\src\gtOs\win32\osWin32Queue.c
# End Source File
# Begin Source File

SOURCE=..\mainOs\src\gtOs\win32\osWin32Rand.c
# End Source File
# Begin Source File

SOURCE=..\mainOs\src\gtOs\win32\osWin32Sem.c
# End Source File
# Begin Source File

SOURCE=..\mainOs\src\gtOs\win32\osWin32Str.c
# End Source File
# Begin Source File

SOURCE=..\mainOs\src\gtOs\win32\osWin32Task.c
# End Source File
# Begin Source File

SOURCE=..\mainOs\src\gtOs\win32\osWinGen.c
# End Source File
# Begin Source File

SOURCE=..\mainOs\src\gtOs\win32\oxWin32Timer.c
# End Source File
# End Group
# Begin Group "commonSrc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\cmdShell\common\cmdWrapUtils.c
# End Source File
# End Group
# Begin Group "cmdDbSrc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\cmdShell\cmdDb\cmdBase.c
# End Source File
# End Group
# Begin Group "debug"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\src\cmdShell\debug\cmdBaseInit.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdShell\debug\cmdDebug.c
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\cmdShell\shell\cmdConsole.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdShell\shell\cmdEngine.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdShell\shell\cmdMain.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdShell\shell\cmdParser.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdShell\shell\cmdServer.c
# End Source File
# Begin Source File

SOURCE=.\src\cmdShell\shell\cmdStream.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Group "OSWrapper"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=..\..\prestera\mainDrv\h\prestera\os\gtOs.h
# End Source File
# Begin Source File

SOURCE=.\h\cmdShell\os\gtOsIO.h
# End Source File
# Begin Source File

SOURCE=.\h\cmdShell\os\gtOsMutex.h
# End Source File
# Begin Source File

SOURCE=.\h\cmdShell\os\gtOsSerial.h
# End Source File
# Begin Source File

SOURCE=.\h\cmdShell\os\gtOsSocket.h
# End Source File
# Begin Source File

SOURCE=..\..\prestera\mainDrv\h\prestera\os\gtTypes.h
# End Source File
# End Group
# Begin Group "commonInc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\h\cmdShell\common\cmdCommon.h
# End Source File
# Begin Source File

SOURCE=.\h\cmdShell\common\cmdWrapUtils.h
# End Source File
# End Group
# Begin Group "cmdDbInc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\h\cmdShell\cmdDb\cmdBase.h
# End Source File
# End Group
# Begin Group "debugInc"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\h\cmdShell\debug\cmdDebug.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\h\cmdShell\shell\cmdConsole.h
# End Source File
# Begin Source File

SOURCE=.\h\cmdShell\shell\cmdEngine.h
# End Source File
# Begin Source File

SOURCE=.\h\cmdShell\shell\cmdParser.h
# End Source File
# Begin Source File

SOURCE=.\h\cmdShell\shell\cmdServer.h
# End Source File
# Begin Source File

SOURCE=.\h\cmdShell\shell\cmdStream.h
# End Source File
# Begin Source File

SOURCE=.\h\cmdShell\shell\cmdSystem.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
