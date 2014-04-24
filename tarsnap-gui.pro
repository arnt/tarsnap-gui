SOURCES = main.cpp backupwizard.cpp \
  tarsnappathpage.cpp accountpage.cpp backuppage.cpp \
  includepage.cpp actionpage.cpp
HEADERS = backupwizard.h \
  tarsnappathpage.h accountpage.h backuppage.h \
  includepage.h actionpage.h
TARGET = tarsnap-gui
CONFIG += qt
# to enable debugging:
CONFIG += debug
# the code uses c++11 features. to enable those using qt4:
QMAKE_CXXFLAGS += -std=c++11
# to use qt5 instead:
# CONFIG += c++11
