file(GLOB HEADER "*.h")
file(GLOB cpp "./qextserialport.cpp")
file(GLOB WinCPP "./qextserialport_win.cpp")
file(GLOB UnixCPP "./qextserialport_unix.cpp")
IF (WIN32)
    target_sources(${PROJECT_NAME}
            PRIVATE
            ${HEADER}
            ${cpp}
            ${WinCPP}
            )

ELSEIF (UNIX)
    target_sources(${PROJECT_NAME}
            PRIVATE
            ${HEADER}
            ${cpp}
            ${UnixCPP}
            )
ENDIF ()