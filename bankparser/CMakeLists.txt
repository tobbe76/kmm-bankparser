cmake_minimum_required(VERSION 3.9)

project(bankparser)

find_package(ECM 1.0.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)
include(FeatureSummary)

# Find Qt modules
find_package(Qt5 CONFIG REQUIRED 
  Core    # QCommandLineParser, QStringLiteral
  Widgets # QApplication
  Gui)

find_package(Qt5WebEngineWidgets 5.8 REQUIRED)
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

find_package( Qt5Core )
find_package( Qt5PrintSupport )
find_package( Qt5Gui )
find_package( Qt5Widgets )

# Find KDE modules
find_package(KF5 5.2 REQUIRED COMPONENTS
    CoreAddons      # KAboutData
    I18n            # KLocalizedString
    WidgetsAddons   # KMessageBox
    XmlGui
)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE5_ENABLE_EXCEPTIONS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")
 
find_package(LibAlkimia5 7.0.0 REQUIRED)

add_definitions(-DHAVE_STDINT_H -DHAVE_SYS_TYPES_H -DHAVE_UNISTD_H)

include( GenerateExportHeader )
include(KDECompilerSettings)
kde_enable_exceptions()

include_directories( ${Qt5_INCLUDES}
  "/usr/include/kmymoney")

# patch the version with the version defined in the build system
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kmm_bankparser.desktop ${CMAKE_CURRENT_BINARY_DIR}/kmm_bankparser.desktop)
add_subdirectory( dialogs )
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../../mymoney
  ${CMAKE_CURRENT_SOURCE_DIR}/dialogs
  ${CMAKE_CURRENT_BINARY_DIR}/dialogs)

########### next target ###############

set(kmm_bankparser_PART_SRCS
  kmm_bankparser.cpp
  swedbankparser.cpp
  sebparser.cpp
  debugwebpage.cpp
  dateinterval.cpp
  handelsbankenparser.cpp
  norwegianparser.cpp
  bankparser.cpp
  parserfactory.cpp)

set(kmm_bankparser_PART_UI importoption.ui)

QT5_ADD_RESOURCES(lib_resources ${PROJECT_SOURCE_DIR}/files.qrc)
#add_library(kmm_bankparser_qrc ${lib_resources})

QT5_WRAP_UI(kmm_bankparser_PART_SRCS ${kmm_bankparser_PART_UI})

add_library(kmm_bankparser MODULE ${kmm_bankparser_PART_SRCS} ${lib_resources})

add_definitions(-DUSE_WEBENGINE)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/kmm_bankparser.json.in ${CMAKE_CURRENT_BINARY_DIR}/kmm_bankparser.json @ONLY)

link_directories("/usr/lib/x86_64-linux-gnu")
find_library(KMM_MYMONEY kmm_mymoney)
find_library(KMM_PLUGIN kmm_plugin)
find_library(KMM_WIDGETS kmm_widgets)
find_library(ALKIMIA5 alkimia5)

message( status "hoho " ${LIBALKIMIA_LIBRARIES} ${KMM_MYMONEY})

target_link_libraries(kmm_bankparser
                      kmm_bankparser_dialogs
                      ${KMM_MYMONEY}
                      ${KMM_WIDGETS}
                      ${KMM_PLUGIN}
		      KF5::XmlGui
		      KF5::WidgetsAddons
		      Qt5::Core
		      Qt5::Gui
		      Qt5::Widgets
		      Qt5::WebEngineWidgets
                      ${ALKIMIA5})

########### install files ###############

install(FILES kmm_bankparser.rc
        DESTINATION  ${DATA_INSTALL_DIR}/kmm_bankparser)

install(TARGETS kmm_bankparser
        DESTINATION ${PLUGIN_INSTALL_DIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kmm_bankparser.desktop
        DESTINATION ${SERVICES_INSTALL_DIR})

      
