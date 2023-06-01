QT = core gui widgets

greaterThan(QT_MAJOR_VERSION, 5) {
	QT += core5compat openglwidgets
} else {
	QT += opengl
}

TARGET = ShaderPlayground
TEMPLATE = app
CONFIG += c++2a

mac* | linux*{
	CONFIG(release, debug|release):CONFIG += Release
	CONFIG(debug, debug|release):CONFIG += Debug
}

contains(QT_ARCH, x86_64) {
	ARCHITECTURE = x64
} else {
	ARCHITECTURE = x86
}

android {
	Release:OUTPUT_DIR=android/release
	Debug:OUTPUT_DIR=android/debug

} else:ios {
	Release:OUTPUT_DIR=ios/release
	Debug:OUTPUT_DIR=ios/debug

} else {
	Release:OUTPUT_DIR=release/$${ARCHITECTURE}
	Debug:OUTPUT_DIR=debug/$${ARCHITECTURE}
}

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

LIBS += -L../bin/$${OUTPUT_DIR} -lcpputils -lqtutils

HEADERS += \
	src/codeeditor/colorscheme.h \
	src/colorschemeeditor.h \
	src/errorlogsyntaxhighlighter.h \
	src/mainwindow.h \
	src/shaderrenderwidget.h \
	src/codeeditor/codeeditor.h \
	src/shadersyntaxhighlighter.h \
	src/shaderframework.h \
	src/document/csaveabledocument.h \
	src/document/cdocumenthandler.h \
	src/settings.h \
	src/codeeditor/ctextsearchwidget.h

SOURCES += \
	src/codeeditor/colorscheme.cpp \
	src/colorschemeeditor.cpp \
	src/errorlogsyntaxhighlighter.cpp \
	src/main.cpp \
	src/mainwindow.cpp \
	src/shaderrenderwidget.cpp \
	src/codeeditor/codeeditor.cpp \
	src/shadersyntaxhighlighter.cpp \
	src/shaderframework.cpp \
	src/document/csaveabledocument.cpp \
	src/document/cdocumenthandler.cpp \
	src/codeeditor/ctextsearchwidget.cpp

FORMS += \
	src/colorschemeeditor.ui \
	src/mainwindow.ui \
	src/codeeditor/ctextsearchwidget.ui

RESOURCES += \
	shaders.qrc
