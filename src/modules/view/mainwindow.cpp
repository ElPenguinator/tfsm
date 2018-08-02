#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    buildInterface();
    relaySignals();
}

void MainWindow::buildInterface()
{
    // --
    _central_widget = new QWidget(this);
    setCentralWidget(_central_widget);
    _central_widget_layout = new QVBoxLayout(_central_widget);
    _central_widget->setLayout(_central_widget_layout);

    // ----
        _tabs_widget = new QTabWidget(_central_widget);
    _central_widget_layout->addWidget(_tabs_widget);

    _homepage_widget = new HomepageWidget(_tabs_widget);
    _homepageWdgetPos = _tabs_widget->addTab(_homepage_widget, QString("Homepage"));
}

void MainWindow::relaySignals()
{

}
