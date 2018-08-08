#include "mutationwidget.h"
#include <iostream>
using namespace std;

MutationWidget::MutationWidget(QWidget *parent) : QWidget(parent)
{
    buildInterface();
    updateTab();
    relaySignals();
    fillInterface();
}

void MutationWidget::setButtonStyle(QPushButton *button)
{
    QFont font = button->font();
    font.setPointSize(12);
    font.setBold(true);
    button->setFont(font);
}

void SetHeight (QPlainTextEdit* edit, int nRows)
{
    QFontMetrics m (edit -> font()) ;
    int RowHeight = m.lineSpacing() ;
    edit -> setFixedHeight  (nRows * RowHeight) ;
}

void MutationWidget::updateTab()
{
    _specification_machine_tab->clear();
    _specification_machine_tab->setRowCount(1);
    _specification_machine_tab->setColumnCount(4);
    _specification_machine_tab->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("src")));
    _specification_machine_tab->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("i")));
    _specification_machine_tab->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("o")));
    _specification_machine_tab->setHorizontalHeaderItem(3, new QTableWidgetItem(QString("tgt")));

    _mutation_machine_tab->clear();
    _mutation_machine_tab->setRowCount(1);
    _mutation_machine_tab->setColumnCount(4);
    _mutation_machine_tab->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("src")));
    _mutation_machine_tab->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("i")));
    _mutation_machine_tab->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("o")));
    _mutation_machine_tab->setHorizontalHeaderItem(3, new QTableWidgetItem(QString("tgt")));

    _input_tab->clear();
    _input_tab->setRowCount(1);
    _input_tab->setColumnCount(1);
    _input_tab->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Inputs")));

    _output_tab->clear();
    _output_tab->setRowCount(1);
    _output_tab->setColumnCount(1);
    _output_tab->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Outputs")));

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

    _specification_machine_tab = new QTableWidget(this);
    _main_layout->addWidget(_specification_machine_tab, 2, 2, 1
                            , 2);

    _mutation_machine_tab = new QTableWidget(this);
    _main_layout->addWidget(_mutation_machine_tab, 3, 2, 1
                            , 2);

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
    _main_layout->addWidget(_renderer, 0, 0, 4, 2);

}

void MutationWidget::fillInterface()
{
    //_FAQ_text_browser->setSource(QUrl("doc/homepage.html"));
}

bool MutationWidget::stateValidator(QTableWidget * ref, int row, int column, QString content)
{
    bool isValid = true;
    QString reason;
    QString pattern = "[0-9]+";
    QRegExp rx(pattern);
    int nbOfState = _nbStates_input->text().toInt();
    int index = rx.indexIn(content);
    if (index != -1  && rx.matchedLength() == _nbStates_input->text().length()) {
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
    else if (columnHeader == "g") {
        isValid = this->stateValidator(table, row, column, content);
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

void MutationWidget::updateSpecification(int row, int column)
{

    lastLineUpdate(_specification_machine_tab, row);
    bool isEmpty = emptyUpdate(_specification_machine_tab, row);

    if (!isEmpty) {
        bool isValid = validateMachine(_specification_machine_tab);
        //Deterministic Check

        //Complete Check

        if (isValid) {
            emit generateSpecification(_specification_machine_tab, this->_nbStates_input->text().toInt(), this->_input_tab, this->_output_tab);
            emit generateMutation(_mutation_machine_tab, this->_nbStates_input->text().toInt(), this->_input_tab, this->_output_tab);
        }
    }
}

void MutationWidget::updateMutation(int row, int column)
{
    lastLineUpdate(_mutation_machine_tab, row);
    bool isEmpty = emptyUpdate(_mutation_machine_tab, row);
    if (!isEmpty) {
        //Validator
        bool isValid = validateMachine(_mutation_machine_tab);
        if (isValid) {
            emit generateMutation(_mutation_machine_tab, this->_nbStates_input->text().toInt(), this->_input_tab, this->_output_tab);
        }
    }
}

void MutationWidget::updateMachineAlphabet(QTableWidget * table, QString header, QTableWidget * tableAlphabet)
{
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
}

void MutationWidget::updateInputs(int row, int column)
{
    lastLineUpdate(_input_tab, row);
    bool isEmpty = emptyUpdate(_input_tab, row);
    if (!isEmpty) {
        updateMachineAlphabet(_specification_machine_tab, QString("i"), _input_tab);
        updateMachineAlphabet(_mutation_machine_tab, QString("i"), _input_tab);
    }
}

void MutationWidget::updateOutputs(int row, int column)
{
    lastLineUpdate(_output_tab, row);
    bool isEmpty = emptyUpdate(_output_tab, row);
    if (!isEmpty) {
        updateMachineAlphabet(_specification_machine_tab, QString("o"), _output_tab);
        updateMachineAlphabet(_mutation_machine_tab, QString("o"), _output_tab);
    }
}

bool MutationWidget::updateMachineStates(QTableWidget * table, QString header)
{
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
    return isValid;
}

void MutationWidget::updateNbOfStates()
{
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
        updateMachineStates(_specification_machine_tab, QString("src"));
        updateMachineStates(_specification_machine_tab, QString("tgt"));

        updateMachineStates(_mutation_machine_tab, QString("src"));
        updateMachineStates(_mutation_machine_tab, QString("tgt"));
    }


}

void MutationWidget::relaySignals()
{
    connect(_import_button, &QPushButton::released, this, &MutationWidget::importFile);
    connect(_export_button, &QPushButton::released, this, &MutationWidget::exportFile);
    connect(_ce_button, &QPushButton::released, this, &MutationWidget::checkingExperiment);
    connect(_cs_button, &QPushButton::released, this, &MutationWidget::checkingSequence);
    connect(_specification_machine_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateSpecification);
    connect(_mutation_machine_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateMutation);
    connect(_input_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateInputs);
    connect(_output_tab, &QTableWidget::cellChanged, this, &MutationWidget::updateOutputs);
    connect(_nbStates_input, &QLineEdit::textChanged, this, &MutationWidget::updateNbOfStates);
    /*
    connect(_graph_button, &QPushButton::released, this, &HomepageWidget::switchToGraph);
    connect(_gestion_button, &QPushButton::released, this, &HomepageWidget::switchToGestion);
    connect(_config_file_button, &QPushButton::released, this, &HomepageWidget::switchToConfigFile);
    */
}

void MutationWidget::checkingExperimentResults(std::vector<sequence> E)
{
    _test_results_text->clear();
    std::string text = "";
    for (sequence s : E) {
        for (ts timeState : s) {
            text += timeState.first;
        }
        text += "\n";
    }
    _test_results_text->appendPlainText(QString(text.c_str()));

}

void MutationWidget::checkingSequenceResults(sequence s)
{
    _test_results_text->clear();
    std::string text = "";
    for (ts timeState : s) {
        text += timeState.first;
    }
    _test_results_text->appendPlainText(QString(text.c_str()));
}

void MutationWidget::machineSVGGenerated(bool success)
{
    if (success) {
        _renderer->openFile(QString("tmp/spec.svg"));
    }
}
