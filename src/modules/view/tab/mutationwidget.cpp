#include "mutationwidget.h"
#include <iostream>
#include <sstream>
using namespace std;

MutationWidget::MutationWidget(QWidget *parent) : QWidget(parent)
{
    buildInterface();
    updateTab(false, false);
    relaySignals();
}

void MutationWidget::setButtonStyle(QPushButton *button)
{
    QFont font = button->font();
    font.setPointSize(12);
    font.setBold(true);
    button->setFont(font);
    button->setMaximumWidth(300);
}

void SetHeight (QPlainTextEdit* edit, int nRows)
{
    QFontMetrics m (edit -> font()) ;
    int RowHeight = m.lineSpacing() ;
    edit -> setFixedHeight  (nRows * RowHeight) ;
    edit->setMaximumWidth(400);
}

void MutationWidget::initializeMachineTransitionsTable(QTableWidget * table, bool needGuards)
{
    table->clear();
    table->setRowCount(1);
    table->setColumnCount(4 + needGuards);
    table->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("src")));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("i")));
    if (needGuards)
        table->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("g")));
    table->setHorizontalHeaderItem(2 + needGuards, new QTableWidgetItem(QString("o")));
    table->setHorizontalHeaderItem(3 + needGuards, new QTableWidgetItem(QString("tgt")));

    for (int i=0; i < 4 + needGuards; i++) {
        table->setColumnWidth(i, 50);
    }
}

void MutationWidget::initializeMachineTimeoutsTable(QTableWidget * table)
{
    table->clear();
    table->setRowCount(1);
    table->setColumnCount(3);
    table->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("src")));
    table->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("t")));
    table->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("tgt")));

    for (int i=0; i < 3; i++) {
        table->setColumnWidth(i, 50);
    }
}

void MutationWidget::initializeAlphabetTable(QTableWidget * table, QString name)
{
    table->clear();
    table->setRowCount(1);
    table->setColumnCount(1);
    table->setHorizontalHeaderItem(0, new QTableWidgetItem(name));

    table->setColumnWidth(0, 50);
}

void MutationWidget::updateTab(bool needGuards, bool needTimeouts)
{
    initializeMachineTransitionsTable(_specification_machine_transitions_tab, needGuards);
    initializeMachineTransitionsTable(_mutation_machine_transitions_tab, needGuards);

    if (needTimeouts) {
        initializeMachineTimeoutsTable(_specification_machine_timeouts_tab);
        initializeMachineTimeoutsTable(_mutation_machine_timeouts_tab);
        //_main_layout
    }

    _specification_machine_timeouts_tab->setVisible(needTimeouts);
    _mutation_machine_timeouts_tab->setVisible(needTimeouts);

    initializeAlphabetTable(_input_tab, QString("Inputs"));
    initializeAlphabetTable(_output_tab, QString("Outputs"));

}

