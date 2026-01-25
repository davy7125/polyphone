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
OutputBaseFilename=Polyphone_{#MyAppVersion}-windows_x64-installer
SetupIconFile=polyphone.ico
Compression=zip
SolidCompression=no
UninstallDisplayIcon={app}\polyphone.exe
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Languages]
Name: "catalan"; MessagesFile: "compiler:Languages\Catalan.isl"
Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"
Name: "czech"; MessagesFile: "compiler:Languages\Czech.isl"
Name: "danish"; MessagesFile: "compiler:Languages\Danish.isl"
Name: "dutch"; MessagesFile: "compiler:Languages\Dutch.isl"
Name: "english"; MessagesFile: "compiler:Default.isl"
Name: "french"; MessagesFile: "compiler:Languages\French.isl"
Name: "german"; MessagesFile: "compiler:Languages\German.isl"
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
Root: HKCR; Subkey: ".sf2"; ValueType: string; ValueData: "Polyphone.sf2"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Polyphone.sf2"; ValueType: string; ValueData: "SoundFont 2 file"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Polyphone.sf2\DefaultIcon"; ValueType: string; ValueData: "{app}\polyphone.exe,1"
Root: HKCR; Subkey: "Polyphone.sf2\shell\open\command"; ValueType: string; ValueData: """{app}\polyphone.exe"" ""%1"""

Root: HKCR; Subkey: ".sf3"; ValueType: string; ValueData: "Polyphone.sf3"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Polyphone.sf3"; ValueType: string; ValueData: "SoundFont 3 file"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Polyphone.sf3\DefaultIcon"; ValueType: string; ValueData: "{app}\polyphone.exe,2"
Root: HKCR; Subkey: "Polyphone.sf3\shell\open\command"; ValueType: string; ValueData: """{app}\polyphone.exe"" ""%1"""

Root: HKCR; Subkey: ".sfArk"; ValueType: string; ValueData: "Polyphone.sfArk"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Polyphone.sfArk"; ValueType: string; ValueData: "sfArk soundfont archive"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Polyphone.sfArk\DefaultIcon"; ValueType: string; ValueData: "{app}\polyphone.exe,3"
Root: HKCR; Subkey: "Polyphone.sfArk\shell\open\command"; ValueType: string; ValueData: """{app}\polyphone.exe"" ""%1"""

Root: HKCR; Subkey: ".sfz"; ValueType: string; ValueData: "Polyphone.sfz"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Polyphone.sfz"; ValueType: string; ValueData: "SFZ instrument definition"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Polyphone.sfz\DefaultIcon"; ValueType: string; ValueData: "{app}\polyphone.exe,4"
Root: HKCR; Subkey: "Polyphone.sfz\shell\open\command"; ValueType: string; ValueData: """{app}\polyphone.exe"" ""%1"""

Root: HKCR; Subkey: ".organ"; ValueType: string; ValueData: "Polyphone.organ"; Flags: uninsdeletevalue
Root: HKCR; Subkey: "Polyphone.organ"; ValueType: string; ValueData: "GrandOrgue organ definition"; Flags: uninsdeletekey
Root: HKCR; Subkey: "Polyphone.organ\DefaultIcon"; ValueType: string; ValueData: "{app}\polyphone.exe,5"
Root: HKCR; Subkey: "Polyphone.organ\shell\open\command"; ValueType: string; ValueData: """{app}\polyphone.exe"" ""%1"""

[Code]
function IsWindows10AndAbove: Boolean;
var
  Major, Minor, Build: Cardinal;
begin
  if not GetWindowsVersion(Major, Minor, Build) then
  begin
    Result := False;
    exit;
  end;
  // Windows 10 correspond à Major = 10
  Result := Major >= 10;
end;

[Run]
Filename: "{sys}\ie4uinit.exe"; Parameters: "-ClearIconCache"; StatusMsg: "Refreshing Windows icon cache..."; Flags: runhidden; Check: not IsWindows10AndAbove and not IsWin64
Filename: "{sys}\ie4uinit.exe"; Parameters: "-Show"; StatusMsg: "Refreshing Windows icon cache..."; Flags: runhidden; Check: IsWindows10AndAbove and not IsWin64
Filename: "{sys}\ie4uinit.exe"; Parameters: "-ClearIconCache"; StatusMsg: "Refreshing Windows icon cache..."; Flags: runhidden 64bit; Check: not IsWindows10AndAbove and IsWin64
Filename: "{sys}\ie4uinit.exe"; Parameters: "-Show"; StatusMsg: "Refreshing Windows icon cache..."; Flags: runhidden 64bit; Check: IsWindows10AndAbove and IsWin64
Filename: "{app}\polyphone.exe"; Description: "{cm:LaunchProgram,Polyphone}"; Flags: nowait postinstall skipifsilent
