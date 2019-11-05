/* hooks extension for PHP */

#ifndef PHP_HOOKS_H
# define PHP_HOOKS_H

extern zend_module_entry hooks_module_entry;
# define phpext_hooks_ptr &hooks_module_entry

# define PHP_HOOKS_VERSION "0.0.1"

# if defined(ZTS) && defined(COMPILE_DL_HOOKS)
ZEND_TSRMLS_CACHE_EXTERN()
# endif

#endif	/* PHP_HOOKS_H */

