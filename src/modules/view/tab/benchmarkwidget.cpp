#include "benchmarkwidget.h"

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

    _machine_type = new QComboBox(this);
    _machine_type->addItem(QString("FSM"));
    _machine_type->addItem(QString("TFSM_TO"));
    _machine_type->addItem(QString("TFSM"));
    _machine_type->addItem(QString("TEFSM"));
    _main_layout->addWidget(_machine_type, 0, 0);



    _nbStates_input = new QLineEdit(this);
    _main_layout->addWidget(_nbStates_input, 1, 1);
    _nbStates_label = new QLabel("Number of states :", this);
    _main_layout->addWidget(_nbStates_label, 1, 0);

    _nbMachines_input = new QLineEdit(this);
    _main_layout->addWidget(_nbMachines_input, 2, 1);
    _nbMachines_label = new QLabel("Number of machines :", this);
    _main_layout->addWidget(_nbMachines_label, 2, 0);

    _maxTimeout_input = new QLineEdit(this);
    _main_layout->addWidget(_maxTimeout_input, 3, 1);
    _maxTimeout_label = new QLabel("Maximal timeout of machines :", this);
    _main_layout->addWidget(_maxTimeout_label, 3, 0);

    _timeoutedValue_input = new QLineEdit(this);
    _main_layout->addWidget(_timeoutedValue_input, 4, 1);
    _timeoutedValue_label = new QLabel("Maximal computing time :", this);
    _main_layout->addWidget(_timeoutedValue_label, 4, 0);

    _format_type = new QComboBox(this);
    _format_type->addItem(QString("Plain"));
    _format_type->addItem(QString("CSV"));
    _main_layout->addWidget(_format_type, 5, 0);

    _select_folder_button= new QPushButton("Select a folder", this);
    _main_layout->addWidget(_select_folder_button, 5, 1);
    setButtonStyle(_select_folder_button);

    _ce_button = new QPushButton("Checking Experiment", this);
    _main_layout->addWidget(_ce_button, 6, 0);
    setButtonStyle(_ce_button);

    _cs_button = new QPushButton("Checking Sequence", this);
    _main_layout->addWidget(_cs_button, 6, 1);
    setButtonStyle(_cs_button);
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
