
#include "xdr/types.h"
#include "xdr/xdr.h"


typedef struct _pFunc_int64_t {
	uint8_t id;
	bool_t (*pFunc)(XDR *, int64_t *);
} pFunc_int64_t;

static bool_t xdr_encode_int64_t (XDR *xdrs, int64_t *ip)
{
	int32_t t1;
	/* This must be unsigned, otherwise we get problems with sign
	   extension in the DECODE case.  */
	uint32_t t2;
	t1 = (int32_t)((*ip) >> 32);
	t2 = (int32_t)(*ip);

	return (XDR_PUTINT32(xdrs, &t1) && XDR_PUTINT32(xdrs, (int32_t *) &t2));
}

static bool_t xdr_decode_int64_t (XDR *xdrs, int64_t *ip)
{
	int32_t t1;
	/* This must be unsigned, otherwise we get problems with sign
	   extension in the DECODE case.  */
	uint32_t t2;
	if (!XDR_GETINT32(xdrs, &t1) || !XDR_GETINT32(xdrs, (int32_t *) &t2)) {
		return FALSE;
	}

	*ip = ((int64_t) t1) << 32;
	*ip |= t2;
	return TRUE;
}

pFunc_int64_t xdrFunc_int64_t [] = { { XDR_ENCODE, &xdr_encode_int64_t },
	{ XDR_DECODE, &xdr_decode_int64_t }
};

