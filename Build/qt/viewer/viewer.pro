TARGET = rmlviewer
QT += widgets xml
CONFIG += object_parallel_to_source

HEADERS += \
	mainwindow.h \
	codeeditor.h highlighter.h xmlhighlighter.h \
	qtplist/PListParser.h qtplist/PListSerializer.h \
	RocketInterface/qtGraphicSystem.h RocketInterface/qtRocketFileInterface.h RocketInterface/qtRocketRenderInterface.h RocketInterface/qtRocketSystem.h
SOURCES += \
	mainwindow.cpp \
	main.cpp codeeditor.cpp highlighter.cpp xmlhighlighter.cpp \
	qtplist/PListParser.cpp qtplist/PListSerializer.cpp \
	RocketInterface/qtGraphicSystem.cpp RocketInterface/qtRocketFileInterface.cpp RocketInterface/qtRocketRenderInterface.cpp RocketInterface/qtRocketSystem.cpp

include("../config.pri")
include("../FileList.pri")

Lua_SRC_FILES += ../ext/lua.c

INCLUDEPATH += $$PWD/../ext

SOURCES += \
  $$Core_SRC_FILES \
  $$Controls_SRC_FILES \
  $$Debugger_SRC_FILES \
  $$Ext_SRC_FILES \
  $$LuaCore_SRC_FILES \
  $$LuaControls_SRC_FILES \
  \ # use included ft2 sources
  $$Ft_SRC_FILES

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
    $$Lua_SRC_FILES

ICON += res/rocket.icns
