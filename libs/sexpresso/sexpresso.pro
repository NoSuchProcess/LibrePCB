TEMPLATE = lib
TARGET = sexpresso

# Set the path for the generated library
GENERATED_DIR = ../../generated

# Use common project definitions
include(../../common.pri)

QT -= core gui widgets

CONFIG -= qt app_bundle
CONFIG += staticlib

# suppress compiler warnings
CONFIG += warn_off

SOURCES += \
    sexpresso/sexpresso.cpp \

HEADERS += \
    sexpresso/sexpresso.hpp \

