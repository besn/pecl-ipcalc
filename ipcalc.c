#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include "php.h"
#include <stdio.h>
#include <string.h>
#include "php_globals.h"
#include "php_main.h"
#include "php_ini.h"
#include "zend_objects_API.h"
#include "zend_exceptions.h"
#include "zend_extensions.h"
#include "ext/standard/info.h"
#include "ext/standard/php_string.h"
#include "php_ipcalc.h"

#if HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

char *my_ntoa(unsigned long ip);

static function_entry ipcalc_functions[] = {
  PHP_FE(ipcalc, NULL)
  {NULL, NULL, NULL}
};

zend_module_entry ipcalc_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
  STANDARD_MODULE_HEADER,
#endif
  PHP_IPCALC_EXTNAME,
  ipcalc_functions,
  PHP_MINIT(ipcalc),
  PHP_MSHUTDOWN(ipcalc),
  PHP_RINIT(ipcalc),
  PHP_RSHUTDOWN(ipcalc),
  PHP_MINFO(ipcalc),
#if ZEND_MODULE_API_NO >= 20010901
  PHP_IPCALC_VERSION,
#endif
  STANDARD_MODULE_PROPERTIES
};

#ifdef COMPILE_DL_IPCALC
ZEND_GET_MODULE(ipcalc)
#endif

long ip2long(char *addr)
{
  int addr_len = sizeof(addr);
  struct in_addr ip;

  if (addr_len == 0 || inet_pton(AF_INET, addr, &ip) != 1) {
    return 0xFFFFFFFF;
  }
  return ntohl(ip.s_addr);
}

char long2ip(long unsigned int *addr_long)
{
  struct in_addr myaddr;
  char str[40];

  myaddr.s_addr = htonl(*addr_long);
  if (inet_ntop(AF_INET, &myaddr, str, sizeof(str))) {
    return str;
  }
}

PHP_FUNCTION(ipcalc)
{
  struct in_addr addr,netmask;
  unsigned long mask,network,hostmask,broadcast;
  int maskbits, cidr, i;

  char *pAddr;
  unsigned int *pAddr_len;

  if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "sl", &pAddr, &pAddr_len, &cidr) == FAILURE) {
    RETURN_NULL();
  }

  /* Convert the string ip address to network order address */
  if(!inet_aton(pAddr, &addr) ) {
    RETURN_NULL();
  }
  // long unsigned int addr_long = ip2long(addr);

  /* Check if the cidr bits are valid */
  if(cidr < 1 || cidr > 30) {
    RETURN_NULL();
  }

  /* Create the netmask from the number of bits */
  mask = 0;
  for ( i=0 ; i<cidr ; i++ )
    mask |= 1<<(31-i);
  netmask.s_addr = htonl(mask);

  array_init(return_value);
  add_assoc_string(return_value, "address", inet_ntoa(addr), 1);
  add_assoc_long(return_value, "address_long", htonl(addr.s_addr));
  add_assoc_string(return_value, "netmask", inet_ntoa(netmask), 1);
  add_assoc_long(return_value, "netmask_long", htonl(netmask.s_addr));

  network = ntohl(addr.s_addr) & ntohl(netmask.s_addr);
  add_assoc_string(return_value, "network", my_ntoa(network), 1);
  add_assoc_long(return_value, "network_long", network);

  hostmask = ~ntohl(netmask.s_addr);
  broadcast = network | hostmask;
  add_assoc_string(return_value, "broadcast", my_ntoa(broadcast), 1);
  add_assoc_long(return_value, "broadcast_long", broadcast);
  add_assoc_string(return_value, "host_first", my_ntoa(network+1), 1);
  add_assoc_long(return_value, "host_first_long", network+1);
  add_assoc_string(return_value, "host_last", my_ntoa(broadcast-1), 1);
  add_assoc_long(return_value, "host_last_long", broadcast-1);
  add_assoc_long(return_value, "host_count", broadcast-network-1);

  add_assoc_long(return_value, "cidr", cidr);
}

PHP_MINIT_FUNCTION(ipcalc)
{
  return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(ipcalc)
{
  return SUCCESS;
}

PHP_RINIT_FUNCTION(ipcalc)
{
  return SUCCESS;
}

PHP_RSHUTDOWN_FUNCTION(ipcalc)
{
  return SUCCESS;
}

PHP_MINFO_FUNCTION(ipcalc)
{
  php_info_print_table_start();
  php_info_print_table_header(2, "ipcalc support", "enabled");
  php_info_print_table_row(2, "Extension Version", PHP_IPCALC_VERSION);
  php_info_print_table_end();

  DISPLAY_INI_ENTRIES();
}

/* Convert the given ip address in native byte order to a printable string */
char *my_ntoa(unsigned long ip) {
  struct in_addr addr;
  addr.s_addr = htonl(ip);
  return inet_ntoa(addr);
}
