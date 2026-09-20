# Creates a signing keystore, unless it is already there.
#
# Run with -P. The caller passes KEYSTORE_FILE, KEYTOOL, KEYALIAS, STOREPASS,
# KEYPASS and COMPANY.
#
# The "unless it is already there" is the point: keytool refuses to add an
# alias twice, so a rule that simply ran it again would stop the build the
# moment anything made the build system think the keystore was out of date.

if(EXISTS "${KEYSTORE_FILE}")
	message(STATUS "Keystore already exists, keeping it: ${KEYSTORE_FILE}")
	return()
endif()

message(STATUS "Creating keystore ${KEYSTORE_FILE}")
execute_process(
	COMMAND "${KEYTOOL}" -genkeypair -validity 10000
		-keystore "${KEYSTORE_FILE}" -alias "${KEYALIAS}"
		-keyalg RSA -keysize 2048
		-storepass "${STOREPASS}" -keypass "${KEYPASS}"
		-dname "CN=${COMPANY},O=Android,C=ES"
	RESULT_VARIABLE result)

if(NOT result EQUAL 0)
	message(FATAL_ERROR "keytool failed (${result}) creating ${KEYSTORE_FILE}")
endif()
