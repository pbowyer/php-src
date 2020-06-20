/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) The PHP Group                                          |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Wez Furlong <wez@php.net>                                    |
  +----------------------------------------------------------------------+
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "pdo/php_pdo.h"
#include "pdo/php_pdo_driver.h"
#include "php_pdo_sqlite.h"
#include "php_pdo_sqlite_int.h"
#include "zend_exceptions.h"
#include "zend_interfaces.h"

/* Class entry pointers */
PHPAPI zend_class_entry *pdo_dbh_sqlite_ptr;

/* {{{ pdo_sqlite_functions[] */
static const zend_function_entry pdo_sqlite_functions[] = {
	PHP_FE_END
};
/* }}} */

/* {{{ pdo_sqlite_deps
 */
static const zend_module_dep pdo_sqlite_deps[] = {
	ZEND_MOD_REQUIRED("pdo")
	ZEND_MOD_END
};
/* }}} */

/* {{{ pdo_sqlite_module_entry
 */
zend_module_entry pdo_sqlite_module_entry = {
	STANDARD_MODULE_HEADER_EX, NULL,
	pdo_sqlite_deps,
	"pdo_sqlite",
	pdo_sqlite_functions,
	PHP_MINIT(pdo_sqlite),
	PHP_MSHUTDOWN(pdo_sqlite),
	NULL,
	NULL,
	PHP_MINFO(pdo_sqlite),
	PHP_PDO_SQLITE_VERSION,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#if defined(COMPILE_DL_PDO_SQLITE) || defined(COMPILE_DL_PDO_SQLITE_EXTERNAL)
ZEND_GET_MODULE(pdo_sqlite)
#endif

/* {{{ PHP_MINIT_FUNCTION */
PHP_MINIT_FUNCTION(pdo_sqlite)
{
#ifdef SQLITE_DETERMINISTIC
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_DETERMINISTIC", (zend_long)SQLITE_DETERMINISTIC);
#endif

	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_ATTR_OPEN_FLAGS", (zend_long)PDO_SQLITE_ATTR_OPEN_FLAGS);
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_OPEN_READONLY", (zend_long)SQLITE_OPEN_READONLY);
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_OPEN_READWRITE", (zend_long)SQLITE_OPEN_READWRITE);
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_OPEN_CREATE", (zend_long)SQLITE_OPEN_CREATE);
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_ATTR_READONLY_STATEMENT", (zend_long)PDO_SQLITE_ATTR_READONLY_STATEMENT);
	REGISTER_PDO_CLASS_CONST_LONG("SQLITE_ATTR_EXTENDED_RESULT_CODES", (zend_long)PDO_SQLITE_ATTR_EXTENDED_RESULT_CODES);

    zend_class_entry ce_sqlite;
    INIT_CLASS_ENTRY(ce_sqlite, "PDOSQLite", pdo_sqlite_functions);
    // The Reflection extension manges to set serialize and unserialize *before* calling
    // zend_register_internal_class(). I couldn't make that work (something to do with
    // pointers/references?) so have had to put them after.
    pdo_dbh_sqlite_ptr = zend_register_internal_class(&ce_sqlite); // @TODO Add a second parameter with name of PDO ptr
    pdo_dbh_sqlite_ptr->serialize = zend_class_serialize_deny;
    pdo_dbh_sqlite_ptr->unserialize = zend_class_unserialize_deny;
    zend_declare_property_string(pdo_dbh_sqlite_ptr, "name", sizeof("name")-1, "", ZEND_ACC_PUBLIC);

	return php_pdo_register_driver(&pdo_sqlite_driver);
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION */
PHP_MSHUTDOWN_FUNCTION(pdo_sqlite)
{
	php_pdo_unregister_driver(&pdo_sqlite_driver);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(pdo_sqlite)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "PDO Driver for SQLite 3.x", "enabled");
	php_info_print_table_row(2, "SQLite Library", sqlite3_libversion());
	php_info_print_table_end();
}
/* }}} */
