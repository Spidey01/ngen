@CLS
@TITLE NGEN Development Command Prompt

mkdir build
mkdir dist

@REM /GR = rtti; /EHsc = exceptions; /MD = sane C/C++ runtime.
@REM /Zi = .pdb debuggy; /FS use mspdbsrv /Od = no expansion; /RTC1 = runtime error checking
@REM /W4 is like -Wall, sorta.
@REM
@SET NGEN_CXX_STD=17
SET NGEN_FLAGS=/W4 /GR /EHsc /Zi /Ob0 /Od /RTC1 /MD /std:c++%NGEN_CXX_STD% /FS /DWIN32 /D_WINDOWS /Isrc /Ijson\single_include


DEL /Q build\ngen.exe dist\ngen.exe .\ngen.exe

cl /nologo %NGEN_FLAGS% /Fdbuild\ngen.pdb /Fobuild\main.obj /c src\main.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fdbuild\ngen.pdb /Fobuild\Statement.obj /c src\Statement.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fdbuild\ngen.pdb /Fobuild\Shinobi.obj /c src\Shinobi.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fdbuild\ngen.pdb /Fobuild\gcc.obj /c src\gcc.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fdbuild\ngen.pdb /Fobuild\msvc.obj /c src\msvc.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fdbuild\ngen.pdb /Fobuild\path.obj /c src\path.cpp
@IF errorlevel 1 goto :eof

@SET NGEN_OBJ=build\main.obj build\Statement.obj build\Shinobi.obj build\msvc.obj build\gcc.obj build\path.obj

cl /nologo %NGEN_FLAGS% /Fdbuild\ngen.pdb /Febuild\ngen %NGEN_OBJ%
@IF errorlevel 1 goto :eof

COPY build\ngen.exe .\dist\ngen.exe /Y /B
@IF errorlevel 1 goto :eof
COPY .\dist\ngen.exe .\ngen.exe /Y /B
@IF errorlevel 1 goto :eof

@ECHO TESTING
PUSHD examples\c_helloworld\
..\..\ngen
POPD
