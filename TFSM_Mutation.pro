QT += core gui sql xml

#greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT += widgets
QT += svg
qtHaveModule(opengl): QT += opengl


TARGET = PDP
TEMPLATE = app

SOURCES += src/main.cpp \
    src/modules/model/machine/tfsm_to.cpp \
    src/modules/model/machine/productstate.cpp \
    src/modules/model/tools.cpp \
    src/modules/model/machine/fsm.cpp \
    src/modules/model/machine/tfsm.cpp \
    src/modules/module.cpp \
    src/modules/model/machine/transition.cpp \
    src/modules/model/machine/iotransition.cpp \
    src/modules/model/machine/timeouttransition.cpp \
    src/modules/model/machine/guardedtransition.cpp \
    src/modules/model/machine/guard.cpp \
    src/modules/view/view.cpp \
    src/modules/view/mainwindow.cpp \
    src/modules/controller/controller.cpp \
    src/modules/model/model.cpp \
    src/tmp.cpp \
    src/modules/view/tab/homepagewidget.cpp \
    src/modules/model/machine/distinguishingautomaton_fsm.cpp \
    src/modules/model/machine/distinguishingautomaton_tfsm_to.cpp \
    src/modules/model/machine/distinguishingautomaton_tfsm.cpp \
    src/modules/model/algorithm/algorithms.cpp \
    src/modules/model/algorithm/algorithms_fsm.cpp \
    src/modules/model/algorithm/algorithms_tfsm_to.cpp \
    src/modules/model/algorithm/algorithms_tfsm.cpp \
    src/modules/view/tab/mutationwidget.cpp \
    src/modules/view/tab/svgview.cpp \
    src/modules/view/tab/benchmarkwidget.cpp \
    src/modules/model/validator/validator.cpp

HEADERS += \
    src/modules/model/machine/tfsm_to.h \
    src/modules/model/structs.h \
    src/modules/model/machine/productstate.h \
    src/modules/model/tools.h \
    src/modules/model/machine/fsm.h \
    src/modules/model/machine/tfsm.h \
    src/lib/infint.h \
    src/modules/module.h \
    src/modules/model/machine/transition.h \
    src/modules/model/machine/iotransition.h \
    src/modules/model/machine/timeouttransition.h \
    src/modules/model/machine/guardedtransition.h \
    src/modules/model/machine/guard.h \
    src/modules/view/view.h \
    src/modules/view/mainwindow.h \
    src/modules/controller/controller.h \
    src/modules/model/model.h \
    src/modules/view/tab/homepagewidget.h \
    src/modules/model/machine/distinguishingautomaton_fsm.h \
    src/modules/model/machine/distinguishingautomaton_tfsm_to.h \
    src/modules/model/machine/distinguishingautomaton_tfsm.h \
    src/modules/model/algorithm/algorithms.h \
    src/modules/model/algorithm/algorithms_fsm.h \
    src/modules/model/algorithm/algorithms_tfsm_to.h \
    src/modules/model/algorithm/algorithms_tfsm.h \
    src/modules/view/tab/mutationwidget.h \
    src/modules/view/tab/svgview.h \
    src/modules/view/tab/benchmarkwidget.h \
    src/modules/model/validator/validator.h

INCLUDEPATH += /misc/home/asyd/prestadi/include/

LIBS +=  -L/misc/home/asyd/prestadi/lib/ -lcryptominisat5
