TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tfsm_to.cpp \
    product_tfsm_to.cpp \
    productstate.cpp \
    tools.cpp \
    fsm.cpp \
    product_fsm_full.cpp \
    checkingalgorithms.cpp

HEADERS += \
    tfsm_to.h \
    structs.h \
    product_tfsm_to.h \
    productstate.h \
    tools.h \
    fsm.h \
    product_fsm_full.h \
    checkingalgorithms.h

INCLUDEPATH += /misc/home/asyd/prestadi/include/

LIBS +=  -L/misc/home/asyd/prestadi/lib/ -lcryptominisat5
