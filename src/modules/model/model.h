#ifndef MODEL_H
#define MODEL_H
#include <QObject>
#include "machine/tfsm.h"
#include "machine/distinguishingautomaton_tfsm.h"
#include <QTableWidgetItem>

class Model : public QObject
{
    Q_OBJECT
private:
    void bindEvents();
    bool showSpecification;
public:
    explicit Model();
    ~Model();
    void modelCheck();

    FSM * SpecificationMachine;
    FSM * MutationMachine;
    bool saveSVG(std::string dot);
public slots:
    void importFile(QString fileName);
    void exportFile(QString fileName);
    void checkingExperiment();
    void checkingSequence();
    void generateSpecification(QTableWidget * tableTransitions, int nbOfStates, QTableWidget *tableInputs, QTableWidget *tableOutputs);
    void generateMutation(QTableWidget * tableTransitions, int nbOfStates, QTableWidget *tableInputs, QTableWidget *tableOutputs);
signals:
    void checkingExperimentResults(std::vector<sequence>);
    void checkingSequenceResults(sequence);
    void machineSVGGenerated(bool);
};

#endif // MODEL_H
