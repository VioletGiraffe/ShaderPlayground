[Setup]
AppName=Shader Playground
AppId=ShaderPlayground
AppVerName=Shader Playground
DefaultDirName={pf}\Shader Playground
DefaultGroupName=Shader Playground
AllowNoIcons=true
OutputDir=.
OutputBaseFilename=ShaderPlayground
UsePreviousAppDir=yes

;SetupIconFile=..\app\resources\FTS_icon.ico
UninstallDisplayIcon={app}\ShaderPlayground.exe

WizardImageBackColor=clWhite
ShowTasksTreeLines=yes

SolidCompression=true
LZMANumBlockThreads=4
Compression=lzma2/ultra64
LZMAUseSeparateProcess=yes
LZMABlockSize=8192

[Files]

;App binaries
Source: binaries/*; DestDir: {app}; Flags: ignoreversion

;Qt binaries
Source: binaries/Qt/*; DestDir: {app}; Flags: ignoreversion recursesubdirs

;MSVC binaries
Source: binaries/msvcr/*; DestDir: {app}; Flags: ignoreversion

[Icons]
Name: {group}\Shader Playground; Filename: {app}\ShaderPlayground.exe;
Name: {group}\{cm:UninstallProgram,Shader Playground}; Filename: {uninstallexe}

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons};

[Run]
Filename: {app}\ShaderPlayground.exe; Description: {cm:LaunchProgram,Shader Playground}; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: dirifempty; Name: "{app}"