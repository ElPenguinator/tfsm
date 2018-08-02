#ifndef HOMEPAGEWIDGET_H
#define HOMEPAGEWIDGET_H

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QTextBrowser>

class HomepageWidget : public QWidget
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
    explicit HomepageWidget(QWidget *parent = 0);
    ~HomepageWidget();

signals:
    void switchToImport();
    void switchToGraph();
    void switchToGestion();
    void switchToConfigFile();
};

#endif // HOMEPAGEWIDGET_H
