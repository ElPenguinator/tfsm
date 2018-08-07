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
public:
    explicit Model();
    ~Model();
    void modelCheck();

    FSM * SpecificationMachine;
    FSM * MutationMachine;

public slots:
    void importFile(QString fileName);
    void exportFile(QString fileName);
    void checkingExperiment();
    void checkingSequence();
    void generateSpecification(QTableWidget * tableTransitions, int nbOfStates, QTableWidget *tableInputs, QTableWidget *tableOutputs);

signals:
    void checkingExperimentResults(std::vector<sequence>);
    void checkingSequenceResults(sequence);
};

#endif // MODEL_H
