/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2017 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_wechat_ecdh.h"
#include <openssl/ec.h>
#include <openssl/bn.h>
#include <openssl/ecdh.h>
#include <openssl/ssl.h>
#include <openssl/ecdsa.h>
#include <openssl/md5.h>
#include <string.h>
#include <stdio.h>
//#include "md5.c"
int String2Buffer(unsigned char *src, int srclen, unsigned char *dest)
{
	int i = 0;
	if (srclen % 2 != 0)
		return 0;
	for (i = 0; i < srclen / 2; i++)
	{
		char tmp[3];
		tmp[0] = *(src + 2 * i);
		tmp[1] = *(src + 2 * i + 1);
		tmp[2] = 0;
		int out = 0;
		sscanf(tmp, "%x", &out);
		unsigned char ch = (unsigned char)out;
		*(dest + i) = ch;
	}
	return i;
}

int Buffer2String(unsigned char *src, int srclen, unsigned char *dest)
{
	int i;
	for (i = 0; i < srclen; i++)
	{
		char tmp[3] = {0};
		sprintf(tmp, "%x", *(src + i));
		if (strlen(tmp) == 1)
		{
			strcat((char *)dest, "0");
			strncat((char *)dest, tmp, 1);
		}
		else if (strlen(tmp) == 2)
		{
			strncat((char *)dest, tmp, 2);
		}
		else
		{
			strcat((char *)dest, "00");
		}
	}
	return i * 2;
}
static void disp(const char *str, const void *pbuf, const int size)
{
	int i = 0;
	if (str != NULL)
	{
		printf("%s:\n", str);
	}
	if (pbuf != NULL && size > 0)
	{
		for (i = 0; i < size; i++)
			printf("%02x ", *((unsigned char *)pbuf + i));
		putchar('\n');
	}
	putchar('\n');
}
void handleErrors()
{
	printf("Error occurred.\n");
}
PHP_FUNCTION(compute_key_auto)
{
	int ECDH_SIZE = 57;
	char *spubkey;
	int spublen;
	ZEND_PARSE_PARAMETERS_START(1, 3)
	Z_PARAM_STRING(spubkey, spublen)
	ZEND_PARSE_PARAMETERS_END();
	spublen = strlen(spubkey);

	zval *subarray;
	array_init(return_value);
	//unsigned char spubkey_b[57] = {0x04, 0x45, 0x80, 0x0B, 0x42, 0x32, 0x68, 0xF0, 0x81, 0x73, 0x1A, 0xE1, 0xA8, 0x73, 0xE5, 0x34, 0x24, 0x27, 0xE8, 0xDD, 0xCD, 0x76, 0x8C, 0x01, 0x7D, 0x49, 0xA6, 0xBA, 0x34, 0x02, 0x17, 0xFD, 0x12, 0x97, 0x6E, 0xBD, 0x5D, 0x0A, 0x33, 0x2E, 0x8D, 0x6F, 0xA1, 0xC4, 0x5A, 0xF9, 0x2A, 0x12, 0x74, 0x41, 0x1D, 0xE1, 0x0B, 0x95, 0x0B, 0x77, 0xDC};
	unsigned char spubkey_b[57];
	String2Buffer(spubkey,spublen,spubkey_b);

	//Client ECDH
	EC_KEY *ecdh = EC_KEY_new();
	EC_POINT *point = NULL;
	EC_POINT *point2c;
	EC_GROUP *group;
	unsigned char pubkey[ECDH_SIZE];
	unsigned char shared[ECDH_SIZE];
	int len;
	//client Generate Public
	ecdh = EC_KEY_new_by_curve_name(NID_secp224r1); //NID_secp521r1
	EC_KEY_generate_key(ecdh);
	point = (struct ec_point_st *)EC_KEY_get0_public_key(ecdh);
	group = (struct ec_group_st *)EC_KEY_get0_group(ecdh);
	if (0 == (len = EC_POINT_point2oct(group, point, POINT_CONVERSION_UNCOMPRESSED, pubkey, ECDH_SIZE, NULL)))
		handleErrors();
	printf("client_public_key_len=%d\n", len);
	disp("client_public_key", pubkey, len);
	unsigned char* cpubkey_hex=(unsigned char*)malloc(len*2);
	Buffer2String(pubkey,len,cpubkey_hex);
	/* Server ECDH */
	EC_KEY *ecdh2 = EC_KEY_new();
	EC_POINT *point2 = NULL;
	EC_POINT *pointc;
	EC_GROUP *group2;
	unsigned char pubkey2[ECDH_SIZE];
	unsigned char shared2[ECDH_SIZE];
	int len2;
	//Server Generate Public
	ecdh2 = EC_KEY_new_by_curve_name(NID_secp224r1); //NID_secp521r1
	EC_KEY_generate_key(ecdh2);
	group2 = (struct ec_group_st *)EC_KEY_get0_group(ecdh2);
	point2 = EC_POINT_new(group2);
	EC_POINT_oct2point(group2, point2, spubkey_b, 57, NULL);
	EC_KEY_set_public_key(ecdh2, point2);
	point2 = (struct ec_point_st *)EC_KEY_get0_public_key(ecdh2);
	group2 = (struct ec_group_st *)EC_KEY_get0_group(ecdh2);
	if (0 == (len2 = EC_POINT_point2oct(group2, point2, POINT_CONVERSION_UNCOMPRESSED, pubkey2, ECDH_SIZE, NULL)))
		handleErrors();
	printf("Server_public_key_len=%d\n", len);
	disp("Server_public_key", pubkey2, len2);
	/* client */
	//ComputeKey
	point2c = EC_POINT_new(group);
	EC_POINT_oct2point(group, point2c, pubkey2, ECDH_SIZE, NULL);
	if (0 != EC_POINT_cmp(group, point2, point2c, NULL))
		handleErrors();
	if (0 == (len = ECDH_compute_key(shared, ECDH_SIZE, point2c, ecdh, NULL)))
		handleErrors();
	printf("client_shared_len=%d\n", len);
	disp("client_shared", shared, len);
	char sharedKey[32];
	memset(sharedKey, 0, 16);
	MD5(shared, len, sharedKey);
	disp("client_sharedKey:", sharedKey, 16);
	/* Server */
	//ComputeKey
	pointc = EC_POINT_new(group2);
	EC_POINT_oct2point(group2, pointc, pubkey, ECDH_SIZE, NULL);

	if (0 != EC_POINT_cmp(group2, point, pointc, NULL))
		handleErrors();
	if (0 == (len2 = ECDH_compute_key(shared2, ECDH_SIZE, pointc, ecdh2, NULL)))
		handleErrors();
	printf("server_shared_len=%d\n", len2);
	disp("server_shared2", shared2, len2);
	MD5(shared, len, sharedKey+16);
	disp("server_sharedKey:", sharedKey+16, 16);
	unsigned char final_shared[16];
	memset(final_shared,0,16);
	Buffer2String(sharedKey+16,16,final_shared);
	//set return
	
	add_assoc_string(return_value, "cpubkey", cpubkey_hex);
	add_assoc_string(return_value, "sharekey", final_shared);

	/* Client */
	EC_POINT_free(pointc);
	EC_KEY_free(ecdh);
	free(cpubkey_hex);
	/* Server */
	EC_POINT_free(point2c);
	EC_KEY_free(ecdh2);
}

