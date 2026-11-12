# Scripts of the GitHub workflows

Used by `.github/workflows/ci.yml` (each change, and before a release) and `build-plugin.yml`. They also run by
hand on Linux.

| Script | Purpose |
|---|---|
| `check_dependencies.py` | The parts of the sources only include what they may use: `synth_core` nothing (no Qt), `core` itself, `synth_core` and `lib` (no widgets of Qt), `plugin` itself and `synth_core`, `mcp` no interface except the tools and the model of the timbre view (`editor/timbre/model`). Run it before a commit: a forbidden include is printed with its line. |
| `build_editor.sh` | Builds the editor with CMake as the official builds (release, local RtAudio), with ccache if found: `build_editor.sh <repository> <build directory> [CMake options]`. `JOBS` and `LOCAL_RTMIDI=1` in the environment. |
| `install_dependencies.sh` | Ubuntu packages by group: `editor`, `qt5`, `synth_core`, `plugin`, `tests`. |
| `get_soundfonts.sh` | Soundfonts of `tests/render` in a directory: FluidR3_GM and TimGM6mb from the packages, VintageDreamsWaves from the sources of FluidSynth (checked by SHA-256). |

The renderings compared by the workflow are described in `tests/render/README.md`, `tests/dls/README.md` and
`tests/plugin/README.md`. When a change of the sound is expected, the job comparing with the previous version fails:
its renderings are uploaded to be listened to.
