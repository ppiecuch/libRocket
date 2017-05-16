CONFIG(debug,debug|release): DBG = dbg-
else: DBG = rel-

OBJECTS_DIR = ../$$DBG$$[QMAKE_SPEC]/build/$$TARGET-$$TEMPLATE/obj
MOC_DIR = ../$$DBG$$[QMAKE_SPEC]/build/$$TARGET-$$TEMPLATE/ui
UI_DIR = ../$$DBG$$[QMAKE_SPEC]/build/$$TARGET-$$TEMPLATE/ui
RCC_DIR = ../$$DBG$$[QMAKE_SPEC]/build/$$TARGET-$$TEMPLATE/ui
SRC_DIR = ../$$DBG$$[QMAKE_SPEC]/build/$$TARGET-$$TEMPLATE/src

DESTDIR = ../$$DBG$$[QMAKE_SPEC]
