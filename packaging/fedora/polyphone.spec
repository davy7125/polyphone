%global source_date_epoch_from_changelog 0
Name:           polyphone
Version:        2.5
Release:        1%{?dist}
Summary:        Soundfont editor
License:        GPLv3+
URL:            https://www.polyphone.io
Group:          Applications/Multimedia
BuildRequires:  alsa-lib-devel rtaudio-devel zlib-devel libogg-devel flac-devel libvorbis-devel glib2-devel openssl-devel rtmidi-devel stk-devel libsndfile-devel
Requires:       alsa-utils rtaudio zlib libogg flac libvorbis glib2 openssl rtmidi stk qt6-qtbase qt6-qtsvg libsndfile
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)
Packager:       Davy Triponney

%description
Turn any sound into a playable instrument with the help of SoundFonts.
These files contain a multitude of audio samples put together and
configured so as to form musical instruments that can be used by
synthesizers such as fluidsynth and played using a MIDI keyboard.
The goal of Polyphone is to provide:
 - a simple and efficient interface for creating and editing soundfonts,
available on Linux, Windows and Mac OS X,
 - tools to facilitate and automate the setting of different parameters,
making it possible to handle a large amount of data.
 
Main features:
 - import from sf2, sfz, sfArk, sf3, grandOrgue
 - export as sf2, sfz, sf3,
 - support of sf2 format v2.01 and v2.04 (with 24-bit samples),
 - support for jack and ASIO audio servers,
 - simultaneous editing of parameters,
 - specific tools for musical instrument creation,
 - automatic detection of the sample pitch and correction,
 - automatic loop of samples,
 - a recorder, for the record of what is played in a .wav file,
 - built-in synthesizer, using a virtual keyboard or MIDI signals,
 - user manual, forum and online soundfont repository,
 - available in several languages.

%prep
rm *.rpm || true
rm -r x86_64 || true

%build

%clean
rm -rf $RPM_BUILD_ROOT

%install
rm -rf $RPM_BUILD_ROOT
mkdir -p %{buildroot}%{_bindir}
mkdir -p %{buildroot}%{_datadir}/icons
mkdir -p %{buildroot}%{_datadir}/mime/packages
mkdir -p %{buildroot}%{_docdir}/%{name}
install -p -m 755 ../polyphone %{buildroot}%{_bindir}
install -p -m 644 ../polyphone.png %{buildroot}%{_datadir}/icons
install -p -m 644 ../../sources/contrib/polyphone.xml %{buildroot}%{_datadir}/mime/packages/polyphone.xml
install -p -m 644 ../../sources/changelog %{buildroot}%{_docdir}/%{name}
desktop-file-install --vendor="" --dir=%{buildroot}%{_datadir}/applications/ ../../sources/contrib/io.polyphone.polyphone.desktop

%post
# icons
touch --no-create %{_datadir}/icons/hicolor
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
  %{_bindir}/gtk-update-icon-cache -q %{_datadir}/icons/hicolor;
fi
update-mime-database %{_datadir}/mime &> /dev/null || :
update-desktop-database &> /dev/null || :

# Mime types
update-desktop-database

%postun
# icons
touch --no-create %{_datadir}/icons/hicolor
if [ -x %{_bindir}/gtk-update-icon-cache ]; then
  %{_bindir}/gtk-update-icon-cache -q %{_datadir}/icons/hicolor;
fi
update-mime-database %{_datadir}/mime &> /dev/null || :
update-desktop-database &> /dev/null || :

%files
%license ../../sources/LICENSE.txt
%doc ../../sources/contrib/man/man1/polyphone.1
%doc ../../sources/contrib/man/fr/man1/polyphone.1
%{_bindir}/%{name}
%{_docdir}/%{name}
%{_datadir}/applications/io.polyphone.polyphone.desktop
%{_datadir}/icons/polyphone.png
%{_datadir}/mime/packages/polyphone.xml

