#ifndef BENCHMARKWIDGET_H
#define BENCHMARKWIDGET_H


#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QTextBrowser>
#include <QtSvg>
#include <QMap>

#include "svgview.h"

#include "../../model/structs.h"

class BenchmarkWidget : public QWidget
{
    Q_OBJECT

private:
    QGridLayout *_main_layout;
    QComboBox *_machine_type;
    QComboBox *_format_type;
    QLabel *_nbStates_label;
    QLineEdit *_nbStates_input;
    QLabel *_nbMachines_label;
    QLineEdit *_nbMachines_input;
    QLabel *_timeoutedValue_label;
    QLineEdit *_timeoutedValue_input;
    QLabel *_maxTimeout_label;
    QLineEdit *_maxTimeout_input;
    QPushButton *_select_folder_button;
    QPushButton *_ce_button;
    QPushButton *_cs_button;
    void buildInterface();
    void fillInterface();
    void relaySignals();
    void setButtonStyle(QPushButton *);

public:
    explicit BenchmarkWidget(QWidget *parent = 0);
    ~BenchmarkWidget();

signals:
    void switchToImport();
    void switchToGraph();
    void switchToGestion();
    void switchToConfigFile();
};

#endif // BENCHMARKWIDGET_H
