TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    tfsm_to.cpp \
    product_tfsm_to.cpp \
    productstate.cpp \
    alpha_product_tfsm_to.cpp \
    alphaproductstate.cpp \
    tools.cpp

HEADERS += \
    tfsm_to.h \
    structs.h \
    product_tfsm_to.h \
    productstate.h \
    alpha_product_tfsm_to.h \
    alphaproductstate.h \
    tools.h

INCLUDEPATH += /misc/home/asyd/prestadi/include/

LIBS +=  -L/misc/home/asyd/prestadi/lib/ -lcryptominisat5