void MutationWidget::buildInterface()
{
    // -- main layout
    _main_layout = new QGridLayout(this);
    setLayout(_main_layout);

    _test_results_group = new QGroupBox("Tests", this);
    _main_layout->addWidget(_test_results_group, 4, 0, 2, 2);
    _test_layout = new QGridLayout(_test_results_group);
    _test_results_group->setLayout(_test_layout);

    _machine_content = new QGroupBox("Machine", this);

    // ---- buttons
    _import_button = new QPushButton("Import", this);
    _main_layout->addWidget(_import_button, 5, 2);
    setButtonStyle(_import_button);

    _export_button = new QPushButton("Export", this);
    _main_layout->addWidget(_export_button, 5, 3);
    setButtonStyle(_export_button);

    _ce_button = new QPushButton("Checking Experiment", _test_results_group);
    _test_layout->addWidget(_ce_button, 0, 0);
    setButtonStyle(_ce_button);

    _cs_button = new QPushButton("Checking Sequence", _test_results_group);
    _test_layout->addWidget(_cs_button, 1, 0);
    setButtonStyle(_cs_button);

    _nbStates_input = new QLineEdit(this);
    _main_layout->addWidget(_nbStates_input, 1, 3);
    _nbStates_label = new QLabel("Number of states :", this);
    _main_layout->addWidget(_nbStates_label, 1, 2);

    _machine_type = new QComboBox(this);
    _machine_type->addItem(QString("FSM"));
    _machine_type->addItem(QString("TFSM_TO"));
    _machine_type->addItem(QString("TFSM"));
    _machine_type->addItem(QString("TEFSM"));
    _main_layout->addWidget(_machine_type, 0, 2);

    _specification_machine_transitions_tab = new QTableWidget(this);
    _main_layout->addWidget(_specification_machine_transitions_tab, 2, 2, 1
                            , 1);

    _mutation_machine_transitions_tab = new QTableWidget(this);
    _main_layout->addWidget(_mutation_machine_transitions_tab, 3, 2, 1
                            , 1);


    _specification_machine_timeouts_tab = new QTableWidget(this);
    _main_layout->addWidget(_specification_machine_timeouts_tab, 2, 3, 1
                            , 1);

    _mutation_machine_timeouts_tab = new QTableWidget(this);
    _main_layout->addWidget(_mutation_machine_timeouts_tab, 3, 3, 1
                            , 1);

    _input_tab = new QTableWidget(this);
    _main_layout->addWidget(_input_tab, 4, 2, 1
                            , 1);

    _output_tab = new QTableWidget(this);
    _main_layout->addWidget(_output_tab, 4, 3, 1
                            , 1);

    // ------ experiment description
    _test_results_label = new QLabel("Test result : ", _test_results_group);
    _test_layout->addWidget(_test_results_label, 0, 1, 1, 1);
    _test_results_text = new QPlainTextEdit(_test_results_group);
    _test_results_text->setReadOnly(true);
    _test_layout->addWidget(_test_results_text, 1, 1, 1, 1);
    SetHeight(_test_results_text, 10);

    _renderer = new SvgView();
    _renderer->openFile(QString("penguin-01.svg"));
    _renderer->setMaximumWidth(700);
    _main_layout->addWidget(_renderer, 0, 0, 4, 2);

}

bool MutationWidget::stateValidator(QTableWidget * ref, int row, int column, QString content)
{
    bool isValid = true;
    QString reason;
    QString pattern = "^[1-9][0-9]*$";
    QRegExp rx(pattern);
    int nbOfState = _nbStates_input->text().toInt();
    int index = rx.indexIn(content);
    if (index != -1  && rx.matchedLength() == content.length()) {
        int src = rx.cap(0).toInt();
        if (src > nbOfState) {
            isValid = false;
            reason = "Not an existing state.";
        }
    }
    else {
        isValid = false;
        reason = "Not a number.";
    }

    if (!isValid) {
        ref->item(row, column)->setBackground(QBrush(QColor(255, 0, 0)));
        ref->item(row, column)->setToolTip(reason);
    }
    else {
        ref->item(row, column)->setBackground(QBrush(QColor(255, 255, 255)));
        ref->item(row, column)->setToolTip(QString());
    }
    return isValid;
}

bool MutationWidget::timeoutValidator(QTableWidget * ref, int row, int column, QString content)
{
    bool isValid = true;
    QString reason;
    QString pattern = "^[1-9][0-9]*$";
    QRegExp rx(pattern);
    int index = rx.indexIn(content);
    if (content == "inf") {
        //Valid
    }
    else if (index != -1  && rx.matchedLength() == content.length()) {
        //Valid
    }
    else {
        isValid = false;
        reason = "Not a valid number.";
    }

    if (!isValid) {
        ref->item(row, column)->setBackground(QBrush(QColor(255, 0, 0)));
        ref->item(row, column)->setToolTip(reason);
    }
    else {
        ref->item(row, column)->setBackground(QBrush(QColor(255, 255, 255)));
        ref->item(row, column)->setToolTip(QString());
    }
    return isValid;
}

bool MutationWidget::guardValidator(QTableWidget * ref, int row, int column, QString content)
{
    bool isValid = true;
    QString reason;
    QString pattern = "([\\[\\(])\\s*([0-9]+)\\s*,\\s*([0-9]+|inf)\\s*([\\)\\]])";
    QRegExp rx(pattern);
    int index = rx.indexIn(content);
    //cout << "Guard : " << content.toStdString() << " " << index << " " << rx.matchedLength() << " " << content.length() << endl;
    if (index != -1  && rx.matchedLength() == content.length()) {
        //Valid
    }
    else {
        isValid = false;
        reason = "Not a guard.";
    }

    if (!isValid) {
        ref->item(row, column)->setBackground(QBrush(QColor(255, 0, 0)));
        ref->item(row, column)->setToolTip(reason);
    }
    else {
        ref->item(row, column)->setBackground(QBrush(QColor(255, 255, 255)));
        ref->item(row, column)->setToolTip(QString());
    }
    return isValid;
}

