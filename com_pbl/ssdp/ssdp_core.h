#ifndef SSDP_CORE_H
#define SSDP_CORE_H

#include "ssdp_setting.h"

class Ssdp_Core : public Ssdp_Setting
{
    Q_OBJECT
    explicit Ssdp_Core(QObject *parent = nullptr);
public:
    static Ssdp_Core *bulid(QObject *parent = nullptr);

};

#endif // SSDP_CORE_H
