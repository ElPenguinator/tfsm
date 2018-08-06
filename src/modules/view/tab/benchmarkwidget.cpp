#include "benchmarkwidget.h"

#include <QLabel>

BenchmarkWidget::BenchmarkWidget(QWidget *parent) : QWidget(parent)
{
    buildInterface();
    relaySignals();
    fillInterface();
}

BenchmarkWidget::~BenchmarkWidget()
{}

void BenchmarkWidget::setButtonStyle(QPushButton *button)
{
    QFont font = button->font();
    font.setPointSize(16);
    font.setBold(true);
    button->setFont(font);
}

void BenchmarkWidget::buildInterface()
{
    // -- main layout
    _main_layout = new QGridLayout(this);
    setLayout(_main_layout);

    // ---- buttons
    _import_button = new QPushButton("Import", this);
    _main_layout->addWidget(_import_button, 0, 0);
    setButtonStyle(_import_button);


    /*
    _graph_button = new QPushButton("Graph", this);
    _main_layout->addWidget(_graph_button, 1, 0);
    setButtonStyle(_graph_button);

    _gestion_button = new QPushButton("Create an experiment", this);
    _main_layout->addWidget(_gestion_button, 2, 0);
    setButtonStyle(_gestion_button);

    _config_file_button = new QPushButton("Import a config file", this);
    _main_layout->addWidget(_config_file_button, 3, 0);
    setButtonStyle(_config_file_button);

    // ---- FAQ
    _FAQ_text_browser = new QTextBrowser(this);
    _main_layout->addWidget(_FAQ_text_browser, 0, 1, 5, 1);
    */
}

void BenchmarkWidget::fillInterface()
{
    //_FAQ_text_browser->setSource(QUrl("doc/homepage.html"));
}

void BenchmarkWidget::relaySignals()
{
    //connect(_import_button, &QPushButton::released, this, &BenchmarkWidget::switchToImport);
    /*
    connect(_graph_button, &QPushButton::released, this, &BenchmarkWidget::switchToGraph);
    connect(_gestion_button, &QPushButton::released, this, &BenchmarkWidget::switchToGestion);
    connect(_config_file_button, &QPushButton::released, this, &BenchmarkWidget::switchToConfigFile);
    */
}