/* XDR 64bit integers */
bool_t xdr_int64_t (XDR *xdrs, int64_t *ip)
{
	enum xdr_op op = xdrs->x_op;
	uint8_t i;

	for (i = 0; i < XDR_FREE; i++) {
		if (xdrFunc_int64_t[i].id == op) {
			return xdrFunc_int64_t[i].pFunc(xdrs, ip);
		}
	}
	if (op == XDR_FREE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

typedef struct _pFunc_uint64_t {
	uint8_t id;
	bool_t (*pFunc)(XDR *, uint64_t *);
} pFunc_uint64_t;

static bool_t xdr_encode_uint64_t (XDR *xdrs, uint64_t *uip)
{
	uint32_t t1;
	uint32_t t2;

	t1 = (uint32_t)((*uip) >> 32);
	t2 = (uint32_t)(*uip);

	return (XDR_PUTINT32(xdrs, (int32_t *) &t1) &&
			XDR_PUTINT32(xdrs, (int32_t *) &t2));
}

static bool_t xdr_decode_uint64_t (XDR *xdrs, uint64_t *uip)
{
	uint32_t t1;
	uint32_t t2;

	if (!XDR_GETINT32(xdrs, (int32_t *) &t1) ||
		!XDR_GETINT32(xdrs, (int32_t *) &t2)) {
		return FALSE;
	}

	*uip = ((uint64_t) t1) << 32;
	*uip |= t2;
	return TRUE;
}

pFunc_uint64_t xdrFunc_uint64_t [] = { { XDR_ENCODE, &xdr_encode_uint64_t },
	{ XDR_DECODE, &xdr_decode_uint64_t }
};

/* XDR 64bit unsigned integers */
bool_t xdr_uint64_t (XDR *xdrs, uint64_t *uip)
{
	enum xdr_op op = xdrs->x_op;
	uint8_t i;

	for (i = 0; i < XDR_FREE; i++) {
		if (xdrFunc_uint64_t[i].id == op) {
			return xdrFunc_uint64_t[i].pFunc(xdrs, uip);
		}
	}
	if (op == XDR_FREE) {
		return TRUE;
	} else {
		return FALSE;
	}
}

/* XDR 32bit integers */
bool_t xdr_int32_t (XDR *xdrs, int32_t *lp)
{
	enum xdr_op op = xdrs->x_op;
	switch (op) {
	case XDR_ENCODE:
		return XDR_PUTINT32(xdrs, lp);
	case XDR_DECODE:
		return XDR_GETINT32(xdrs, lp);
	case XDR_FREE:
		return TRUE;
	default:
		return FALSE;
	}
}

/* XDR 32bit unsigned integers */
bool_t xdr_uint32_t (XDR *xdrs, uint32_t *ulp)
{
	enum xdr_op op = xdrs->x_op;
	switch (op) {
	case XDR_ENCODE:
		return XDR_PUTINT32(xdrs, (int32_t *) ulp);
	case XDR_DECODE:
		return XDR_GETINT32(xdrs, (int32_t *) ulp);
	case XDR_FREE:
		return TRUE;
	default:
		return FALSE;
	}
}

static bool_t xdr_encode_int16_t (XDR *xdrs, int16_t *ip)
{
	int32_t t;
	t = (int32_t) * ip;
	return XDR_PUTINT32(xdrs, &t);
}

static bool_t xdr_decode_int16_t (XDR *xdrs, int16_t *ip)
{
	int32_t t;

	if (!XDR_GETINT32(xdrs, &t)) {
		return FALSE;
	}
	*ip = (int16_t) t;
	return TRUE;
}

/* XDR 16bit integers */
bool_t xdr_int16_t (XDR *xdrs, int16_t *ip)
{
	enum xdr_op op = xdrs->x_op;

	switch (op) {
	case XDR_ENCODE:
		return xdr_encode_int16_t (xdrs, ip);
	case XDR_DECODE:
		return xdr_decode_int16_t (xdrs, ip);
	case XDR_FREE:
		return TRUE;
	default:
		return FALSE;
	}
}

static bool_t xdr_encode_uint16_t (XDR *xdrs, uint16_t *uip)
{
	uint32_t ut;
	ut = (uint32_t) * uip;
	return XDR_PUTINT32(xdrs, (int32_t *) &ut);
}

static bool_t xdr_decode_uint16_t (XDR *xdrs, uint16_t *uip)
{
	uint32_t ut;

	if (!XDR_GETINT32(xdrs, (int32_t *) &ut)) {
		return FALSE;
	}
	*uip = (uint16_t) ut;
	return TRUE;
}

/* XDR 16bit unsigned integers */
bool_t xdr_uint16_t (XDR *xdrs, uint16_t *uip)
{
	enum xdr_op op = xdrs->x_op;

	switch (op) {
	case XDR_ENCODE:
		return xdr_encode_uint16_t (xdrs, uip);
	case XDR_DECODE:
		return xdr_decode_uint16_t (xdrs, uip);
	case XDR_FREE:
		return TRUE;
	default:
		return FALSE;
	}
}

static bool_t xdr_encode_int8_t (XDR *xdrs, int8_t *ip)
{
	int32_t t;
	t = (int32_t) * ip;
	return XDR_PUTINT32(xdrs, &t);
}

static bool_t xdr_decode_int8_t (XDR *xdrs, int8_t *ip)
{
	int32_t t;
	if (!XDR_GETINT32(xdrs, &t)) {
		return FALSE;
	}
	*ip = (int8_t) t;
	return TRUE;
}

/* XDR 8bit integers */
bool_t xdr_int8_t (XDR *xdrs, int8_t *ip)
{
	enum xdr_op op = xdrs->x_op;

	switch (op) {
	case XDR_ENCODE:
		return xdr_encode_int8_t (xdrs, ip);
	case XDR_DECODE:
		return xdr_decode_int8_t (xdrs, ip);
	case XDR_FREE:
		return TRUE;
	default:
		return FALSE;
	}
}

static bool_t xdr_encode_uint8_t (XDR *xdrs, uint8_t *uip)
{
	uint32_t ut;
	ut = (uint32_t) * uip;
	return XDR_PUTINT32(xdrs, (int32_t *) &ut);
}

static bool_t xdr_decode_uint8_t (XDR *xdrs, uint8_t *uip)
{
	uint32_t ut;
	if (!XDR_GETINT32(xdrs, (int32_t *) &ut)) {
		return FALSE;
	}
	*uip = (uint8_t) ut;
	return TRUE;
}

/* XDR 8bit unsigned integers */
bool_t xdr_uint8_t (XDR *xdrs, uint8_t *uip)
{
	enum xdr_op op = xdrs->x_op;

	switch (op) {
	case XDR_ENCODE:
		return xdr_encode_uint8_t (xdrs, uip);
	case XDR_DECODE:
		return xdr_decode_uint8_t (xdrs, uip);
	case XDR_FREE:
		return TRUE;
	default:
		return FALSE;
	}
}