bool MutationWidget::alphabetValidator(QTableWidget * ref, QTableWidget * table, int row, int column, QString content)
{
    bool isValid = false;
    for (int i=0; i<table->rowCount()-1; i++) {
        QString element = table->item(i, 0)->text();
        if (content == element)
            isValid = true;
    }

    if (!isValid) {
        ref->item(row, column)->setBackground(QBrush(QColor(255, 0, 0)));
        ref->item(row, column)->setToolTip(QString("Not in the associated alphabet."));
    }
    else {
        ref->item(row, column)->setBackground(QBrush(QColor(255, 255, 255)));
        ref->item(row, column)->setToolTip(QString());
    }

    return isValid;
}

//If this is the last line, add a new line.
void lastLineUpdate(QTableWidget * table, int row)
{
    if (row == table->rowCount()-1) {
        table->setRowCount(table->rowCount()+1);
    }
}

//If he deletes everything on the line, delete the line.
bool emptyUpdate(QTableWidget * table, int row)
{
    bool isEmpty = true;
    for (int i=0; i<table->columnCount(); i++) {
        QTableWidgetItem * item = table->item(row, i);
        if (item && !item->text().isEmpty()) {
            isEmpty = false;
        }
    }
    if (isEmpty) {
        table->removeRow(row);
    }
    return isEmpty;
}

bool MutationWidget::validateMachine(QTableWidget * table)
{
    bool isValid = true;
    for (int row=0; row<table->rowCount()-1; row++) {
        for (int column=0; column<table->columnCount(); column++) {
            QTableWidgetItem * item = table->item(row, column);
            if (item && !item->text().isEmpty()) {
                QString content = item->text();
                QString columnHeader =  table->horizontalHeaderItem(column)->text();
                bool res = validateMachineCell(table, row, column, content, columnHeader);
                isValid = isValid && res;
            }
            else {
                isValid = false;
            }
        }
    }
    //If the cells are valid, check if the lines are empty

    if (isValid) {
        for (int row=0; row<table->rowCount()-1; row++) {
            bool isFull = true;
            for (int column=0; column<table->columnCount(); column++) {
                QTableWidgetItem * item = table->item(row, column);
                if (!item || item->text().isEmpty()) {
                    isFull = false;
                }
            }
            isValid = isValid && isFull;
        }
    }
    return isValid;
}

bool MutationWidget::validateMachineCell(QTableWidget * table, int row, int column, QString content, QString columnHeader)
{
    bool isValid;

    if (columnHeader == "src") {
        isValid = this->stateValidator(table, row, column, content);
        if (isValid)
            this->updateNbOfStates();
    }
    else if (columnHeader == "i") {
        isValid = this->alphabetValidator(table, this->_input_tab, row, column, content);
    }
    else if (columnHeader == "t") {
        isValid = this->timeoutValidator(table, row, column, content);
    }
    else if (columnHeader == "g") {
        isValid = this->guardValidator(table, row, column, content);
    }
    else if (columnHeader == "o") {
        isValid = this->alphabetValidator(table, this->_output_tab, row, column, content);
    }
    else if (columnHeader == "tgt") {
        isValid = this->stateValidator(table, row, column, content);
        if (isValid)
            this->updateNbOfStates();
    }

    return isValid;
}

void MutationWidget::sendMachine(QTableWidget * transitions, QTableWidget * timeouts, bool isSpecification)
{
    QMap<QString, QTableWidget *> map;
    map.insert(QString("inputs"), this->_input_tab);
    map.insert(QString("outputs"), this->_output_tab);
    map.insert(QString("transitions"), transitions);
    map.insert(QString("timeouts"), timeouts);
    if (isSpecification) {
        emit generateSpecification(map, this->_nbStates_input->text().toInt());
    }
    else {
        emit generateMutation(map, this->_nbStates_input->text().toInt());
    }
}

void MutationWidget::updateSpecificationTransitions(int row, int column)
{
    deactivateTabs();
    lastLineUpdate(_specification_machine_transitions_tab, row);
    emptyUpdate(_specification_machine_transitions_tab, row);
    bool isValid = validateMachine(_specification_machine_transitions_tab);
    bool timeoutsValid = validateMachine(_specification_machine_timeouts_tab);
    isValid = isValid && timeoutsValid;
    //Deterministic Check

    //Complete Check
    activateTabs();
    if (isValid) {
        sendMachine(_specification_machine_transitions_tab, _specification_machine_timeouts_tab, true);
        sendMachine(_mutation_machine_transitions_tab, _mutation_machine_timeouts_tab, false);
    }
}

