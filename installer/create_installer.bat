:: This script must set QTDIR64 path to the root of corresponding Qt build. Example:
:: set QTDIR64=k:\Qt\5\5.4\msvc2013_64_opengl\

call set_qt_paths.bat

RMDIR /S /Q binaries\

SETLOCAL

if exist "%programfiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" (
    call "%programfiles(x86)%\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64 -vcvars_ver=14.29
) else (
    if exist "%programfiles(x86)%\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" (
        call "%programfiles(x86)%\Microsoft Visual Studio\2019\Enterprise\VC\Auxiliary\Build\vcvarsall.bat" amd64 -vcvars_ver=14.29
    ) else (
        call "%programfiles(x86)%\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" amd64 -vcvars_ver=14.29
    )
)

:: X64
pushd ..\
del .qmake.stash
%QTDIR64%\bin\qmake.exe -tp vc -r
popd

msbuild ../ShaderPlayground.sln /t:Build /p:Configuration=Release;Platform="x64";PlatformToolset=v142
if not %errorlevel% == 0 goto build_fail

xcopy /R /Y ..\bin\release\x64\*.exe binaries\64\

SETLOCAL
SET PATH=%QTDIR64%\bin\
%QTDIR64%\bin\windeployqt.exe --dir binaries\64\Qt --force --release --no-compiler-runtime --no-angle --no-translations binaries\64\ShaderPlayground.exe
ENDLOCAL

xcopy /R /Y %SystemRoot%\System32\msvcp140.dll binaries\64\msvcr\
xcopy /R /Y %SystemRoot%\System32\msvcp140_1.dll binaries\64\msvcr\
xcopy /R /Y %SystemRoot%\System32\vcruntime140.dll binaries\64\msvcr\
xcopy /R /Y %SystemRoot%\System32\vcruntime140_1.dll binaries\64\msvcr\

if not defined WIN_SDK (
	if exist "%programfiles(x86)%\Windows Kits\10\Redist\10.0.19041.0" (
		set WIN_SDK=10.0.19041.0
	) else (
		set WIN_SDK=10.0.18362.0
	)
)
xcopy /R /Y "%programfiles(x86)%\Windows Kits\10\Redist\%WIN_SDK%\ucrt\DLLs\x64\*" binaries\64\msvcr\
if %ERRORLEVEL% GEQ 1 goto windows_sdk_not_found

del binaries\64\Qt\opengl*.*

"c:\Program Files (x86)\Inno Setup 6\iscc" setup.iss

ENDLOCAL
exit /b 0

:build_fail
ENDLOCAL
echo Build failed
pause
exit /b 1

:windows_sdk_not_found
ENDLOCAL
echo Windows SDK not found (required for CRT DLLs)
pause
exit /b 1