add_subdirectory(RepeaterWidget)
add_subdirectory(TCPHandle)
add_subdirectory(ComToolDepend)

IF (WIN32)
    MESSAGE(STATUS "Now is windows")


ELSEIF (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS's.")

ENDIF ()