void MutationWidget::updateMutationTransitions(int row, int column)
{
    deactivateTabs();
    lastLineUpdate(_mutation_machine_transitions_tab, row);
    emptyUpdate(_mutation_machine_transitions_tab, row);
    //Validator
    bool isValid = validateMachine(_mutation_machine_transitions_tab);
    bool timeoutsValid = validateMachine(_mutation_machine_timeouts_tab);
    isValid = isValid && timeoutsValid;
    activateTabs();
    if (isValid) {
        sendMachine(_mutation_machine_transitions_tab, _mutation_machine_timeouts_tab, false);
    }
}

void MutationWidget::updateSpecificationTimeouts(int row, int column)
{
    deactivateTabs();
    lastLineUpdate(_specification_machine_timeouts_tab, row);
    emptyUpdate(_specification_machine_timeouts_tab, row);

    bool isValid = validateMachine(_specification_machine_transitions_tab);
    bool timeoutsValid = validateMachine(_specification_machine_timeouts_tab);
    isValid = isValid && timeoutsValid;
    //Deterministic Check

    //Complete Check
    activateTabs();
    if (isValid) {
        sendMachine(_specification_machine_transitions_tab, _specification_machine_timeouts_tab, true);
        sendMachine(_mutation_machine_transitions_tab, _mutation_machine_timeouts_tab, false);
    }
}

void MutationWidget::updateMutationTimeouts(int row, int column)
{
    deactivateTabs();
    lastLineUpdate(_mutation_machine_timeouts_tab, row);
    emptyUpdate(_mutation_machine_timeouts_tab, row);
    //Validator
    bool isValid = validateMachine(_mutation_machine_transitions_tab);
    bool timeoutsValid = validateMachine(_mutation_machine_timeouts_tab);
    isValid = isValid && timeoutsValid;
    activateTabs();
    if (isValid) {
        sendMachine(_mutation_machine_transitions_tab, _mutation_machine_timeouts_tab, false);
    }
}

void MutationWidget::updateMachineAlphabet(QTableWidget * table, QString header, QTableWidget * tableAlphabet)
{
    deactivateTabs();
    for (int j=0; j<table->columnCount(); j++) {
        if (table->horizontalHeaderItem(j)->text() == header) {
            for (int i=0; i<table->rowCount(); i++) {
                QTableWidgetItem * item = table->item(i, j);
                if (item && !item->text().isEmpty()) {
                    this->alphabetValidator(table, tableAlphabet, i, j, table->item(i, j)->text());
                }
            }
        }
    }
    activateTabs();
}

void MutationWidget::updateInputs(int row, int column)
{
    deactivateTabs();
    lastLineUpdate(_input_tab, row);
    emptyUpdate(_input_tab, row);
    updateMachineAlphabet(_specification_machine_transitions_tab, QString("i"), _input_tab);
    updateMachineAlphabet(_mutation_machine_transitions_tab, QString("i"), _input_tab);
    activateTabs();
}

void MutationWidget::updateOutputs(int row, int column)
{
    deactivateTabs();
    lastLineUpdate(_output_tab, row);
    emptyUpdate(_output_tab, row);
    updateMachineAlphabet(_specification_machine_transitions_tab, QString("o"), _output_tab);
    updateMachineAlphabet(_mutation_machine_transitions_tab, QString("o"), _output_tab);
    activateTabs();
}

bool MutationWidget::updateMachineStates(QTableWidget * table, QString header)
{
    deactivateTabs();
    bool isValid = true;
    for (int j=0; j<table->columnCount(); j++) {
        if (table->horizontalHeaderItem(j)->text() == header) {
            for (int i=0; i<table->rowCount(); i++) {
                QTableWidgetItem * item = table->item(i, j);
                if (item && !item->text().isEmpty()) {
                    bool res = this->stateValidator(table, i, j, table->item(i, j)->text());
                    isValid = isValid && res;
                }
            }
        }
    }
    activateTabs();
    return isValid;
}

