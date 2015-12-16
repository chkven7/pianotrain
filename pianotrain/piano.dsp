# Microsoft Developer Studio Project File - Name="piano" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=piano - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "piano.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "piano.mak" CFG="piano - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "piano - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "piano - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "piano - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "NDEBUG"
# ADD RSC /l 0xc0a /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386

!ELSEIF  "$(CFG)" == "piano - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GR /GX /ZI /Od /I "C:\Archivos de programa\Microsoft Visual Studio\MyProjects\piano\musicxml\\" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "VC6" /FR /YX /FD /GZ /Zm1000 /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0xc0a /d "_DEBUG"
# ADD RSC /l 0xc0a /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib winmm.lib comctl32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "piano - Win32 Release"
# Name "piano - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\abctopiano.cpp
# End Source File
# Begin Source File

SOURCE=.\pitch\dtw2.cpp
# End Source File
# Begin Source File

SOURCE=.\pitch\dywapitchtrack.cpp
# End Source File
# Begin Source File

SOURCE=.\generator.cpp
# End Source File
# Begin Source File

SOURCE=.\gennote.cpp
# End Source File
# Begin Source File

SOURCE=.\markov.cpp
# End Source File
# Begin Source File

SOURCE=.\midi2abc.cpp
# End Source File
# Begin Source File

SOURCE=.\midifile.cpp
# End Source File
# Begin Source File

SOURCE=.\parseabc.c
# End Source File
# Begin Source File

SOURCE=.\parsexml.cpp
# End Source File
# Begin Source File

SOURCE=.\Piano.cpp
# End Source File
# Begin Source File

SOURCE=.\pianoAbout.cpp
# End Source File
# Begin Source File

SOURCE=.\pianoconf.cpp
# End Source File
# Begin Source File

SOURCE=.\pianoinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\pitch\pitch.cpp
# End Source File
# Begin Source File

SOURCE=.\readxml.cpp
# End Source File
# Begin Source File

SOURCE=.\viterbi.cpp
# End Source File
# Begin Source File

SOURCE=.\pitch\VOICERECOG.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\abc.h
# End Source File
# Begin Source File

SOURCE=.\pitch\dywapitchtrack.h
# End Source File
# Begin Source File

SOURCE=.\midifile.h
# End Source File
# Begin Source File

SOURCE=.\parseabc.h
# End Source File
# Begin Source File

SOURCE=.\piano.h
# End Source File
# Begin Source File

SOURCE=.\pianoabout.h
# End Source File
# Begin Source File

SOURCE=.\pianoconf.h
# End Source File
# Begin Source File

SOURCE=.\pianoinfo.h
# End Source File
# Begin Source File

SOURCE=.\pitch\portaudio.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\lrhand.cur
# End Source File
# Begin Source File

SOURCE=.\pianotrain.bmp
# End Source File
# Begin Source File

SOURCE=.\playy.bmp
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# End Group
# Begin Group "lib"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\pitch\libportaudio.lib
# End Source File
# Begin Source File

SOURCE=.\musicxml\libmusicxml2.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\piano.ico
# End Source File
# Begin Source File

SOURCE=.\Starttic.wav
# End Source File
# End Target
# End Project
