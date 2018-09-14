#ifndef MODEL_H
#define MODEL_H
#include <QObject>
#include <QMap>
#include "machine/tfsm.h"
#include "machine/distinguishingautomaton_tfsm.h"
#include <QTableWidgetItem>
#include "factory/machinefactory.h"

class Model : public QObject
{
    Q_OBJECT
private:
    void bindEvents();
    bool showSpecification;
    MachineFactory * currentFactory;
    void getImportedType(std::string header);
public:
    explicit Model();
    ~Model();
    void modelCheck();

    FSM * SpecificationMachine;
    FSM * MutationMachine;
    bool saveSVG(std::string dot);

    void importMachine(std::string path);
public slots:
    void importFile(QString fileName, QMap<QString, QTableWidget *> map, QLineEdit * edit);
    void exportFile(QString fileName);
    void checkingExperiment();
    void checkingSequence();
    void checkingExperimentBenchmark(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout);
    void checkingSequenceBenchmark(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout);
    void generateSpecification(QMap<QString, QTableWidget *> map, int nbOfStates);
    void generateMutation(QMap<QString, QTableWidget *> map, int nbOfStates);
    void changeMachineType(const QString & type);
signals:
    void checkingExperimentResults(std::vector<Sequence *>);
    void checkingSequenceResults(Sequence *);
    void machineSVGGenerated(bool);
};

#endif // MODEL_H
