#ifndef MODEL_H
#define MODEL_H
#include <QObject>
#include "machine/tfsm.h"


class Model : public QObject
{
    Q_OBJECT
private:
    void bindEvents();
public:
    explicit Model();
    ~Model();
    void modelCheck();
};

#endif // MODEL_H
