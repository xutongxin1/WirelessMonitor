# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\WirelessMonitor_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\WirelessMonitor_autogen.dir\\ParseCache.txt"
  "CMakeFiles\\whatever_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\whatever_autogen.dir\\ParseCache.txt"
  "WirelessMonitor_autogen"
  "thirdLib\\qmetarial\\CMakeFiles\\components_autogen.dir\\AutogenUsed.txt"
  "thirdLib\\qmetarial\\CMakeFiles\\components_autogen.dir\\ParseCache.txt"
  "thirdLib\\qmetarial\\components_autogen"
  "whatever_autogen"
  )
endif()
