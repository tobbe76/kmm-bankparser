
########### next target ###############

set(kmm_bankparser_dialogs_la_SOURCES
  datesdialog.cpp
  mapaccountdialog.cpp
  kmmaccountinfowidget.cpp
  logindialog.cpp
  )

set(kmm_bankparser_dialogs_UI
  kmmaccountinfowidget.ui
  datesdialog.ui
  mapaccountdialog.ui
  logindialog.ui
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../)

QT5_WRAP_UI(kmm_bankparser_dialogs_la_SOURCES
  ${kmm_bankparser_dialogs_UI})
add_library(kmm_bankparser_dialogs STATIC ${kmm_bankparser_dialogs_la_SOURCES})

target_link_libraries( kmm_bankparser_dialogs
  PUBLIC
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
    Qt5::WebEngineWidgets
    KF5::XmlGui

)
