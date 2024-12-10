#ifndef RTU_MODBUS_H
#define RTU_MODBUS_H

#include "rtu_write.h"

class Rtu_Modbus : public QObject
{
    Q_OBJECT
    explicit Rtu_Modbus(QObject *parent = nullptr);
public:
    static Rtu_Modbus *bulid(QObject *parent = nullptr);
    RtuRw *get();

private:
    RtuRw *mRtuRw;
    sCfgComIt *mItem;
};

#endif // RTU_MODBUS_H
