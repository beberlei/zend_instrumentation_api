/* hooks extension for PHP */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include "php.h"
#include "ext/standard/info.h"
#include "zend_instrument.h"
#include "php_hooks.h"

#define HOOKS_G(e) php_hooks_globals.e

typedef struct _php_hooks_globals_t {
} php_hooks_globals_t;

ZEND_TLS php_hooks_globals_t php_hooks_globals;

void hooks_instrument_printf_begin(zend_execute_data *execute_data, zval *context, void *ext)
{
    php_printf("hooking begin\n");
}

void hooks_instrument_printf_end(zend_execute_data *execute_data, zval *context, void *ext)
{
    php_printf("hooking end\n");
}

zend_instrument_target foo_target;
zend_instrument printf_instrument;

PHP_MINIT_FUNCTION(hooks)
{
    zend_instrument_init(); // this would be called by instrumentation extension / ZendEngine

    foo_target.type = ZEND_INSTRUMENT_CALL;
    foo_target.funcname = zend_new_interned_string(zend_string_init("foo", sizeof("foo"), 1)); // TODO: make it an interned string
    foo_target.classname = NULL;

    printf_instrument.begin = hooks_instrument_printf_begin;
    printf_instrument.end = hooks_instrument_printf_end;
    printf_instrument.ext = NULL;

    zend_instrument_register(&foo_target, &printf_instrument);

    return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(hooks)
{
    zend_instrument_uninit(); // this would be called by instrumentation extension / ZendEngine

    return SUCCESS;
}

PHP_RINIT_FUNCTION(hooks)
{
#if defined(ZTS) && defined(COMPILE_DL_HOOKS)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(hooks)
{
    return SUCCESS;
}

PHP_MINFO_FUNCTION(hooks)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "hooks support", "enabled");
    php_info_print_table_end();
}

zend_function_entry php_hooks_functions[] = {
    PHP_FE_END
};

zend_module_entry hooks_module_entry = {
    STANDARD_MODULE_HEADER,
    "hooks",
    php_hooks_functions,
    PHP_MINIT(hooks),
    PHP_MSHUTDOWN(hooks),
    PHP_RINIT(hooks),
    PHP_RSHUTDOWN(hooks),
    PHP_MINFO(hooks),
    PHP_HOOKS_VERSION,
    STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_HOOKS
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(hooks)
#endif
