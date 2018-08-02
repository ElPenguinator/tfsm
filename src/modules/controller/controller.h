#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "../model/model.h"
#include "../view/view.h"

#include <QObject>

class Controller : public QObject
{
    Q_OBJECT

private:
    View *_view;
    Model *_model;

public:
    explicit Controller();

    /**
     * @brief bindEvents bind events for view class. This method is useful cause each view doesn't have the same events or event names
     */
    void bindEvents(View &, Model &);
    //void bindEvents(ViewGUI &, Model &){}
    //void bindEvents(ViewSH &, Model &){}

signals:
    void initialiseEvent();
};

#endif // CONTROLLER_H
