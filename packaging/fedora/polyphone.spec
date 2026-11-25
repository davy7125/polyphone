# RPM package of the editor for Fedora, built by packaging/build_fedora_package.sh (which gives polyphone_tag and adds
# the changelog). The source is the whole repository: the CMake project in sources/ installs files of packaging/ and
# the README of the root. Build dependencies: sudo dnf builddep packaging/fedora/polyphone.spec

# Tag built ("2.7.3", "2.8.0-beta"), the version of the package being "2.8.0~beta" (before 2.8.0)
%{!?polyphone_tag:%global polyphone_tag 0}
%global polyphone_version %{lua: print((rpm.expand("%{polyphone_tag}"):gsub("[-]", "~", 1)))}

# No link-time optimization (default of Fedora): built as the other official builds
%global _lto_cflags %{nil}
# CMake project in sources/
%global _vpath_srcdir sources

Name:           polyphone
Version:        %{polyphone_version}
Release:        1%{?dist}
Summary:        Soundfont editor
# Polyphone and sfArkLib: GPL-3.0-or-later, copy of RtAudio: MIT (RtMidi of the system), QtSingleApplication:
# BSD-3-Clause
License:        GPL-3.0-or-later AND MIT AND BSD-3-Clause
URL:            https://www.polyphone.io
Source0:        https://github.com/davy7125/polyphone/archive/%{polyphone_tag}/%{name}-%{polyphone_tag}.tar.gz

BuildRequires:  cmake >= 3.16
BuildRequires:  gcc-c++
BuildRequires:  pkgconfig
BuildRequires:  python3
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qtsvg-devel
BuildRequires:  qt6-linguist
BuildRequires:  pkgconfig(alsa)
BuildRequires:  pkgconfig(jack)
BuildRequires:  pkgconfig(rtmidi)
BuildRequires:  pkgconfig(sndfile)
BuildRequires:  pkgconfig(ogg)
BuildRequires:  pkgconfig(vorbis)
BuildRequires:  pkgconfig(vorbisfile)
BuildRequires:  pkgconfig(vorbisenc)
BuildRequires:  pkgconfig(openssl)
BuildRequires:  pkgconfig(zlib)
BuildRequires:  desktop-file-utils
BuildRequires:  libappstream-glib
# Plugins of Qt SVG: the style sheets and the icons use SVG images
Requires:       qt6-qtsvg%{?_isa}
Requires:       hicolor-icon-theme
Requires:       shared-mime-info

%description
Polyphone turns any sound into a musical instrument. A soundfont is a single
file holding everything a synthesizer needs to play an instrument: the
recorded samples, how they are spread across the keyboard and the
velocities, and how they are shaped by envelopes, filters and modulators.
Polyphone creates soundfonts from recordings, refines existing ones, converts
them to other formats and plays them with its built-in synthesizer.

Main features:
 - sf2 (versions 2.01 and 2.04, 24-bit samples) and sf3 files opened and
   saved, sfz and DLS imported and exported, sfArk archives and GrandOrgue
   organs imported,
 - parameters of many samples, instruments or presets edited at once,
   graphical editors for the key and velocity ranges and the envelopes,
 - sample editor with loop points, pitch and tuning estimation and an
   equalizer, timbre view showing the level of each harmonic on every key,
 - more than 30 tools such as the automatic loop, the creation of missing
   samples, transposition, chords, organ mixtures,
 - built-in synthesizer with reverb and chorus, played with the virtual
   keyboard, a MIDI keyboard or the computer keyboard, in more than 200
   temperaments and scales, audio output through ALSA or JACK,
 - MIDI file player with an export to wav, recorder of the sound (wav) or of
   what is played (MIDI),
 - soundfont explorer for browsing a directory, online repository of the
   soundfonts shared on polyphone.io,
 - command line for converting files and rendering MIDI files,
 - MCP server letting an AI assistant browse and edit the open soundfonts,
 - interface translated into 16 languages.

The Polyphone plugin (VST3, CLAP, LV2), playing soundfonts in a DAW with the
same synthesizer, is distributed separately.

%prep
%autosetup -n %{name}-%{polyphone_tag}
# Version written in sources/version.txt and in the metainfo, as for the other official builds
python3 packaging/set_version.py %{polyphone_tag}

%build
# Official package: the notifications of a new version are shown. Release optimized as the other official builds
# (the macro sets the flags of the release type to -DNDEBUG alone, -O3 added), with the copy of RtAudio in
# sources/lib (no dependency on the version of RtAudio of the system)
%cmake \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_C_FLAGS_RELEASE:STRING="-O3 -DNDEBUG" \
    -DCMAKE_CXX_FLAGS_RELEASE:STRING="-O3 -DNDEBUG" \
    -DPOLYPHONE_OFFICIAL_BUILD=ON \
    -DPOLYPHONE_LOCAL_RTAUDIO=ON
%cmake_build

%install
%cmake_install

%check
desktop-file-validate %{buildroot}%{_datadir}/applications/io.polyphone.polyphone.desktop
appstream-util validate-relax --nonet %{buildroot}%{_datadir}/metainfo/io.polyphone.polyphone.metainfo.xml

%files
%license LICENSE.txt
# README and changelog installed by CMake
%doc %{_docdir}/%{name}
%{_bindir}/%{name}
%{_datadir}/applications/io.polyphone.polyphone.desktop
%{_datadir}/metainfo/io.polyphone.polyphone.metainfo.xml
%{_datadir}/mime/packages/%{name}.xml
%{_datadir}/icons/hicolor/512x512/apps/%{name}.png
%{_datadir}/icons/hicolor/*/mimetypes/audio-*.png
%{_mandir}/man1/%{name}.1*
%lang(de) %{_mandir}/de/man1/%{name}.1*
%lang(es) %{_mandir}/es/man1/%{name}.1*
%lang(fr) %{_mandir}/fr/man1/%{name}.1*
%lang(it) %{_mandir}/it/man1/%{name}.1*
%lang(ja) %{_mandir}/ja/man1/%{name}.1*
%lang(pt_BR) %{_mandir}/pt_BR/man1/%{name}.1*
%lang(ru) %{_mandir}/ru/man1/%{name}.1*
%lang(zh_CN) %{_mandir}/zh_CN/man1/%{name}.1*

%changelog
