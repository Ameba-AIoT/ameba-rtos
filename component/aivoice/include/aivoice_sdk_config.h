#ifndef _AIVOICE_SDK_CONFIG_H
#define _AIVOICE_SDK_CONFIG_H

struct aivoice_sdk_config {
	int no_cmd_timeout;     /* ASR exits when no command detected during timeout.
                               Only used in full flow.
                               Unit: second. Recommend range [5, 60] */
};

/************ default configurations **********/
#define AIVOICE_SDK_CONFIG_DEFAULT() {\
    .no_cmd_timeout=10,\
};

#endif
