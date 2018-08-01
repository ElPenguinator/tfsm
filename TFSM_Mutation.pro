TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += src/main.cpp \
    src/modules/model/machine/tfsm_to.cpp \
    src/modules/model/machine/product_tfsm_to.cpp \
    src/modules/model/machine/productstate.cpp \
    src/modules/model/tools.cpp \
    src/modules/model/machine/fsm.cpp \
    src/modules/model/machine/tfsm.cpp \
    src/modules/model/examplesfsm.cpp \
    src/modules/model/machine/product_tfsm.cpp \
    src/modules/model/examplesto.cpp \
    src/modules/model/checkingalgorithms_to.cpp \
    src/modules/model/checkingalgorithms.cpp \
    src/modules/model/examplestfsm.cpp \
    src/modules/module.cpp \
    src/modules/model/machine/transition.cpp \
    src/modules/model/machine/iotransition.cpp \
    src/modules/model/machine/timeouttransition.cpp

HEADERS += \
    src/modules/model/machine/tfsm_to.h \
    src/modules/model/structs.h \
    src/modules/model/machine/product_tfsm_to.h \
    src/modules/model/machine/productstate.h \
    src/modules/model/tools.h \
    src/modules/model/machine/fsm.h \
    src/modules/model/machine/tfsm.h \
    src/modules/model/machine/product_tfsm.h \
    src/modules/model/checkingalgorithms_to.h \
    src/modules/model/checkingalgorithms.h \
    src/lib/infint.h \
    src/modules/module.h \
    src/modules/model/machine/transition.h \
    src/modules/model/machine/iotransition.h \
    src/modules/model/machine/timeouttransition.h

INCLUDEPATH += /misc/home/asyd/prestadi/include/

LIBS +=  -L/misc/home/asyd/prestadi/lib/ -lcryptominisat5
