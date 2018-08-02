#include "view.h"

View::View()
{
mainWindow = new MainWindow;
}

void View::show()
{
    mainWindow->show();
}

void View::bindEvents()
{

}
