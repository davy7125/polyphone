# Libraries of the plugin built by vcpkg (.github/workflows/build-plugin.yml): static with the dynamic runtime of the
# plugin (/MD), release only
set(VCPKG_TARGET_ARCHITECTURE x64)
set(VCPKG_CRT_LINKAGE dynamic)
set(VCPKG_LIBRARY_LINKAGE static)
set(VCPKG_BUILD_TYPE release)
