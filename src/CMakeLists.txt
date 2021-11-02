include_directories(${X11_INCLUDE_DIRS})
link_directories(${X11_LIBRARY_DIRS})

set(FCITX_MYMODULE_SOURCES
  matchbox-keyboard-toggler.c)

fcitx_add_addon_full(matchbox-keyboard-toggler
  SOURCES ${FCITX_MYMODULE_SOURCES}
  LINK_LIBS ${X11_LIBRARIES})
