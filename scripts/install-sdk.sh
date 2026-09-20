#!/usr/bin/env bash
# Installs ey3 so other projects (e.g. templates/ey3-starter, copied
# elsewhere) can use it without vendoring a copy of it into themselves:
#
#   - Desktop: builds libey3.so and `cmake --install`s it (+ headers +
#     ey3Config.cmake) into a prefix, default /usr/local (needs sudo -- pass
#     EY3_PREFIX=$HOME/.local to install without root, then add that to
#     CMAKE_PREFIX_PATH when configuring a project that uses it).
#   - Android: there's no installable prebuilt equivalent (a static lib built
#     with one NDK version isn't reliably ABI-compatible with an app built
#     against another), so instead this copies the engine sources, headers
#     and CMake modules to ONE shared location, default
#     ~/.local/share/ey3-android-sdk, that any Android project points its own
#     NDK build at through EY3_ANDROID_SDK (see any apps/*/android/CMakeLists.txt).
#   - Also refreshes templates/ey3-starter/doc/ey3-api.md from doc/ey3-api.md.
#
# Run again after changing anything under include/, src/ or doc/ey3-api.md
# to refresh all of the above. The desktop and Android steps are independent
# -- one failing (e.g. sudo needing a real TTY) does not skip the other.
set -uo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PREFIX="${EY3_PREFIX:-/usr/local}"
ANDROID_SDK="${EY3_ANDROID_SDK:-$HOME/.local/share/ey3-android-sdk}"
desktop_ok=1

echo "--- Desktop: installing to ${PREFIX} ---"
if cmake -S "${ROOT}/desktop" -B "${ROOT}/build-desktop" -G Ninja >/dev/null \
	&& cmake --build "${ROOT}/build-desktop" --target ey3; then
	# Only the install step (writing outside this repo) needs elevated
	# privileges when PREFIX isn't user-writable -- configure/build above
	# must NOT run as root, or build-desktop/ ends up root-owned and breaks
	# your next unprivileged build.
	if cmake --install "${ROOT}/build-desktop" --prefix "${PREFIX}" 2>/tmp/ey3-install-sdk.err; then
		:
	else
		echo "(needs elevated privileges for ${PREFIX}, retrying with sudo)"
		if ! sudo cmake --install "${ROOT}/build-desktop" --prefix "${PREFIX}"; then
			echo "Desktop install FAILED (see above). Continuing with the Android step."
			desktop_ok=0
		fi
	fi
else
	echo "Desktop configure/build FAILED. Continuing with the Android step."
	desktop_ok=0
fi

echo "--- Android: vendoring sources into ${ANDROID_SDK} ---"
rm -rf "${ANDROID_SDK}"
mkdir -p "${ANDROID_SDK}"
# The whole of include/ and src/, rather than a hand-kept list of files: the
# list went stale twice (mat4.cpp, head_tracker.cpp) and the breakage only
# showed up when building from outside the repo. The few desktop-only sources
# that come along are never compiled by the Android build -- cmake/sources.cmake,
# copied below, is the list that decides.
cp -r "${ROOT}/include" "${ANDROID_SDK}/include"
cp -r "${ROOT}/src" "${ANDROID_SDK}/src"
# The CMake modules that build the engine and package an APK. A project using
# the SDK includes these instead of carrying its own copy of either.
cp -r "${ROOT}/cmake" "${ANDROID_SDK}/cmake"
cat > "${ANDROID_SDK}/VENDORED.md" <<EOF
This is a generated copy of include/ and part of src/ from the main EY3 repo
(${ROOT}), produced by scripts/install-sdk.sh. Do not edit it here -- edit
the main repo and re-run that script to refresh this copy. Every Android
project pointing EY3_ANDROID_SDK here shares this one copy.
EOF

echo "--- Docs: refreshing templates/ey3-starter/doc/ey3-api.md ---"
mkdir -p "${ROOT}/templates/ey3-starter/doc"
cp "${ROOT}/doc/ey3-api.md" "${ROOT}/templates/ey3-starter/doc/ey3-api.md"

echo
if [ "${desktop_ok}" -eq 1 ]; then
	echo "Done. For a new project (see templates/ey3-starter/README.md):"
	echo "  desktop: find_package(ey3) works out of the box if EY3_PREFIX was"
	echo "           left at /usr/local; otherwise configure with"
	echo "           -DCMAKE_PREFIX_PATH=${PREFIX}"
	echo "  android: configure with -DEY3_ANDROID_SDK=${ANDROID_SDK} (this is"
	echo "           already the default if you didn't override EY3_ANDROID_SDK)"
else
	echo "Android SDK + docs refreshed OK, but the desktop install did NOT complete -- see above."
	exit 1
fi
