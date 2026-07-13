/* component/network/ssdp/ssdp.h */
#ifndef _SSDP_H_
#define _SSDP_H_

#ifdef __cplusplus
extern "C" {
#endif

/* ── Configuration macros ───────────────────────────────────── */

#ifndef SSDP_DEVICE_DESC_ENABLED
#define SSDP_DEVICE_DESC_ENABLED    1
#endif

#ifndef SSDP_CACHE_CONTROL_MAX_AGE
#define SSDP_CACHE_CONTROL_MAX_AGE  1800
#endif

/* ── Public types ───────────────────────────────────────────── */

typedef void (*ssdp_found_cb_t)(const char *usn,
								const char *location,
								const char *st);

typedef struct {
	const char      *device_type;
	const char      *friendly_name;
	const char      *manufacturer;
	const char      *model_name;
	const char      *uuid;
	const char      *location;
	ssdp_found_cb_t  found_cb;
} ssdp_info_t;

/* ── Public functions ───────────────────────────────────────── */

int  ssdp_start(const ssdp_info_t *info);
void ssdp_stop(void);
int  ssdp_search(const char *st);

#if SSDP_DEVICE_DESC_ENABLED
const char *ssdp_get_description_xml(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SSDP_H_ */