void MutationWidget::updateNbOfStates()
{
    deactivateTabs();
    bool isValid = true;
    QString reason;
    QString pattern = "[0-9]+";
    QRegExp rx(pattern);
    int index = rx.indexIn(_nbStates_input->text());
    if (index == -1 || rx.matchedLength() != _nbStates_input->text().length()) {
        isValid = false;
        reason = "Not a number.";
    }

    QPalette palette;
    if (!isValid) {
        palette.setColor(QPalette::Text,Qt::red);
        _nbStates_input->setToolTip(reason);
    }
    else {
        palette.setColor(QPalette::Text,Qt::black);
        _nbStates_input->setToolTip(QString());
    }
    _nbStates_input->setPalette(palette);
    if (isValid) {
        bool machineValid;
        updateMachineStates(_specification_machine_transitions_tab, QString("src"));
        updateMachineStates(_specification_machine_transitions_tab, QString("tgt"));

        updateMachineStates(_mutation_machine_transitions_tab, QString("src"));
        updateMachineStates(_mutation_machine_transitions_tab, QString("tgt"));
    }
    activateTabs();

}

void MutationWidget::prepareImportFile()
{
    QMap<QString, QTableWidget *> map;
    map.insert(QString("inputs"), this->_input_tab);
    map.insert(QString("outputs"), this->_output_tab);
    map.insert(QString("specificationTransitions"), _specification_machine_transitions_tab);
    map.insert(QString("specificationTimeouts"), _specification_machine_timeouts_tab);
    map.insert(QString("mutationTransitions"), _mutation_machine_transitions_tab);
    map.insert(QString("mutationTimeouts"), _mutation_machine_timeouts_tab);
    deactivateTabs();
    emit importFile(map, this->_nbStates_input);
}

void MutationWidget::activateTabs()
{
    connect(_specification_machine_transitions_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateSpecificationTransitions);
    connect(_mutation_machine_transitions_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateMutationTransitions);
    connect(_specification_machine_timeouts_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateSpecificationTimeouts);
    connect(_mutation_machine_timeouts_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateMutationTimeouts);
    connect(_input_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateInputs);
    connect(_output_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateOutputs);
    connect(_nbStates_input, &QLineEdit::textChanged, this, &MutationWidget::updateNbOfStates);
}

void MutationWidget::deactivateTabs()
{
    disconnect(_specification_machine_transitions_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateSpecificationTransitions);
    disconnect(_mutation_machine_transitions_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateMutationTransitions);
    disconnect(_specification_machine_timeouts_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateSpecificationTimeouts);
    disconnect(_mutation_machine_timeouts_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateMutationTimeouts);
    disconnect(_input_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateInputs);
    disconnect(_output_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateOutputs);
    disconnect(_nbStates_input, &QLineEdit::textChanged, this, &MutationWidget::updateNbOfStates);
}

void MutationWidget::relaySignals()
{
    connect(_import_button, &QPushButton::released, this, &MutationWidget::prepareImportFile);
    connect(_export_button, &QPushButton::released, this, &MutationWidget::exportFile);
    connect(_ce_button, &QPushButton::released, this, &MutationWidget::checkingExperiment);
    connect(_cs_button, &QPushButton::released, this, &MutationWidget::checkingSequence);
    activateTabs();
    connect(_machine_type, static_cast<void (QComboBox::*)(const QString &)>(&QComboBox::currentIndexChanged), this, &MutationWidget::changeMachineType);
    /*
    connect(_graph_button, &QPushButton::released, this, &HomepageWidget::switchToGraph);
    connect(_gestion_button, &QPushButton::released, this, &HomepageWidget::switchToGestion);
    connect(_config_file_button, &QPushButton::released, this, &HomepageWidget::switchToConfigFile);
    */
}

void MutationWidget::checkingExperimentResults(std::vector<Sequence *> E)
{
    _test_results_text->clear();

    ostringstream text;

    for (Sequence * s : E) {
        text << s->toString() << endl;
    }
    _test_results_text->appendPlainText(QString(text.str().c_str()));

}

void MutationWidget::checkingSequenceResults(Sequence * s)
{
    _test_results_text->clear();
    ostringstream text;
    text << s->toString();
    _test_results_text->appendPlainText(QString(text.str().c_str()));
}

void MutationWidget::machineSVGGenerated(bool success)
{
    if (success) {
        _renderer->openFile(QString("tmp/spec.svg"));
    }
    activateTabs();
}

void MutationWidget::changeMachineType(const QString &text)
{
    //Change tab if needed
    if (text == "FSM") {
        updateTab(false, false);
    }
    else if (text == "TFSM_TO") {
        updateTab(false, true);
    }
    else if (text == "TFSM") {
        updateTab(true, true);
    }
    else {
        //Not implemented
    }

    emit machineTypeChanged(text);
}
