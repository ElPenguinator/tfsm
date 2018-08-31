#ifndef BENCHMARKWIDGET_H
#define BENCHMARKWIDGET_H


#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QtSvg>
#include <QMap>
#include <set>
#include "svgview.h"

#include "../../model/structs.h"

class BenchmarkWidget : public QWidget
{
    Q_OBJECT

private:
    QGridLayout *_main_layout;
    QComboBox *_machine_type;
    QComboBox *_format_type;
    QLabel *_nbMachines_label;
    QLineEdit *_nbMachines_input;
    QLabel *_timeoutedValue_label;
    QLineEdit *_timeoutedValue_input;
    QLabel *_maxTimeout_label;
    QLineEdit *_maxTimeout_input;
    QTableWidget *_nbStates_tab;
    QTableWidget *_nbMutations_tab;
    QPushButton *_select_folder_button;
    QPushButton *_ce_button;
    QPushButton *_cs_button;
    void buildInterface();
    void relaySignals();
    void setButtonStyle(QPushButton *);
    QString currentFolder;
    int currentNbStates;
    int currentNbMachines;
    int currentTimeoutedValue;
    int currentMaxTimeout;

    bool verifyNumber(QLineEdit * edit);

public:
    explicit BenchmarkWidget(QWidget *parent = 0);
    ~BenchmarkWidget();

signals:
    void checkingExperimentBenchmark(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout);
    void checkingSequenceBenchmark(std::string folder, std::set<int> nbStates, std::set<int> nbMutations, int nbMachines, int timeoutedValue, int maxTimeout);
    void machineTypeChanged(const QString &);
public slots:
    void updateNbMutations(int row, int column);
    void updateNbStates(int row, int column);
    void selectFolder();
    void verifyNumberMachines();
    void verifyNumberTimeouted();
    void verifyNumberMax();
    void changeMachineType(const QString &text);
    void checkingExperiment();
    void checkingSequence();
};

#endif // BENCHMARKWIDGET_H
