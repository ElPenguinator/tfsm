#include "machinefactory.h"
#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QtSvg>
#include <QMap>
using namespace std;
MachineFactory::MachineFactory()
{

}

set<int> MachineFactory::generateStates(int nbOfStates)
{
    set<int> S;
    for (int i=1; i<nbOfStates+1; i++) {
        S.insert(i);
    }
    return S;
}

std::set<std::string> MachineFactory::generateAlphabet(QTableWidget * table)
{
    set<string> A;
    for (int row=0; row<table->rowCount()-1; row++) {
        A.insert(table->item(row, 0)->text().toStdString());
    }
    return A;
}
void MachineFactory::fillNbStates(FSM * machine, QLineEdit *edit)
{
    edit->setText(QString::number(machine->states.size()));
}

void MachineFactory::fillInputs(FSM * machine, QMap<QString, QTableWidget *> map)
{
    QTableWidget * tableInputs = (*map.find(QString("inputs")));
    tableInputs->setRowCount(0);
    tableInputs->setRowCount(machine->inputs.size()+1);
    int row = 0;
    for (string i : machine->inputs) {
        tableInputs->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(i)));
        row++;
    }
}

void MachineFactory::fillOutputs(FSM * machine, QMap<QString, QTableWidget *> map)
{
    QTableWidget * tableOutputs = (*map.find(QString("outputs")));
    tableOutputs->setRowCount(0);
    tableOutputs->setRowCount(machine->outputs.size()+1);
    int row = 0;
    for (string o : machine->outputs) {
        tableOutputs->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(o)));
        row++;
    }
}
