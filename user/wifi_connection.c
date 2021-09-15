//// tcp-udp-soft apmode-station mode library 
/*
deniz pelen
29 August 2020 v1.0
*/

#include "wifi_connection.h"
  
#include "ets_sys.h"
#include "mem.h"
#include "osapi.h"
#include "spi_flash.h"

#include "c_types.h"
#include "ets_sys.h"
#include "user_interface.h"
#include "espconn.h"
#include "driver/uart.h"
#include "driver/uart_register.h"


#define SSID "deniz"
#define PASSWORD "admin1234"// random password trust me

#define UDP_PORT 1234
#define TCP_PORT 1235

////// for the udp

static uint8_t ipAddr[] = { 192, 168, 43, 1 };// ip address of the receiver
static struct espconn espConn ;
static  esp_udp espProto ;
static os_timer_t udpServerTimer;
//// for the tcp 
static struct espconn tcpConn;
static esp_tcp tcpProto;



///// for the tcp 
static esp_tcp tcpProto;




static uint8 message[128] = "merhaba";
static uint8 msg[5] = "12345";

uint8_t conn_status;
/*************************************
connect to existing defined AP
no error handling is implemented yet  

**************************************/
void ICACHE_FLASH_ATTR user_connect_ap(){
	char ssid[32] = SSID;
	char password[64] = PASSWORD;
	struct station_config stationConf;
	bool isConnected = false ;
	struct ip_info my_ip;
	
	wifi_set_opmode_current(STATION_MODE);
	//station_config.bssid_set = 0;   /// do not check the mac addresses of AP
	os_memcpy(&stationConf.ssid, ssid, 32);
	os_memcpy(&stationConf.password, password, 64);
	
	wifi_set_opmode_current(STATION_MODE);
	//wifi_station_disconnect();
	wifi_station_set_config(&stationConf);// only when station is enabled
	
	
	isConnected = wifi_station_connect();
	if(isConnected == true)
	{
		os_printf("connected asdfas. \n");
		//wifi_get_ip_info(STATION_IF, &my_ip );
		//os_printf(IP2STR(&my_ip.ip));
	}
	else{
		os_printf("connection failed xx.\n ");
	}
	conn_status = wifi_station_get_connect_status();
	os_printf("%d helll\n", conn_status);
	

}
/*
void ICACHE_FLASH_ATTR user_softAp(){
	struct softap_config softapConf;
	
	os_memcpy(&softapConf.ssid, ssid, 32);
	os_memcpy(&softapConf.password, password, 64);
	// set the current opmode 
	wifi_set_opmode_current(SOFTAP_MODE);
	// configure the mode -- softap_config for softap, for the station mode station_config
	wifi_softap_set_config(&softap_config);
	// call the set_config 
	// 
	
}*/


static void ICACHE_FLASH_ATTR udp_func(void)
{
	int8_t errEsp ;
	uint8  rssiESP = 0;
	// send the rssi value
	
	rssiESP = (uint8)wifi_station_get_rssi();
	rssiESP = rssiESP * (-1);
	
	msg[0] = (uint8)(rssiESP / 100);
	rssiESP = rssiESP - msg[0]*100;
	msg[1] = (uint8)(rssiESP / 10);
	rssiESP = rssiESP - msg[1]*10;
	msg[2] = (uint8)(rssiESP);
	os_printf("%d %d %d \n", msg[0], msg[1], msg[2]);
	
	//os_memcpy(message, msg, 5);
	
	//os_memcpy(&espProto.remote_ip, ipAddr, 4);
	//espProto.remote_port = UDP_PORT;
	
	errEsp = espconn_send(&espConn, msg,3); //os_strlen((char*)message )
	//errEsp = espconn_send(&espConn, &rssiESP, 1);
	if (errEsp != 0)
	{
		conn_status = wifi_station_get_connect_status();
		os_printf("espconn send err %d \n", errEsp);
		os_printf("%d   \n", conn_status);
	}
	
}

void ICACHE_FLASH_ATTR tcp_connect_cb(void * arg)
{
	// this ia a call back function, it is called when a connection occurs
	// parameter is the espconn, to use send or receive data
	struct espconn * cb_tcpConn = (struct espconn *)arg;
	os_printf("TCP connection from "IPSTR":: %d", IP2STR(cb_tcpConn->proto.tcp->remote_ip), cb_tcpConn->proto.tcp->remote_port);
	espconn_send(cb_tcpConn, message, os_strlen((char *)message));
}

