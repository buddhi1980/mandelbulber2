; This script requires Inno Setup Compiler 5.5.9 or later to compile
; The Inno Setup Compiler (and IDE) can be found at http://www.jrsoftware.org/isinfo.php

; General documentation on how to use InnoSetup scripts: http://www.jrsoftware.org/ishelp/index.php

#ifndef Platform
  #error Platform undefined. Pass /DPlatform
#endif

#ifndef Configuration
  #error Configuration undefined. Pass /DConfiguration
#endif

#define MyAppName "Mandelbulber2"
#define MyAppInstallerVersion GetFileVersion("mandelbulber2.exe")
#define MyAppPublisher "Mandelbulber Team"
#define MyAppPublisherURL "http://www.mandelbulber.com"
#define MyAppURL "https://github.com/buddhi1980/mandelbulber2"
#define MyAppExeName "mandelbulber2.exe"
#define MyAppIcoName "mandelbulber2.ico"

[Setup]
AppId={{82DA50F1-72DF-47FE-9FE8-023FFC5695B1}
AppName={#MyAppName}
AppVersion={#MyAppInstallerVersion}
VersionInfoVersion={#MyAppInstallerVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppPublisherURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
AppCopyright=Copyright (C) 2017 {#MyAppPublisher}
BackColor=clWhite
BackSolid=yes
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
OutputBaseFilename=Mandelbulber2-v{#MyAppInstallerVersion}-Setup
OutputDir=..\..\Setup\{#Configuration}\{#Platform}
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
LicenseFile=COPYING.txt
WizardImageFile=setup_inno.bmp
WizardSmallImageFile=setup_inno_small.bmp

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
DestDir: "{app}"; Flags: ignoreversion recursesubdirs; Source:"*"

[UninstallDelete]
; Deletes the entire installation directory, including files and subdirectories
Type: filesandordirs; Name: "{app}";

[Run]
Filename: "{app}\mandelbulber2.exe"; Description: "Launch Mandelbulber2 application"; \
    Flags: postinstall nowait skipifsilent unchecked

