cmake_minimum_required(VERSION 2.8.12)

project(bankparser)

set(CMAKE_AUTOMOC TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(KDE4 REQUIRED)

add_definitions(-DUSE_WEBKIT)

if(NOT TARGET Qt4::moc)
  add_executable(Qt4::moc IMPORTED)
  set_target_properties(Qt4::moc PROPERTIES
    IMPORTED_LOCATION "${QT_MOC_EXECUTABLE}"
  )
endif(NOT TARGET Qt4::moc)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
 
find_package(LibAlkimia 4.3.2 REQUIRED)
#TODO: remove when LibAlkimia minimum version will be raised
if (NOT LIBALKIMIA_LIBRARIES AND LIBALKIMIA_LIBRARY)
  # LibAlkimia 4.3.2 does not properly define LIBALKIMIA_LIBRARIES
  find_package(GMP REQUIRED)
  set(LIBALKIMIA_LIBRARIES ${GMP_LIBRARIES} ${LIBALKIMIA_LIBRARY} )
endif()

add_definitions(-DHAVE_STDINT_H -DHAVE_SYS_TYPES_H -DHAVE_UNISTD_H)

include( KDE4Defaults )
include( MacroLibrary )
include( GenerateExportHeader )


include_directories( ${KDE4_INCLUDES}
  "/usr/local/include/kmymoney"
  "/usr/include/kmymoney")

# patch the version with the version defined in the build system
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kmm_bankparser.desktop ${CMAKE_CURRENT_BINARY_DIR}/kmm_bankparser.desktop)
add_subdirectory( dialogs )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../mymoney
  ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
  ${CMAKE_CURRENT_BINARY_DIR}/dialogs)

########### next target ###############

set(kmm_bankparser_PART_SRCS
  bankparserplugin.cpp
  swedbankparser.cpp
  sebparser.cpp
  debugwebpage.cpp
  dateinterval.cpp
  handelsbankenparser.cpp
  norwegianparser.cpp
  bankparser.cpp
  parserfactory.cpp)

set(kmm_bankparser_PART_UI importoption.ui)

QT4_ADD_RESOURCES(lib_resources ${PROJECT_SOURCE_DIR}/files.qrc)
add_library(kmm_bankparser_qrc ${lib_resources})

kde4_add_ui_files(kmm_bankparser_PART_SRCS ${kmm_bankparser_PART_UI})

kde4_add_plugin(kmm_bankparser ${kmm_bankparser_PART_SRCS})

target_link_libraries(kmm_bankparser
                      kmm_bankparser_dialogs
                      kmm_mymoney
                      kmm_widgets
		      kmm_bankparser_qrc
                      kmm_plugin
		      Qt4::QtWebKit
		      ${kmm_bankparser_qrc}
                      ${KDE4_KDEUI_LIBS}
                      ${KDE4_KIO_LIBS}
                      ${LIBALKIMIA_LIBRARIES})

########### install files ###############

install(FILES kmm_bankparser.rc
        DESTINATION  ${DATA_INSTALL_DIR}/kmm_bankparser)

install(TARGETS kmm_bankparser
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kmm_bankparser.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

      
