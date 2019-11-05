#ifndef ZEND_INSTRUMENT_H
#define ZEND_INSTRUMENT_H

#include <Zend/zend.h>
#include <Zend/zend_vm_opcodes.h>

typedef void (*zend_instrument_fn)(zend_execute_data *, zval *context, void *ext);

struct zend_instrument {
	zend_instrument_fn begin;
	zend_instrument_fn end;
	zend_instrument_fn exception;
	void *ext;
};
typedef struct zend_instrument zend_instrument;

struct zend_instrument_target {
	enum { ZEND_INSTRUMENT_OPCODE, ZEND_INSTRUMENT_CALL } type;
	union {
		zend_uchar opcode;
		struct {
			/* 1. Use only funcname to trace a function
			 * 2. Use both for a method
			 * 3. Use neither for all function calls
			 */
			zend_string *classname;
			zend_string *funcname;
		};
	};
};
typedef struct zend_instrument_target zend_instrument_target;

void zend_instrument_init();
void zend_instrument_uninit();
_Bool zend_instrument_register(zend_instrument_target *target, zend_instrument *instrument);
_Bool zend_instrument_unregister(zend_instrument_target *target, zend_instrument *instrument);

#endif /* ZEND_INSTRUMENT_H */
