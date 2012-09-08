#ifndef PHP_IPCALC_H
#define PHP_IPCALC_H 1
#define PHP_IPCALC_VERSION "1.0"
#define PHP_IPCALC_EXTNAME "ipcalc"

PHP_FUNCTION(ipcalc);

PHP_MINIT_FUNCTION(ipcalc);
PHP_MSHUTDOWN_FUNCTION(ipcalc);
PHP_RINIT_FUNCTION(ipcalc);
PHP_RSHUTDOWN_FUNCTION(ipcalc);
PHP_MINFO_FUNCTION(ipcalc);

extern zend_module_entry ipcalc_module_entry;
#define phpext_ipcalc_ptr &ipcalc_module_entry

#endif /* PHP_IPCALC_H */
