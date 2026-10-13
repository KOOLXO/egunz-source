file(GLOB src
    "*.h"
    "*.cpp"
)

set(main_file "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
list(REMOVE_ITEM src "${main_file}")

add_target(NAME MatchServer_lib TYPE STATIC SOURCES "${src}")

target_include_directories(MatchServer_lib PUBLIC
	.
	../sdk/dx9/Include
	../cml/Include
	../rapidxml/Include
	../CSCommon/Include
	../RealSpace2/Include
	../sdk
	../SafeUDP/Include
	../Locator
	${LIBSQLITE3_INCLUDE_DIR}
	${LIBSODIUM_INCLUDE_DIRS}
)

target_link_libraries(MatchServer_lib PUBLIC
	msvcrt.lib
	msvcmrt.lib
	${ZLIB_LIBRARY}
	sodium
	sqlite3
	rapidxml
	ini
	cml
	CSCommon
	RealSpace2
	SafeUDP
	Locator
)

if (UNIX)
	target_link_libraries(MatchServer_lib PUBLIC dl systemd)
endif()

if (MSVC)
	ucm_add_linker_flags(/SUBSYSTEM:CONSOLE)
	target_link_libraries(MatchServer_lib PUBLIC legacy_stdio_definitions.lib)
endif()

add_target(NAME MatchServer TYPE EXECUTABLE SOURCES "${main_file}")
target_link_libraries(MatchServer PUBLIC MatchServer_lib)

# EGunZ: after a build, drop the fresh MatchServer.exe into the runnable server folder.
# EGUNZ_SERVER_DIR comes from CMakePresets.json; leave it empty to disable.
# (Stop the server before building, or Windows refuses to overwrite the running exe.)
if (EGUNZ_SERVER_DIR)
	add_custom_command(TARGET MatchServer POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:MatchServer>" "${EGUNZ_SERVER_DIR}/"
		COMMENT "EGunZ: deploying MatchServer.exe to ${EGUNZ_SERVER_DIR}")
endif()

install(
	TARGETS MatchServer RUNTIME 
	DESTINATION "server/"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_WRITE GROUP_EXECUTE WORLD_READ WORLD_EXECUTE
)