add_subdirectory(RepeaterWidget)
add_subdirectory(TCPHandle)
add_subdirectory(ComToolDepend)
add_subdirectory(Highlighter)
add_subdirectory(ConfigClass)
IF (WIN32)
    MESSAGE(STATUS "Now is windows")


ELSEIF (UNIX)
    MESSAGE(STATUS "Now is UNIX-like OS's.")

ENDIF ()