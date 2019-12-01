@ECHO Visual Studio setup

@REM Default path to VS 2015 vcvarsall.bat file.
@SET TRY_VC2015=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC\vcvarsall.bat
@REM
@REM Default path to VS 2019 vcvarsall.bat file.
@SET TRY_VC2019=C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat

IF NOT DEFINED NGEN_MSC (
	SET "NGEN_MSC=%TRY_VC2019%"
	CALL "%TRY_VC2019%" x64
)

