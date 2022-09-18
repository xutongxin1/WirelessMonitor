add_subdirectory(RepeaterWidget)
add_subdirectory(qmetarial)

IF (WIN32)
    MESSAGE(STATUS "Now is windows")


ELSEIF (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS's.")

ENDIF ()