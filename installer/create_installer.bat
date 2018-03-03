REM this script must set QTDIR32 path to the root of the Qt folder. Example:
REM set QTDIR32=k:\Qt\5\5.4\msvc2013_opengl\

call set_qt_paths.bat
set VS_TOOLS_DIR=%VS140COMNTOOLS%

SETLOCAL

RMDIR /S /Q binaries\

call "%VS_TOOLS_DIR%..\..\VC\vcvarsall.bat" amd64

pushd ..\
%QTDIR64%\bin\qmake.exe -tp vc -r
popd

msbuild ..\ShaderPlayground.sln /t:Build /p:Configuration=Release;PlatformToolset=v140

xcopy /R /Y ..\bin\release\x64\*.exe binaries\
xcopy /R /Y "3rdparty binaries"\Win\64\* binaries\

SETLOCAL
SET PATH=%QTDIR64%\bin\
FOR %%p IN (binaries\*.exe) DO %QTDIR64%\bin\windeployqt.exe --dir binaries\Qt --force --release --no-compiler-runtime --no-angle --no-translations %%p
ENDLOCAL

xcopy /R /Y %SystemRoot%\System32\msvcp140.dll binaries\msvcr\
xcopy /R /Y %SystemRoot%\System32\vcruntime140.dll binaries\msvcr\
xcopy /R /Y "%programfiles(x86)%\Windows Kits\10\Redist\ucrt\DLLs\x64\*" binaries\msvcr\

del binaries\Qt\opengl*.*

ENDLOCAL

"c:\Program Files (x86)\Inno Setup 5\iscc" setup.iss