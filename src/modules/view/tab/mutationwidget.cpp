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
    _specification_machine_tab->setRowCount(3);
    _specification_machine_tab->setColumnCount(4);
    _specification_machine_tab->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("src")));
    _specification_machine_tab->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("i")));
    _specification_machine_tab->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("o")));
    _specification_machine_tab->setHorizontalHeaderItem(3, new QTableWidgetItem(QString("tgt")));

    _specification_machine_tab->setItem(0, 0, new QTableWidgetItem(QString("0")));
    _specification_machine_tab->setItem(0, 1, new QTableWidgetItem(QString("a")));
    _specification_machine_tab->setItem(0, 2, new QTableWidgetItem(QString("a")));
    _specification_machine_tab->setItem(0, 3, new QTableWidgetItem(QString("1")));

    _specification_machine_tab->setItem(1, 0, new QTableWidgetItem(QString("1")));
    _specification_machine_tab->setItem(1, 1, new QTableWidgetItem(QString("a")));
    _specification_machine_tab->setItem(1, 2, new QTableWidgetItem(QString("b")));
    _specification_machine_tab->setItem(1, 3, new QTableWidgetItem(QString("0")));

    _mutation_machine_tab->clear();
    _mutation_machine_tab->setRowCount(2);
    _mutation_machine_tab->setColumnCount(4);
    _mutation_machine_tab->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("src")));
    _mutation_machine_tab->setHorizontalHeaderItem(1, new QTableWidgetItem(QString("i")));
    _mutation_machine_tab->setHorizontalHeaderItem(2, new QTableWidgetItem(QString("o")));
    _mutation_machine_tab->setHorizontalHeaderItem(3, new QTableWidgetItem(QString("tgt")));

    _mutation_machine_tab->setItem(0, 0, new QTableWidgetItem(QString("1")));
    _mutation_machine_tab->setItem(0, 1, new QTableWidgetItem(QString("a")));
    _mutation_machine_tab->setItem(0, 2, new QTableWidgetItem(QString("a")));
    _mutation_machine_tab->setItem(0, 3, new QTableWidgetItem(QString("0")));

    _input_tab->clear();
    _input_tab->setRowCount(3);
    _input_tab->setColumnCount(1);
    _input_tab->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Inputs")));

    _input_tab->setItem(0, 0, new QTableWidgetItem(QString("a")));
    _input_tab->setItem(1, 0, new QTableWidgetItem(QString("b")));

    _output_tab->clear();
    _output_tab->setRowCount(3);
    _output_tab->setColumnCount(1);
    _output_tab->setHorizontalHeaderItem(0, new QTableWidgetItem(QString("Outputs")));

    _output_tab->setItem(0, 0, new QTableWidgetItem(QString("a")));
    _output_tab->setItem(1, 0, new QTableWidgetItem(QString("b")));
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

void MutationWidget::relaySignals()
{
    connect(_import_button, &QPushButton::released, this, &MutationWidget::importFile);
    connect(_export_button, &QPushButton::released, this, &MutationWidget::exportFile);
    connect(_ce_button, &QPushButton::released, this, &MutationWidget::checkingExperiment);
    connect(_cs_button, &QPushButton::released, this, &MutationWidget::checkingSequence);
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
