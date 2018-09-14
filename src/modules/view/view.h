#ifndef VIEW_H
#define VIEW_H
#include <QStringList>
#include <QFileDialog>

#include "mainwindow.h"

class View : public QObject
{
    Q_OBJECT
private:
    MainWindow * _main_window;
public:
    View();
    void bindEvents();
    void show();
private slots:
    void importFile(QMap<QString, QTableWidget *> map, QLineEdit * edit);
    void exportFile();
signals:
    void importedFile(QString, QMap<QString, QTableWidget *>, QLineEdit *);
    void exportedFile(QString);
    void checkingExperiment();
    void checkingSequence();
    void checkingExperimentResults(std::vector<Sequence *>);
    void checkingSequenceResults(Sequence *);
    void machineSVGGenerated(bool);
    void generateSpecification(QMap<QString, QTableWidget *>, int);
    void generateMutation(QMap<QString, QTableWidget *>, int);
    void machineTypeChanged(const QString &);
    void checkingExperimentBenchmark(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout);
    void checkingSequenceBenchmark(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout);
};

#endif // VIEW_H
