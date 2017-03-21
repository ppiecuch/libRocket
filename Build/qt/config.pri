debug: DBG = dgb-
else: DBG = rel-

OBJECTS_DIR = ../$$DBG$$[QMAKE_SPEC]/build/$$TARGET-$$DBG$$[QMAKE_SPEC]/obj
MOC_DIR = ../$$DBG$$[QMAKE_SPEC]/build/$$TARGET-$$DBG$$[QMAKE_SPEC]/ui
UI_DIR = ../$$DBG$$[QMAKE_SPEC]/build/$$TARGET-$$DBG$$[QMAKE_SPEC]/ui
RCC_DIR = ../$$DBG$$[QMAKE_SPEC]/build/$$TARGET-$$DBG$$[QMAKE_SPEC]/ui

DESTDIR = ../$$DBG$$[QMAKE_SPEC]
