# The Android NDK's CMake toolchain file unconditionally appends -g to every
# build type (including Release), so full DWARF debug info is compiled in.
# Nothing here uses Gradle's externalNativeBuild, which normally strips that
# before copying the .so into lib/ for packaging, so without this the aapt
# step below bundles the unstripped .so straight into the APK: debug sections
# alone typically account for the large majority of its size.
function(ey3_strip_target target)
	add_custom_command(TARGET ${target} POST_BUILD
		COMMAND ${CMAKE_STRIP} --strip-unneeded $<TARGET_FILE:${target}>
		COMMENT "Stripping debug info from ${target}")
endfunction()
