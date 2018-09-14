#ifndef MUTATIONWIDGET_H
#define MUTATIONWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QtSvg>
#include <QMap>

#include "svgview.h"

#include "../../model/algorithm/sequence.h"

class MutationWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MutationWidget(QWidget *parent = 0);
private:
    QGridLayout *_main_layout;
    QGridLayout *_test_layout;
    QPushButton *_import_button;
    QPushButton *_export_button;
    QPushButton *_ce_button;
    QPushButton *_cs_button;
    QPlainTextEdit *_test_results_text;
    QLabel *_test_results_label;
    QLabel *_nbStates_label;
    QGroupBox *_test_results_group;
    QGroupBox *_machine_content;
    QLineEdit *_nbStates_input;
    QComboBox *_machine_type;
    QTableWidget *_specification_machine_transitions_tab;
    QTableWidget *_mutation_machine_transitions_tab;
    QTableWidget *_specification_machine_timeouts_tab;
    QTableWidget *_mutation_machine_timeouts_tab;
    QTableWidget *_input_tab;
    QTableWidget *_output_tab;
    SvgView *_renderer;
    void buildInterface();
    void relaySignals();
    void updateTab(bool needGuards, bool needTimeouts);
    void setButtonStyle(QPushButton *);
    void initializeAlphabetTable(QTableWidget *table, QString name);
    void initializeMachineTransitionsTable(QTableWidget * table, bool needGuards);
    void initializeMachineTimeoutsTable(QTableWidget * table);

    bool validateMachineCell(QTableWidget * table, int row, int column, QString content, QString columnHeader);
    bool validateMachine(QTableWidget * table);
    bool stateValidator(QTableWidget * ref, int row, int column, QString content);
    bool timeoutValidator(QTableWidget * ref, int row, int column, QString content);
    bool guardValidator(QTableWidget * ref, int row, int column, QString content);
    bool alphabetValidator(QTableWidget * ref, QTableWidget * table, int row, int column, QString content);
    bool updateMachineStates(QTableWidget * table, QString header);
    void updateMachineAlphabet(QTableWidget * table, QString header, QTableWidget * tableAlphabet);
    void sendMachine(QTableWidget *transitions, QTableWidget *timeouts, bool isSpecification);
    void activateTabs();
    void deactivateTabs();
signals:
    void importFile(QMap<QString, QTableWidget *>, QLineEdit *);
    void exportFile();
    void checkingExperiment();
    void checkingSequence();
    void generateSpecification(QMap<QString, QTableWidget *>, int);
    void generateMutation(QMap<QString, QTableWidget *>, int);
    void machineTypeChanged(const QString &);
public slots:
    void prepareImportFile();
    void checkingExperimentResults(std::vector<Sequence *> E);
    void checkingSequenceResults(Sequence * s);
    void machineSVGGenerated(bool success);
    void updateSpecificationTransitions(int row, int column);
    void updateMutationTransitions(int row, int column);
    void updateSpecificationTimeouts(int row, int column);
    void updateMutationTimeouts(int row, int column);
    void updateInputs(int row, int column);
    void updateOutputs(int row, int column);
    void updateNbOfStates();
    void changeMachineType(const QString &text);
};

#endif // MUTATIONWIDGET_H
