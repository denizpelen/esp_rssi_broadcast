//// udp library 
/*
deniz pelen
29 August 2020 v1.0
*/

#include "wifi_connection.h"

#include "mem.h"
#include "osapi.h"
#include "c_types.h"
#include "ets_sys.h"
#include "user_interface.h"
#include "espconn.h"



#define SSID "Pelen"
#define PASSWORD "deniz5a4a3a2a1a"

#define UDP_PORT 85125

static struct espconn espConn ;
static  esp_udp espProto ;
static os_timer_t udpServerTimer;



/*************************************
connect to existing defined AP
no error handling is implemented yet  

**************************************/
void ICACHE_FLASH_ATTR user_connect_ap()
{
	char ssid[32] = SSID;
	char password[64] = PASSWORD;
	struct station_config stationConf;
	bool isConnected ;
	struct ip_info my_ip;
	
	//station_config.bssid_set = 0;   /// do not check the mac addresses of AP
	os_memcpy(&stationConf.ssid, ssid, 32);
	os_memcpy(&stationConf.password, password, 64);
	
	wifi_set_opmode_current(STATION_MODE);
	//wifi_station_disconnect();
	wifi_station_set_config(&stationConf);
	isConnected = wifi_station_connect();
	if(isConnected == true)
	{
		os_printf("connected. \n");
		//wifi_get_ip_info(STATION_IF, &my_ip );
		//os_printf(IP2STR(&my_ip.ip));
	}
	else{
		os_printf("connection failed.\n");
	}
}

static void ICACHE_FLASH_ATTR udp_func(void)
{
	char message[128] = "merhaba";
	espconn_send(&espConn, message, os_strlen((char*)message));
}


void ICACHE_FLASH_ATTR user_udp_init()
{
	
	
	uint8_t ipAddr[] = { 192, 168, 1, 21 };
	//prepare udp connection structure 
	// create udp 
	os_memcpy(&espProto.remote_ip, ipAddr, 4);
	espProto.remote_port = UDP_PORT;
	espProto.local_port = UDP_PORT;
	
	espConn.type = ESPCONN_UDP;
	espConn.state = ESPCONN_NONE;
	espConn.proto.udp = &espProto;
	
	espconn_create(&espConn);
	
	os_timer_disarm(&udpServerTimer);
	os_timer_setfn(&udpServerTimer,(os_timer_func_t *)udp_func, (void*)0);
	os_timer_arm(&udpServerTimer, 100, 1);
	
}







