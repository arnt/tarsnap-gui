SOURCES =  main.cpp backupconfiguration.cpp tarsnapoptions.cpp watcher.cpp
HEADERS = backupconfiguration.h tarsnapoptions.h watcher.h
TARGET = tarsnap-gui
CONFIG += qt
# to enable debugging:
CONFIG += debug
# the code uses c++11 features. to enable those using qt4:
QMAKE_CXXFLAGS += -std=c++11
# to use qt5 instead:
# CONFIG += c++11