void ICACHE_FLASH_ATTR user_tcp_init()
{
	// tcpConn and tcpProto
	// to do
	// make ready structure of connection -- check
	// connection call back function  
	// espconn_accept - creates a tcp server
	tcpProto.local_port = TCP_PORT;
	tcpConn.type = ESPCONN_TCP;
	tcpConn.state = ESPCONN_NONE;
	tcpConn.proto.tcp = &tcpProto;
	
	espconn_regist_connectcb(&tcpConn, tcp_connect_cb);
	espconn_accept(&tcpConn);
}


void ICACHE_FLASH_ATTR user_udp_init(void)
{
	
	//uint8_t ipAddr[] = { 192, 168, 43, 1 };
	//prepare udp connection structure 
	// create udp 
	os_memcpy(&espProto.remote_ip, ipAddr, 4);
	espProto.remote_port = UDP_PORT;
	espProto.local_port = UDP_PORT;
	
	espConn.type = ESPCONN_UDP;
	espConn.state = ESPCONN_NONE;
	espConn.proto.udp = &espProto;
	
	uint8_t is_it_created;
	is_it_created = espconn_create(&espConn);
	if(is_it_created == 0){
		os_printf("espconn created");
		os_timer_disarm(&udpServerTimer);
		os_timer_setfn(&udpServerTimer,(os_timer_func_t *)udp_func, (void*)0);
		os_timer_arm(&udpServerTimer, 1000, 1);
	}
	else{
		os_printf("espconn cannot be created");
	}

	//update_message();
	
}



void ICACHE_FLASH_ATTR ///////
uart_recvTask(os_event_t *events)
{
    if (events->sig == 0) {

        uint8_t fifo_len = (READ_PERI_REG(UART_STATUS(UART0)) >> UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT;
        //uint8_t d_tmp[128];
        uint8_t idx = 0;
		if (fifo_len > 128)
		{
			fifo_len = 128;
		}
		
        for (idx = 0; idx < fifo_len; idx++) {
            message[idx] = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;	
        }
		os_printf("new received\n");
		
        WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);
        uart_rx_intr_enable(UART0);
    }
}

/* to do for writing to flash memory
only sections can be read or written,cannot read or write single byte data
so for example 1MB flash has 256 section and each section is FFF length, 4096 defined as SPI_FLASH_SEC_SIZE. 
** size of the read write must be aligned to 4-byte boundary
** 4 byte des_adrr(sector number * sector ), 4-byte source addr, 4-byte size
-- erase the last section 
-- write the data


*/
void ICACHE_FLASH_ATTR
uart_recv_storeTask(os_event_t *event)
{
	if(event->sig == 0)	{
		uint8_t fifo_len = (READ_PERI_REG(UART_STATUS(UART0)) >> UART_RXFIFO_CNT_S)&UART_RXFIFO_CNT;
		uint8_t idx = 0;
		uint8_t recv_data[8];
		uint8_t *addr = (uint8_t *)&recv_data;
		uint8_t spi_err;
		SpiFlashOpResult spii;
		for (idx=0; idx<8; idx++){
			recv_data[idx] = READ_PERI_REG(UART_FIFO(UART0)) & 0xFF;
			message[idx];
		}
		spi_err = spi_flash_write(254* SPI_FLASH_SEC_SIZE, (uint32_t *)addr, 8);
		if(spi_err != SPI_FLASH_RESULT_OK){
			os_printf("%d", spi_err);
		}
		update_message();
		WRITE_PERI_REG(UART_INT_CLR(UART0), UART_RXFIFO_FULL_INT_CLR | UART_RXFIFO_TOUT_INT_CLR);
		uart_rx_intr_enable(UART0);
	}
	
}

void ICACHE_FLASH_ATTR update_message(){
	uint8_t *des_addr = (uint8_t *)&message;
	
	spi_flash_read(254 * SPI_FLASH_SEC_SIZE, (uint32_t *)des_addr, 8);
}

/******
problems::: can you use task to connect an ap like interrupt
create an tcp--- how to communicate with client when the esp8266 operates in  softap mode
********/