/* If you declare any globals in php_wechat_ecdh.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(wechat_ecdh)
*/

/* True global resources - no need for thread safety here */
static int le_wechat_ecdh;

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("wechat_ecdh.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_wechat_ecdh_globals, wechat_ecdh_globals)
    STD_PHP_INI_ENTRY("wechat_ecdh.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_wechat_ecdh_globals, wechat_ecdh_globals)
PHP_INI_END()
*/
/* }}} */

/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_wechat_ecdh_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_wechat_ecdh_compiled)
{
	char *arg = NULL;
	size_t arg_len, len;
	zend_string *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "s", &arg, &arg_len) == FAILURE)
	{
		return;
	}

	strg = strpprintf(0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "wechat_ecdh", arg);

	RETURN_STR(strg);
}
/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and
   unfold functions in source code. See the corresponding marks just before
   function definition, where the functions purpose is also documented. Please
   follow this convention for the convenience of others editing your code.
*/

/* {{{ php_wechat_ecdh_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_wechat_ecdh_init_globals(zend_wechat_ecdh_globals *wechat_ecdh_globals)
{
	wechat_ecdh_globals->global_value = 0;
	wechat_ecdh_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(wechat_ecdh)
{
	/* If you have INI entries, uncomment these lines
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(wechat_ecdh)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(wechat_ecdh)
{
#if defined(COMPILE_DL_WECHAT_ECDH) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(wechat_ecdh)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(wechat_ecdh)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "wechat_ecdh support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ wechat_ecdh_functions[]
 *
 * Every user visible function must have an entry in wechat_ecdh_functions[].
 */
const zend_function_entry wechat_ecdh_functions[] = {
	PHP_FE(confirm_wechat_ecdh_compiled, NULL) /* For testing, remove later. */
	PHP_FE(compute_key_auto, NULL)
		PHP_FE_END /* Must be the last line in wechat_ecdh_functions[] */
};
/* }}} */

/* {{{ wechat_ecdh_module_entry
 */
zend_module_entry wechat_ecdh_module_entry = {
	STANDARD_MODULE_HEADER,
	"wechat_ecdh",
	wechat_ecdh_functions,
	PHP_MINIT(wechat_ecdh),
	PHP_MSHUTDOWN(wechat_ecdh),
	PHP_RINIT(wechat_ecdh),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(wechat_ecdh), /* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(wechat_ecdh),
	PHP_WECHAT_ECDH_VERSION,
	STANDARD_MODULE_PROPERTIES};
/* }}} */

#ifdef COMPILE_DL_WECHAT_ECDH
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(wechat_ecdh)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
