# Microsoft Developer Studio Project File - Name="Timer_Queue_Test" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=Timer_Queue_Test - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE run the tool that generated this project file and specify the
!MESSAGE nmake output type.  You can then use the following command:
!MESSAGE
!MESSAGE NMAKE /f "Timer_Queue_Test.mak".
!MESSAGE
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE
!MESSAGE NMAKE /f "Timer_Queue_Test.mak" CFG="Timer_Queue_Test - Win32 Debug"
!MESSAGE
!MESSAGE Possible choices for configuration are:
!MESSAGE
!MESSAGE "Timer_Queue_Test - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "Timer_Queue_Test - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Timer_Queue_Test - Win32 Release"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release\Timer_Queue_Test"
# PROP Target_Dir ""
# ADD CPP /nologo /O2 /W3 /GX /MD /GR /I ".." /D NDEBUG /D WIN32 /D _CONSOLE /FD /c
# SUBTRACT CPP /YX
# ADD RSC /l 0x409 /d NDEBUG /i ".."
BSC32=bscmake.exe
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD LINK32 advapi32.lib user32.lib /INCREMENTAL:NO Test_Output.lib ACE.lib /libpath:"..\lib" /nologo /version:5.4 /subsystem:console /pdb:"Release\Timer_Queue_Test.pdb"  /machine:I386 /out:"Release\Timer_Queue_Test.exe"

!ELSEIF  "$(CFG)" == "Timer_Queue_Test - Win32 Debug"

# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "."
# PROP Intermediate_Dir "Debug\Timer_Queue_Test"
# PROP Target_Dir ""
# ADD CPP /nologo /Ob0 /W3 /Gm /GX /Zi /MDd /GR /Gy /I ".." /D _DEBUG /D WIN32 /D _CONSOLE /FD /c
# SUBTRACT CPP /YX
# ADD RSC /l 0x409 /d _DEBUG /i ".."
BSC32=bscmake.exe
# ADD BSC32 /nologo 
LINK32=link.exe
# ADD LINK32 advapi32.lib user32.lib /INCREMENTAL:NO Test_Outputd.lib ACEd.lib /libpath:"..\lib" /nologo /version:5.4 /subsystem:console /pdb:".\Timer_Queue_Test.pdb" /debug /machine:I386 /out:".\Timer_Queue_Test.exe"

!ENDIF

# Begin Target

# Name "Timer_Queue_Test - Win32 Release"
# Name "Timer_Queue_Test - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;cxx;c"
# Begin Source File

SOURCE="..\tests\Main.cpp"
# End Source File
# Begin Source File

SOURCE="Timer_Queue_Test.cpp"
# End Source File
# End Group
# End Target
# End Project
