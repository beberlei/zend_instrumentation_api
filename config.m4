dnl config.m4 for extension "hooks"

PHP_ARG_ENABLE(hooks, whether to enable hooks support,
[  --enable-hooks          Enable hooks support], no)

if test "$PHP_HOOKS" != "no"; then
  PHP_SUBST(HOOKS_SHARED_LIBADD)
  PHP_NEW_EXTENSION(hooks, hooks.c zend_instrument.c, $ext_shared,, -DZEND_ENABLE_STATIC_TSRMLS_CACHE=1)
fi
