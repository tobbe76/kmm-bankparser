
########### next target ###############

set(kmm_sebimport_dialogs_la_SOURCES
  datesdialog.cpp
  mapaccountdialog.cpp
  kmmaccountinfowidget.cpp
  logindialog.cpp
  )

set(kmm_sebimport_dialogs_UI
  kmmaccountinfowidget.ui
  datesdialog.ui
  mapaccountdialog.ui
  logindialog.ui
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

kde4_add_ui_files(kmm_sebimport_dialogs_la_SOURCES
  ${kmm_sebimport_dialogs_UI})
kde4_add_library(kmm_sebimport_dialogs STATIC ${kmm_sebimport_dialogs_la_SOURCES})
