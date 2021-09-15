// wifi_connection.h
#include "mem.h"
#include "osapi.h"
#include "c_types.h"
#include "ets_sys.h"
#include "user_interface.h"
#include "espconn.h"
#include "spi_flash.h"

void ICACHE_FLASH_ATTR user_connect_ap();

void ICACHE_FLASH_ATTR user_udp_init();

void ICACHE_FLASH_ATTR uart_recvTask(os_event_t *events);
///void ICACHE_FLASH_ATTR udp_func();  timer call back fucntion do not declare here


void ICACHE_FLASH_ATTR update_message();

void ICACHE_FLASH_ATTR uart_recv_storeTask(os_event_t *events);
