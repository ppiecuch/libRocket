TARGET = rmlviewer
QT += widgets xml
CONFIG += debug

HEADERS += \
	mainwindow.h settings.h documents.h actions.h tools.h \
	graphicsystem.h rockethelper.h renderingview.h activelabel.h qlogwidgets.h \
	qtplist/PListParser.h qtplist/PListSerializer.h \
	\
	../ext/rocketinterface/qtrocketfileinterface.h \
	../ext/rocketinterface/qtrocketrenderinterface.h \
	../ext/rocketinterface/qtrocketsystem.h
SOURCES += \
	main.cpp settings.cpp mainwindow.cpp documents.cpp actions.cpp tools.cpp \
	graphicsystem.cpp rockethelper.cpp renderingview.cpp activelabel.cpp qlogwidgets.cpp \
	qtplist/PListParser.cpp qtplist/PListSerializer.cpp \
	\
	../ext/rocketinterface/qtrocketfileinterface.cpp \
	../ext/rocketinterface/qtrocketrenderinterface.cpp \
	../ext/rocketinterface/qtrocketsystem.cpp

include("../config.pri")
include("../FileList.pri")

Lua_SRC_FILES += ../ext/lua/lua.c

INCLUDEPATH += \
    $$PWD/../ext/lua \
    $$PWD/../ext/rocketinterface

LIB_SOURCES += \
  $$Core_SRC_FILES \
  $$Controls_SRC_FILES \
  $$Debugger_SRC_FILES \
  $$Ext_SRC_FILES \
  $$LuaCore_SRC_FILES \
  $$LuaControls_SRC_FILES \

*xcode: SOURCES += $$LIB_SOURCES
else {
    message("*** Building library sources")
    UNITY_BUILD_FILE = $$SRC_DIR/$$TARGET-unity.cpp
    write_file($$UNITY_BUILD_FILE)
    for(f, LIB_SOURCES) {
        ff = $$absolute_path($$f)
        INC_FILE = "$${LITERAL_HASH}include \"$$ff\""
        write_file($$UNITY_BUILD_FILE, INC_FILE, append)
    }
    SOURCES += $$UNITY_BUILD_FILE
}

HEADERS += \
  $$Core_HDR_FILES \
  $$MASTER_Core_PUB_HDR_FILES \
  $$Core_PUB_HDR_FILES \
  $$Controls_HDR_FILES \
  $$MASTER_Controls_PUB_HDR_FILES \
  $$Controls_PUB_HDR_FILES \
  $$Debugger_HDR_FILES \
  $$MASTER_Debugger_PUB_HDR_FILES \
  $$Debugger_PUB_HDR_FILES \
  $$Ext_HDR_FILES \
  $$MASTER_Ext_PUB_HDR_FILES \
  $$Ext_PUB_HDR_FILES \
  $$LuaCore_HDR_FILES \
  $$LuaCore_PUB_HDR_FILES \
  $$LuaControls_HDR_FILES \
  $$LuaControls_PUB_HDR_FILES

CONFIG(with_python): {
  SOURCES += \
    $$Pycore_SRC_FILES \
    $$Pycontrols_SRC_FILES

  HEADERS += \
    $$Pycore_HDR_FILES \
    $$Pycore_PUB_HDR_FILES \
    $$Pycontrols_HDR_FILES \
    $$Pycontrols_PUB_HDR_FILES
}

SOURCES += \
    $$Ft_SRC_FILES \
    $$Lua_SRC_FILES

exists($(HOME)/Private/Projekty/0.rt/_applicationSupport/Qt/kdecode/kimageformats/kimageformats.pri): include($(HOME)/Private/Projekty/0.rt/_applicationSupport/Qt/kdecode/kimageformats/kimageformats.pri)
else:exists($(HOME)/Private/Projekty/_applicationSupport/Qt/kdecode/kimageformats/kimageformats.pri): include($(HOME)/Private/Projekty/_applicationSupport/Qt/kdecode/kimageformats/kimageformats.pri)

ICON += res/rocket.icns
RESOURCES += viewer.qrc
