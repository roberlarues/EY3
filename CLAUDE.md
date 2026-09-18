# CLAUDE.md

## Keep templates/ey3-starter in sync

`templates/ey3-starter` is a standalone starter project meant to be copied
out of this repo (see its README.md). It does **not** vendor ey3's headers
or source -- it references a centrally-installed copy (`scripts/install-sdk.sh`
installs desktop's `libey3.so` via `cmake --install`, and vendors Android's
engine sources into `~/.local/share/ey3-android-sdk`). Several files there
duplicate information from the main repo by necessity (a standalone CMake
package needs its own `find_dependency()` calls, a standalone NDK build
needs its own file list, etc.) and go stale silently if the main repo
changes without touching them. After any of the changes below, update the
listed file(s), then run `./scripts/install-sdk.sh` and verify by copying
`templates/ey3-starter` to a directory outside this repo (e.g. `/tmp`) and
building both `cmake -S . -B build` (desktop) and `cmake -S android -B
build-android ...` (Android, `app-unsigned` target) from there -- if it only
builds from inside this repo, something still references it by path.

| If you change... | ...also update |
| --- | --- |
| A public class/method in `include/*.h` (new, renamed, removed, or behavior a caller needs to know) | `doc/ey3-api.md` (source of truth; `install-sdk.sh` copies it into the starter's `doc/`) |
| The list of `.cpp` files compiled into `ey3` (`cmake/sources.cmake`, or the Android-only/desktop-only additions in the root `CMakeLists.txt` / `desktop/CMakeLists.txt`) | `templates/ey3-starter/android/CMakeLists.txt`'s `EY3_SOURCES` list (hardcodes the same files, since Android compiles ey3 from source instead of linking a prebuilt lib -- see its top-of-file comment for why) |
| `desktop/CMakeLists.txt`'s `find_package(...)` calls or `target_link_libraries(ey3 PUBLIC ...)` (e.g. a new required system lib or OpenCV component) | `desktop/cmake/ey3Config.cmake.in`'s `find_dependency(...)` calls -- a consumer's `find_package(ey3)` re-resolves these itself, they don't come for free from the export |
| The root `CMakeLists.txt`'s Android OpenCV static-lib list, or `EY3_WITH_CV` handling | `templates/ey3-starter/android/CMakeLists.txt` (mirrors that list for the same reason as above) |
| The `aapt`/`jarsigner`/`zipalign` packaging recipe in `apps/*/CMakeLists.txt` | `templates/ey3-starter/android/CMakeLists.txt` (same recipe, collapsed to one app) |
| `WindowDesktop`'s constructor, `MOBILE_WIDTH_*`/`MOBILE_HEIGHT_*` constants, or anything else in `include/system/window_desktop.h` that starter code calls directly | `templates/ey3-starter/desktop/main_desktop.cpp` and `doc/ey3-api.md` |
| The `runProgram(Engine&)` pattern itself (unlikely, but if `apps/*/src/program.h` conventions change) | `templates/ey3-starter/include/program.h` / `src/program.cpp` |

If a change doesn't fit the table but touches something a fresh `find_package(ey3)`
consumer or a from-scratch Android build would hit, treat that as a signal to
re-run the standalone verification above rather than assuming it's fine.
