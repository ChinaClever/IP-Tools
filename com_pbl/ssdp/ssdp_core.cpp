/*
 *
 *  Created on: 2022年10月1日
 *      Author: Lzy
 */
#include "ssdp_core.h"

Ssdp_Core::Ssdp_Core(QObject *parent) : Ssdp_Setting(parent)
{

}

Ssdp_Core *Ssdp_Core::bulid(QObject *parent)
{
    static Ssdp_Core* sington = nullptr;
    if(!sington) sington = new Ssdp_Core(parent);
    return sington;
}
