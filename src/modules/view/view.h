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
    void importFile();
    void exportFile();
signals:
    void importedFile(QString);
    void exportedFile(QString);
    void checkingExperiment();
    void checkingSequence();
    void checkingExperimentResults(std::vector<sequence>);
    void checkingSequenceResults(sequence);
    void machineSVGGenerated(bool);
    void generateSpecification(QTableWidget *, int, QTableWidget *, QTableWidget *);
    void generateMutation(QTableWidget *, int, QTableWidget *, QTableWidget *);
};

#endif // VIEW_H
