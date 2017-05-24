#-------------------------------------------------
#
# Project created by QtCreator 2017-05-08T10:12:21
#
#-------------------------------------------------

QT       += core gui
QT       += webkit
QT       += webkitwidgets

TARGET = testParser
TEMPLATE = app

INCLUDEPATH += ../bankparser
INCLUDEPATH += ../bankparser/dialogs

QMAKE_CXXFLAGS += -std=c++11

SOURCES += main.cpp\
  mainwindow.cpp \
  ../bankparser/bankparser.cpp \
  ../bankparser/debugwebpage.cpp \
  ../bankparser/sebparser.cpp \
  ../bankparser/swedbankparser.cpp \
  ../bankparser/handelsbankenparser.cpp \
  ../bankparser/norwegianparser.cpp \
  ../bankparser/parserfactory.cpp \
  ../bankparser/dateinterval.cpp \
  ../bankparser/dialogs/mapaccountdialog.cpp \
  ../bankparser/dialogs/datesdialog.cpp \
  ../bankparser/dialogs/kmmaccountinfowidget.cpp \
  ../bankparser/dialogs/logindialog.cpp

HEADERS  += mainwindow.h \
    ../bankparser/logininterface.h \
    ../bankparser/sebparser.h \
    ../bankparser/kmmaccountinfo.h \
    ../bankparser/bankaccountinfo.h \
    ../bankparser/swedbankparser.h \
    ../bankparser/norwegianparser.h \
    ../bankparser/handelsbankenparser.h \
    ../bankparser/bankparser.h \
    ../bankparser/dateinterval.h \
    ../bankparser/logininterface.h \
    ../bankparser/dialogs/logindialog.h \
    ../bankparser/dialogs/datesdialog.h \
    ../bankparser/dialogs/kmmaccountinfowidget.h \
    ../bankparser/debugwebpage.h \
    ../bankparser/parserfactory.h \
    ../bankparser/dialogs/mapaccountdialog.h \
    kmymoneyplugin.h



FORMS    += mainwindow.ui
 FORMS   += ../bankparser/dialogs/kmmaccountinfowidget.ui
FORMS    += ../bankparser/dialogs/logindialog.ui
FORMS    += ../bankparser/dialogs/mapaccountdialog.ui
FORMS    += ../bankparser/dialogs/datesdialog.ui
