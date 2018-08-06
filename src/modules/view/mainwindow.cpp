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
    _homepageWidgetPos = _tabs_widget->addTab(_homepage_widget, QString("Homepage"));

    _mutation_widget = new MutationWidget(_tabs_widget);
    _mutationWidgetPos = _tabs_widget->addTab(_mutation_widget, QString("Mutation"));

    _benchmark_widget = new BenchmarkWidget(_tabs_widget);
    _benchmarkWidgetPos = _tabs_widget->addTab(_benchmark_widget, QString("Benchmark"));
}

void MainWindow::relaySignals()
{
    //connect(_mutation_widget, &MutationWidget::importFile, this, &MainWindow::onImportFile);
}

MutationWidget * MainWindow::getMutationWidget()
{
    return this->_mutation_widget;
}
