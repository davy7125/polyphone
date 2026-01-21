#ifndef MyAppVersion
  #define MyAppVersion "0.0.0"
#endif

[InstallDelete]
Type: filesandordirs; Name: "{app}"

[Setup]
AppId={{83549E04-CB44-4E12-85B6-9FBBA3B0FE65}
AppName=Polyphone
AppVersion={#MyAppVersion}
AppPublisher=Davy Triponney
AppPublisherURL=https://www.polyphone.io
AppSupportURL=https://www.polyphone.io
AppUpdatesURL=https://www.polyphone.io
ChangesAssociations=yes
DefaultDirName={autopf}\Polyphone
DefaultGroupName=Polyphone
LicenseFile={#SourceDir}\license.txt
OutputDir=.
OutputBaseFilename=Polyphone_{#MyAppVersion}-x64
SetupIconFile=polyphone.ico
Compression=zip
SolidCompression=no
UninstallDisplayIcon={app}\polyphone.exe
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Languages]
Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"
Name: "czech";  MessagesFile: "compiler:Languages\Czech.isl"
Name: "danish";  MessagesFile: "compiler:Languages\Danish.isl"
Name: "dutch";  MessagesFile: "compiler:Languages\Dutch.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french";  MessagesFile: "compiler:Languages\French.isl"
Name: "german";  MessagesFile: "compiler:Languages\German.isl"
Name: "italian"; MessagesFile: "compiler:Languages\Italian.isl"
Name: "japanese"; MessagesFile: "compiler:Languages\Japanese.isl"
Name: "korean"; MessagesFile: "compiler:Languages\Korean.isl"
Name: "polish"; MessagesFile: "compiler:Languages\Polish.isl"
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"
Name: "russian"; MessagesFile: "compiler:Languages\Russian.isl"
Name: "serbian"; MessagesFile: "compiler:Languages\SerbianCyrillic.isl"
Name: "spanish"; MessagesFile: "compiler:Languages\Spanish.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}";

[Files]
Source: "{#SourceDir}\polyphone.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceDir}\*.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#SourceDir}\platforms\*.dll"; DestDir: "{app}\platforms"; Flags: ignoreversion
Source: "{#SourceDir}\styles\*.dll"; DestDir: "{app}\styles"; Flags: ignoreversion
Source: "{#SourceDir}\tls\*.dll"; DestDir: "{app}\tls"; Flags: ignoreversion

[Icons]
Name: "{group}\Polyphone"; Filename: "{app}\polyphone.exe"
Name: "{commondesktop}\Polyphone"; Filename: "{app}\polyphone.exe"; Tasks: desktopicon

[Registry]
Root: HKCR; Subkey: ".sf2"; ValueType: string; ValueName: ""; ValueData: "PolyphoneFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".sf3"; ValueType: string; ValueName: ""; ValueData: "PolyphoneFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".sfz"; ValueType: string; ValueName: ""; ValueData: "PolyphoneFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".sfArk"; ValueType: string; ValueName: ""; ValueData: "PolyphoneFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: ".organ"; ValueType: string; ValueName: ""; ValueData: "PolyphoneFile"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "PolyphoneFile"; ValueType: string; ValueName: ""; ValueData: "soundfont"; Flags: uninsdeletekey
Root: HKCR; Subkey: "PolyphoneFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\polyphone.exe,1"
Root: HKCR; Subkey: "PolyphoneFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\polyphone.exe"" ""%1"""

[Run]
Filename: "{app}\polyphone.exe"; Description: "{cm:LaunchProgram,Polyphone}"; Flags: nowait postinstall skipifsilent

