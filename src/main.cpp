#include "modules/model/model.h"
#include "modules/view/view.h"
#include "modules/controller/controller.h"
#include <QApplication>

#include <QCommandLineParser>
#include <QCommandLineOption>
#include <QThread>
#include <QMetaType>

#include <iostream>
#include <sstream>

int main(int argc, char *argv[])
{

    /// variables declaration
    int returnCode;
    Model model;
    model.modelCheck();
    Controller controller;
    //QThread modelThread;

    // threading of the model, in order to improve user experience
    //model.moveToThread(&modelThread);
    //modelThread.start();

    QApplication app(argc, argv);

    View view;
    controller.bindEvents(view, model);
    view.show();

    // start event loop
    returnCode = app.exec();

    // properly stop the thread
    //modelThread.quit();
    //modelThread.wait();

    return returnCode;
}
