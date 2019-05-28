#include "homepagewidget.h"

#include <QLabel>

HomepageWidget::HomepageWidget(QWidget *parent) : QWidget(parent)
{
    buildInterface();
    relaySignals();
    fillInterface();
}

HomepageWidget::~HomepageWidget()
{}

void HomepageWidget::setButtonStyle(QPushButton *button)
{
    QFont font = button->font();
    font.setPointSize(16);
    font.setBold(true);
    button->setFont(font);
}

void HomepageWidget::buildInterface()
{
    // -- main layout
    _main_layout = new QGridLayout(this);
    setLayout(_main_layout);

    // ---- buttons
    _import_button = new QPushButton("Import", this);
    _main_layout->addWidget(_import_button, 0, 0);
    setButtonStyle(_import_button);

}

void HomepageWidget::fillInterface()
{
    //_FAQ_text_browser->setSource(QUrl("doc/homepage.html"));
}

void HomepageWidget::relaySignals()
{
    connect(_import_button, &QPushButton::released, this, &HomepageWidget::switchToImport);
}
