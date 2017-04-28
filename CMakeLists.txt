CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(myble)

INCLUDE(FindPkgConfig)

pkg_check_modules(pkgs REQUIRED
  glib-2.0
  gio-2.0
  gio-unix-2.0
  gthread-2.0
  )

FOREACH(flag ${pkgs_CFLAGS})
  SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g -Werror -Wall -Wextra -fvisibility=hidden -fPIC -fdata-sections -ffunction-sections")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wmissing-prototypes -Wswitch-default -Wswitch-enum -Wunreachable-code -Wpointer-arith -Waggregate-return")
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wno-unused-parameter -Wno-missing-field-initializers -Wdeclaration-after-statement -Wmissing-declarations -Wcast-align")

INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/src/ ${CMAKE_BINARY_DIR})
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} ")
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")
SET(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

SET(SRCS
  main.c
  src/myb_dbus.c
  src/myb_leadv_object.c
  src/myb_eddystone.c
)

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})
