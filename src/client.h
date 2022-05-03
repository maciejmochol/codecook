#ifndef __CLIENT_H__
#define __CLIENT_H__

#include "device.h"
#include "device_list.h"

#ifdef __cplusplus
extern "C"
{
#endif

RegisteredTypeOfDevice *register_init(char *, void *);
//int register_stop(char *, void *);

#ifdef __cplusplus
}
#endif

#endif