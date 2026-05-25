/*
*******************************************************************************
* Copyright(c) 2021, Realtek Semiconductor Corporation. All rights reserved.
*******************************************************************************
*/

#ifndef ZEPHYR_MSG_H
#define ZEPHYR_MSG_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {	
	unsigned char *buf;	
	unsigned int len;
}T_MSG_Z;

#ifdef __cplusplus
}
#endif

#endif