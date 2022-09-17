add_subdirectory(RepeaterWidget)


IF (WIN32)
    MESSAGE(STATUS "Now is windows")


ELSEIF (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS's.")

ENDIF ()