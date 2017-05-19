QT = core gui widgets opengl

TARGET = ShaderPlayground
TEMPLATE = app
CONFIG += c++14

mac* | linux*{
	CONFIG(release, debug|release):CONFIG += Release
	CONFIG(debug, debug|release):CONFIG += Debug
}

Release:OUTPUT_DIR=release
Debug:OUTPUT_DIR=debug

DESTDIR  = ../bin/$${OUTPUT_DIR}
OBJECTS_DIR = ../build/$${OUTPUT_DIR}/$${TARGET}
MOC_DIR     = ../build/$${OUTPUT_DIR}/$${TARGET}
UI_DIR      = ../build/$${OUTPUT_DIR}/$${TARGET}
RCC_DIR     = ../build/$${OUTPUT_DIR}/$${TARGET}

win*{
	QMAKE_CXXFLAGS += /MP /wd4251
	QMAKE_CXXFLAGS_WARN_ON = /W4
	DEFINES += WIN32_LEAN_AND_MEAN NOMINMAX

	!*msvc2013*:QMAKE_LFLAGS += /DEBUG:FASTLINK

	Debug:QMAKE_LFLAGS += /INCREMENTAL
	Release:QMAKE_LFLAGS += /OPT:REF /OPT:ICF

	LIBS += -lopengl32
}

linux*|mac*{
	QMAKE_CXXFLAGS_WARN_ON = -Wall -Wno-c++11-extensions -Wno-local-type-template-args -Wno-deprecated-register

	Release:DEFINES += NDEBUG=1
	Debug:DEFINES += _DEBUG
}

win32*:!*msvc2012:*msvc* {
	QMAKE_CXXFLAGS += /FS
}

INCLUDEPATH += \
	../cpputils/ \
	../cpp-template-utils/ \
	../qtutils/

LIBS += -L$${DESTDIR} -lcpputils -lqtutils

HEADERS += \
	src/mainwindow.h \
	src/shaderrenderwidget.h \
	src/codeeditor.h \
	src/shadersyntaxhighlighter.h \
	src/shaderframework.h \
    src/document/csaveabledocument.h \
    src/document/cdocumenthandler.h \
    src/settings.h

SOURCES += \
	src/main.cpp \
	src/mainwindow.cpp \
	src/shaderrenderwidget.cpp \
	src/codeeditor.cpp \
	src/shadersyntaxhighlighter.cpp \
	src/shaderframework.cpp \
    src/document/csaveabledocument.cpp \
    src/document/cdocumenthandler.cpp

FORMS += \
	src/mainwindow.ui

RESOURCES += \
	shaders.qrc

DISTFILES +=
