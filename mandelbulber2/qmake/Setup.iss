; This script requires Inno Setup Compiler 5.5.9 or later to compile
; The Inno Setup Compiler (and IDE) can be found at http://www.jrsoftware.org/isinfo.php

; General documentation on how to use InnoSetup scripts: http://www.jrsoftware.org/ishelp/index.php

;#ifndef Platform
;  #error Platform undefined. Pass /DPlatform
;#endif

;#ifndef Configuration
;  #error Configuration undefined. Pass /DConfiguration
;#endif

#define MyAppName "Mandelbulber2"
#define MyAppInstallerVersion "2.32"
#define MyAppPublisher "Mandelbulber Team"
#define MyAppPublisherURL "http://www.mandelbulber.com"
#define MyAppURL "https://github.com/buddhi1980/mandelbulber2"
#define MyAppExeName "mandelbulber2.exe"
#define MyAppIcoName "..\deploy\win64\mandelbulber2.ico"

#define MyBinDir "build\Desktop_Qt_6_7_2_MSVC2019_64bit-Release\bin"
#define MyQtBinDir "C:\Qt\6.7.2\msvc2019_64\bin\"

[Setup]
AppId={{82DA50F1-72DF-47FE-9FE8-023FFC5695B1}
AppName={#MyAppName}
;AppVersion={#MyAppInstallerVersion}
AppVersion="2.32"
VersionInfoVersion={#MyAppInstallerVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppPublisherURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
AppCopyright=Copyright (C) 2024 {#MyAppPublisher}
BackColor=clWhite
BackSolid=yes
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputBaseFilename=Mandelbulber2-v{#MyAppInstallerVersion}-Setup
OutputDir=..\..\Setup\
Compression=lzma2
InternalCompressLevel=ultra64
SolidCompression=yes
DisableDirPage=yes
DisableReadyPage=yes
SetupIconFile={#MyAppIcoName}
WizardImageStretch=no
WindowResizable=no
CloseApplications=yes
ChangesEnvironment=yes
ArchitecturesInstallIn64BitMode=x64
LicenseFile=..\deploy\COPYING
WizardImageFile=..\msvc\setup_inno.bmp
WizardSmallImageFile=..\msvc\setup_inno_small.bmp

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; \
    GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Icons]
Name: "{group}\Mandelbulber"; Filename: "{app}\mandelbulber2.exe"; WorkingDir: "{app}"
Name: "{group}\Uninstall Mandelbulber"; Filename: "{uninstallexe}"
Name: "{userdesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; \
    IconFilename: "{app}\{#MyAppIcoName}"; Tasks: desktopicon

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl";

[Types]
Name: "full"; Description: "Full installation"

[Components]

[Files]
Source: "C:\Users\buddh\Git\mandelbulber\mandelbulber2\formula\*"; DestDir: "{app}\formula"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\buddh\Git\mandelbulber\mandelbulber2\opencl\*"; DestDir: "{app}\opencl"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\buddh\Git\mandelbulber\mandelbulber2\language\*"; DestDir: "{app}\language"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\buddh\Git\mandelbulber\mandelbulber2\deploy\doc\*"; DestDir: "{app}\doc"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\buddh\Git\mandelbulber\mandelbulber2\deploy\share\mandelbulber2\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#MyBinDir}\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "C:\Users\buddh\Git\mandelbulber\mandelbulber2\deploy\NEWS"; DestDir: "{app}\doc"; Flags: ignoreversion
Source: "C:\Users\buddh\Git\mandelbulber\mandelbulber2\deploy\COPYING"; DestDir: "{app}\doc"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6Widgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6UiTools.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6Test.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6Svg.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6Qml.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6OpenGLWidgets.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6OpenGL.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6Network.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6Multimedia.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6Gui.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\Qt6Concurrent.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#MyQtBinDir}\..\plugins\platforms\*.dll"; Excludes: "*.pdb, *d.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "{#MyQtBinDir}\..\plugins\platforms\qdirect2d.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "{#MyQtBinDir}\..\plugins\iconengines\qsvgicon.dll"; DestDir: "{app}\iconengines"; Flags: ignoreversion
Source: "{#MyQtBinDir}\..\plugins\imageformats\*.dll"; Excludes: "*.pdb, *d.dll"; DestDir: "{app}\imageformats"; Flags: ignoreversion

[UninstallDelete]
; Deletes the entire installation directory, including files and subdirectories
Type: filesandordirs; Name: "{app}";

[Run]
Filename: "{app}\mandelbulber2.exe"; Description: "Launch Mandelbulber2 application"; \
    Flags: postinstall nowait skipifsilent unchecked

