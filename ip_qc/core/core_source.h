#ifndef DEV_SOURCETHREAD_H
#define DEV_SOURCETHREAD_H

#include "core_object.h"
#include "rk_serial.h"

class Core_Source : public Core_Object
{
    Q_OBJECT
public:
    explicit Core_Source(QObject *parent = nullptr);
    bool readRk9901();

protected slots:
    void initFunSlot();

private:
    Rk_Serial *mRk;
};

#endif // DEV_SOURCETHREAD_H
