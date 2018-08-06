#ifndef MODEL_H
#define MODEL_H
#include <QObject>
#include "machine/tfsm.h"
#include "machine/distinguishingautomaton_tfsm.h"

class Model : public QObject
{
    Q_OBJECT
private:
    void bindEvents();
public:
    explicit Model();
    ~Model();
    void modelCheck();

public slots:
    void importFile(QString fileName);
    void exportFile(QString fileName);
    void checkingExperiment();
    void checkingSequence();

signals:
    void checkingExperimentResults(std::vector<sequence>);
    void checkingSequenceResults(sequence);
};

#endif // MODEL_H
