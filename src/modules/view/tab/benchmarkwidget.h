#ifndef BENCHMARKWIDGET_H
#define BENCHMARKWIDGET_H


#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QTextBrowser>

class BenchmarkWidget : public QWidget
{
    Q_OBJECT

private:
    QGridLayout *_main_layout;
    QPushButton *_import_button;
    QTextBrowser *_FAQ_text_browser;

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
