# Libraries of the plugin built by vcpkg (.github/workflows/build-plugin.yml): static with the static runtime of the
# plugin (/MT, see sources/plugin/CMakeLists.txt), release only
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE static)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_BUILD_TYPE release)
