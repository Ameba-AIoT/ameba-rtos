/*
 * Copyright (C) 2020 Realtek Semiconductor Corp.  All Rights Reserved.
 */
 
#ifndef RTK_SVC_SETUP_H
#define RTK_SVC_SETUP_H


/* SMC function IDs for Standard Service queries */
#define RTK_SMC_TEST		0x82000001
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
#include <matter_rtk_svc_setup.h>
#endif

#endif /* BSEC_SVC_H */
