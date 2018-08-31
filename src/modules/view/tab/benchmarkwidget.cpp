#include "benchmarkwidget.h"
#include <iostream>
using namespace std;

BenchmarkWidget::BenchmarkWidget(QWidget *parent) : QWidget(parent)
{
    buildInterface();
    relaySignals();
    this->currentFolder = QString("/benchmarks");
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

    _nbStates_tab = new QTableWidget(this);
    _nbStates_tab->clear();
    _nbStates_tab->setRowCount(1);
    _nbStates_tab->setColumnCount(1);
    _nbStates_tab->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Number of states")));
    _nbStates_tab->setColumnWidth(0, 150);
    _main_layout->addWidget(_nbStates_tab, 1, 0);

    _nbMutations_tab = new QTableWidget(this);
    _nbMutations_tab->clear();
    _nbMutations_tab->setRowCount(1);
    _nbMutations_tab->setColumnCount(1);
    _nbMutations_tab->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Number of mutations")));
    _nbMutations_tab->setColumnWidth(0, 150);
    _main_layout->addWidget(_nbMutations_tab, 1, 1);


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

void BenchmarkWidget::changeMachineType(const QString &text)
{
    //cout << "Type : " << text.toStdString() << endl;
    //Change tab if needed
    if (text == "FSM") {
        //updateTab(false, false);
    }
    else if (text == "TFSM_TO") {
        //updateTab(false, true);
    }
    else if (text == "TFSM") {
        //updateTab(true, true);
    }
    else {
        //Not implemented
    }

    emit machineTypeChanged(text);
}

void BenchmarkWidget::selectFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                 "/home",
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    currentFolder = dir;
}

void BenchmarkWidget::updateNbMutations(int row, int column)
{
    if (row == _nbMutations_tab->rowCount()-1) {
        _nbMutations_tab->setRowCount(_nbMutations_tab->rowCount()+1);
    }
    bool isValid = true;
    QTableWidgetItem * item = _nbMutations_tab->item(row, column);

    QString reason;
    QString pattern = "[0-9]+";
    QRegExp rx(pattern);
    int index = rx.indexIn(item->text());
    cout << item->text().toStdString() << endl;
    if (index == -1 || rx.matchedLength() != item->text().length()) {
        isValid = false;
        reason = "Not a number.";
    }

    if (!isValid) {
        item->setBackground(QBrush(QColor(255, 0, 0)));
        item->setToolTip(reason);
    }
    else {
        item->setBackground(QBrush(QColor(255, 255, 255)));
        item->setToolTip(QString());
    }
}

void BenchmarkWidget::updateNbStates(int row, int column)
{
    if (row == _nbStates_tab->rowCount()-1) {
        _nbStates_tab->setRowCount(_nbStates_tab->rowCount()+1);
    }
    bool isValid = true;
    QTableWidgetItem * item = _nbStates_tab->item(row, column);

    QString reason;
    QString pattern = "[0-9]+";
    QRegExp rx(pattern);
    int index = rx.indexIn(item->text());
    cout << item->text().toStdString() << endl;
    if (index == -1 || rx.matchedLength() != item->text().length()) {
        isValid = false;
        reason = "Not a number.";
    }

    if (!isValid) {
        item->setBackground(QBrush(QColor(255, 0, 0)));
        item->setToolTip(reason);
    }
    else {
        item->setBackground(QBrush(QColor(255, 255, 255)));
        item->setToolTip(QString());
    }
}

bool BenchmarkWidget::verifyNumber(QLineEdit * edit)
{
    bool isValid = true;
    QString reason;
    QString pattern = "[0-9]+";
    QRegExp rx(pattern);
    int index = rx.indexIn(edit->text());
    if (index == -1 || rx.matchedLength() != edit->text().length()) {
        isValid = false;
        reason = "Not a number.";
    }

    QPalette palette;
    if (!isValid) {
        palette.setColor(QPalette::Text,Qt::red);
        edit->setToolTip(reason);
    }
    else {
        palette.setColor(QPalette::Text,Qt::black);
        edit->setToolTip(QString());
    }
    edit->setPalette(palette);
    return isValid;
}

void BenchmarkWidget::verifyNumberMachines()
{
    bool isValid = this->verifyNumber(_nbMachines_input);
    if (isValid)
        currentNbMachines = _nbMachines_input->text().toInt();
}

void BenchmarkWidget::verifyNumberTimeouted()
{
    bool isValid = this->verifyNumber(_timeoutedValue_input);
    if (isValid)
        currentTimeoutedValue = _timeoutedValue_input->text().toInt();
}

void BenchmarkWidget::verifyNumberMax()
{
    bool isValid = this->verifyNumber(_maxTimeout_input);
    if (isValid)
        currentMaxTimeout = _maxTimeout_input->text().toInt();
}

void BenchmarkWidget::checkingExperiment()
{
    set<int> nbStates;
    for (int i=0; i<_nbStates_tab->rowCount()-1; i++) {
        QString element = _nbStates_tab->item(i, 0)->text();
        nbStates.insert(element.toInt());
    }
    set<int> nbMutations;
    for (int i=0; i<_nbMutations_tab->rowCount()-1; i++) {
        QString element = _nbMutations_tab->item(i, 0)->text();
        nbMutations.insert(element.toInt());
    }
    emit checkingExperimentBenchmark(currentFolder.toStdString(), nbStates, nbMutations, currentNbMachines, currentTimeoutedValue, currentMaxTimeout);
}

void BenchmarkWidget::checkingSequence()
{

}

void BenchmarkWidget::relaySignals()
{
    //connect(_import_button, &QPushButton::released, this, &BenchmarkWidget::switchToImport);
    /*
    connect(_graph_button, &QPushButton::released, this, &BenchmarkWidget::switchToGraph);
    connect(_gestion_button, &QPushButton::released, this, &BenchmarkWidget::switchToGestion);
    connect(_config_file_button, &QPushButton::released, this, &BenchmarkWidget::switchToConfigFile);
    */

    connect(_ce_button, &QPushButton::released, this, &BenchmarkWidget::checkingExperiment);
    connect(_cs_button, &QPushButton::released, this, &BenchmarkWidget::checkingSequence);
    connect(_select_folder_button, &QPushButton::released, this, &BenchmarkWidget::selectFolder);
    connect(_machine_type, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &BenchmarkWidget::changeMachineType);
    connect(_nbStates_tab, &QTableWidget::cellChanged, this, &BenchmarkWidget::updateNbStates);
    connect(_nbMutations_tab, &QTableWidget::cellChanged, this, &BenchmarkWidget::updateNbMutations);
    connect(_nbMachines_input, &QLineEdit::textChanged, this, &BenchmarkWidget::verifyNumberMachines);
    connect(_maxTimeout_input, &QLineEdit::textChanged, this, &BenchmarkWidget::verifyNumberMax);
    connect(_timeoutedValue_input, &QLineEdit::textChanged, this, &BenchmarkWidget::verifyNumberTimeouted);
}
