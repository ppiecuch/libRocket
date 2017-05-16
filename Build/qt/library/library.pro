TARGET = Rocket
TEMPLATE = lib
CONFIG += static

include("../config.pri")

# CONFIG += with_python
# CONFIG += with_freetype

include("../FileList.pri")

Lua_SRC_FILES += ../ext/lua/lua.c

INCLUDEPATH += $$PWD/../ext/lua


LuaJit_SRC_FILES +=
LuaJit_HDR_FILES +=
LuaJit_INC_PATH +=
LuaJit_LIB +=
LuaJit_LIB_PATH +=

FT_SRC_FILES +=
FT_HDR_FILES +=
FT_INC_PATH +=
FT_LIB +=
FT_LIB_PATH +=

INCLUDEPATH += $$LuaJit_INC_PATH


LIB_SOURCES += \
  $$Controls_SRC_FILES \
  $$Debugger_SRC_FILES \
  $$Ext_SRC_FILES \
  $$LuaCore_SRC_FILES \
  $$LuaControls_SRC_FILES

message("*** Building library sources")
UNITY_BUILD_FILE = $$SRC_DIR/$$TARGET-unity.cpp
write_file($$UNITY_BUILD_FILE)
for(f, LIB_SOURCES) {
    ff = $$absolute_path($$f)
    INC_FILE = "$${LITERAL_HASH}include \"$$ff\""
    write_file($$UNITY_BUILD_FILE, INC_FILE, append)
}
SOURCES += $$UNITY_BUILD_FILE

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

