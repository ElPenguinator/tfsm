#ifndef VIEW_H
#define VIEW_H
#include <QStringList>
#include <QFileDialog>

#include "mainwindow.h"

class View : public QObject
{
    Q_OBJECT
private:
    MainWindow * mainWindow;
public:
    View();
    void bindEvents();
    void show();
};

#endif // VIEW_H
