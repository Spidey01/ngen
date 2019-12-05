@CLS
@TITLE NGEN Development Command Prompt

git submodule init
git submodule update

SET BOOTSTRAPDIR=bootstrap.msvc
mkdir %BOOTSTRAPDIR%

@REM /GR = rtti; /EHsc = exceptions; /MD = sane C/C++ runtime.
@REM /Zi = .pdb debuggy; /FS use mspdbsrv /Od = no expansion; /RTC1 = runtime error checking
@REM /W4 is like -Wall, sorta.
@REM
@SET NGEN_CXX_STD=17
SET NGEN_FLAGS=/W4 /GR /EHsc /Zi /Ob0 /Od /RTC1 /MD /std:c++%NGEN_CXX_STD% /FS /DWIN32 /D_WINDOWS /Isrc /Ijson\single_include


DEL /Q %BOOTSTRAPDIR%\ngen.exe .\ngen.exe

cl /nologo %NGEN_FLAGS% /Fd%BOOTSTRAPDIR%\ngen.pdb /Fo%BOOTSTRAPDIR%\main.obj /c src\main.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fd%BOOTSTRAPDIR%\ngen.pdb /Fo%BOOTSTRAPDIR%\Statement.obj /c src\Statement.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fd%BOOTSTRAPDIR%\ngen.pdb /Fo%BOOTSTRAPDIR%\Shinobi.obj /c src\Shinobi.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fd%BOOTSTRAPDIR%\ngen.pdb /Fo%BOOTSTRAPDIR%\cxxbase.obj /c src\cxxbase.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fd%BOOTSTRAPDIR%\ngen.pdb /Fo%BOOTSTRAPDIR%\msvc.obj /c src\msvc.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fd%BOOTSTRAPDIR%\ngen.pdb /Fo%BOOTSTRAPDIR%\gcc.obj /c src\gcc.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fd%BOOTSTRAPDIR%\ngen.pdb /Fo%BOOTSTRAPDIR%\javac.obj /c src\javac.cpp
@IF errorlevel 1 goto :eof
cl /nologo %NGEN_FLAGS% /Fd%BOOTSTRAPDIR%\ngen.pdb /Fo%BOOTSTRAPDIR%\path.obj /c src\path.cpp
@IF errorlevel 1 goto :eof

@SET NGEN_OBJ=%BOOTSTRAPDIR%\main.obj %BOOTSTRAPDIR%\Statement.obj %BOOTSTRAPDIR%\Shinobi.obj %BOOTSTRAPDIR%\cxxbase.obj %BOOTSTRAPDIR%\msvc.obj %BOOTSTRAPDIR%\gcc.obj %BOOTSTRAPDIR%\javac.obj %BOOTSTRAPDIR%\path.obj

cl /nologo %NGEN_FLAGS% /Fd%BOOTSTRAPDIR%\ngen.pdb /Fe%BOOTSTRAPDIR%\ngen %NGEN_OBJ%
@IF errorlevel 1 goto :eof

@IF errorlevel 1 goto :eof
%BOOTSTRAPDIR%\ngen.exe
@IF errorlevel 1 goto :eof
ninja
COPY dist\ngen.exe .\ngen.exe /Y /B
