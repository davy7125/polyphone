# RtAudio

Copy of [RtAudio](https://github.com/thestk/rtaudio) 6.0.1 (`RtAudio.cpp`, `RtAudio.h`), used on Windows and macOS,
and on Linux with `-DPOLYPHONE_LOCAL_RTAUDIO=ON` (the official builds).

## Local changes

One change, in the WASAPI part of `RtAudio.cpp`: each line is marked with a comment `Polyphone`.

**Media Foundation is loaded at run time** (`WasapiMediaFoundation`), only when WASAPI must convert a sample rate.
RtAudio imported `mfplat.dll` and created its resampler for every WASAPI stream, even without conversion. The N
editions of Windows have no Media Foundation without the Media Feature Pack, and Polyphone did not start there.

- `mfplat.dll` is loaded from the directory of the system only (`LOAD_LIBRARY_SEARCH_SYSTEM32`), and its five functions
  (`MFStartup`, `MFShutdown`, `MFCreateMediaType`, `MFCreateMemoryBuffer`, `MFCreateSample`) are found by
  `GetProcAddress`. `WasapiResampler` declares functions of the same names, so its code calling them is unchanged.
- `WasapiResampler` sets up nothing when the two rates are equal (`Convert` already copies the samples then).
- Without Media Foundation or its resampler, `WasapiResampler::isValid()` is false and the stream stops with an error
  that names the Media Feature Pack, instead of a crash (the result of `CoCreateInstance` was not checked).
- `mfplat.lib` is no longer linked (`#pragma comment` here, `sources/CMakeLists.txt` for MinGW).

Polyphone opens a WASAPI stream at the rate of the device (`AudioDevice::getSampleRates`), so no conversion is made.

## Updating RtAudio

Apply the change again to the new version, then check that the Windows build passes: the step "Check that Media
Foundation is not imported" of `.github/workflows/build-windows-x64.yml` fails if `polyphone.exe` imports
`MFPlat.DLL` again, and the next step starts it on a Windows without Media Foundation.
