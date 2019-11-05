#include "php.h"
#include "zend_llist.h"
#include "zend_instrument.h"

zend_llist *instruments;

static void (*original_zend_execute_ex) (zend_execute_data *execute_data);
void zend_instrument_execute_ex(zend_execute_data *execute_data);

struct zend_instrument_element {
    zend_instrument *instrument;
    zend_instrument_target *target;
};
typedef struct zend_instrument_element zend_instrument_element;

_Bool zend_instrument_register(zend_instrument_target *target, zend_instrument *instrument)
{
    zend_instrument_element *it = NULL;

    it = pemalloc(sizeof(zend_instrument_element), 1);
    it->instrument = instrument;
    it->target = target;

    zend_llist_add_element(instruments, it);
}

_Bool zend_instrument_unregister(zend_instrument_target *target, zend_instrument *instrument)
{
}

void zend_instrument_init()
{
    instruments = (zend_llist*) pemalloc(sizeof(zend_llist), 1);
    zend_llist_init(instruments, sizeof(zend_instrument), NULL, 1);

    original_zend_execute_ex = zend_execute_ex;
    zend_execute_ex = zend_instrument_execute_ex;
}

void zend_instrument_uninit()
{
    zend_llist_destroy(instruments);
}

/* From here there is hacks to get it working on PHP 7 inside an extension */

static zend_always_inline zend_string* tracing_get_class_name(zend_execute_data *data)
{
    zend_function *curr_func;

    curr_func = data->func;

    if (curr_func->common.scope != NULL) {
        return curr_func->common.scope->name;
    }

    return NULL;
}

static zend_always_inline zend_string* tracing_get_function_name(zend_execute_data *data)
{
    zend_function *curr_func;

    curr_func = data->func;

    if (!curr_func->common.function_name) {
        // This branch includes execution of eval and include/require(_once) calls
        // We assume it is not 1999 anymore and not much PHP code runs in the
        // body of a file and if it is, we are ok with adding it to the caller's wt.
        return NULL;
    }

    return curr_func->common.function_name;
}

void zend_instrument_execute_ex(zend_execute_data *execute_data)
{
    zend_string *class_name = tracing_get_class_name(execute_data);
    zend_string *function_name = tracing_get_function_name(execute_data);

    zend_llist_element *element = instruments->head;
    zend_instrument_element *it = NULL;
    zval context;
    ZVAL_NULL(&context);

    // yes iterating a list here is super inefficient, this code is just
    // to get the functionality working for testing purposes, implementation
    // efficiency is for later
    while (element != NULL) {
        it = (zend_instrument_element*) element->data;
        php_printf("list item %d\n", it->target->type);
        element = element->next;

        if (it->target->type != ZEND_INSTRUMENT_CALL) {
            continue;
        }

        if (!function_name) {
            continue;
        }

        if (!zend_string_equals(function_name, it->target->funcname)) {
            continue;
        }

        php_printf("HAS NSTRUMENT\n");

        // TODO: conditional class checks here
        it->instrument->begin(execute_data, &context, it->instrument->ext);
    }

    original_zend_execute_ex(execute_data);

    element = instruments->head;

    while (element != NULL) {
        it = (zend_instrument_element*) element->data;
        element = element->next;

        if (it->target->type != ZEND_INSTRUMENT_CALL) {
            continue;
        }

        if (!function_name) {
            continue;
        }

        if (!zend_string_equals(function_name, it->target->funcname)) {
            continue;
        }

        // TODO: conditional class checks here
        it->instrument->end(execute_data, &context, it->instrument->ext);

        if (EG(exception)) {
            it->instrument->exception(execute_data, &context, it->instrument->ext);
        }
    }
}
