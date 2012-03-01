//
// Copyright (C) 2012 Telldus Technologies AB. All rights reserved.
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef TCPIPCONFIG_H
#define TCPIPCONFIG_H

#define MAX_UDP_SOCKETS 5
#define MAX_HTTP_CONNECTIONS 5

#define STACK_USE_UDP
#define STACK_USE_DHCP_CLIENT
#define STACK_USE_ICMP_SERVER
#define STACK_USE_REBOOT_SERVER

#define STACK_USE_TCP
// #define STACK_USE_SSL_CLIENT
#define STACK_CLIENT_MODE
#define STACK_USE_DNS
#define STACK_USE_SHA1

#define MY_DEFAULT_IP_ADDR_BYTE1        (192ul)
#define MY_DEFAULT_IP_ADDR_BYTE2        (168ul)
#define MY_DEFAULT_IP_ADDR_BYTE3        (0ul)
#define MY_DEFAULT_IP_ADDR_BYTE4        (51ul)

#define MY_DEFAULT_MASK_BYTE1           (255ul)
#define MY_DEFAULT_MASK_BYTE2           (255ul)
#define MY_DEFAULT_MASK_BYTE3           (255ul)
#define MY_DEFAULT_MASK_BYTE4           (0ul)

#define MY_DEFAULT_GATE_BYTE1           (192ul)
#define MY_DEFAULT_GATE_BYTE2           (168ul)
#define MY_DEFAULT_GATE_BYTE3           (0ul)
#define MY_DEFAULT_GATE_BYTE4           (1ul)

#define MY_DEFAULT_PRIMARY_DNS_BYTE1	(192ul)
#define MY_DEFAULT_PRIMARY_DNS_BYTE2	(168ul)
#define MY_DEFAULT_PRIMARY_DNS_BYTE3	(0ul)
#define MY_DEFAULT_PRIMARY_DNS_BYTE4	(1ul)

#define MY_DEFAULT_SECONDARY_DNS_BYTE1	(0ul)
#define MY_DEFAULT_SECONDARY_DNS_BYTE2	(0ul)
#define MY_DEFAULT_SECONDARY_DNS_BYTE3	(0ul)
#define MY_DEFAULT_SECONDARY_DNS_BYTE4	(0ul)

#define TCP_ETH_RAM_SIZE					(3900ul)
#define TCP_PIC_RAM_SIZE					(0ul)
#define TCP_SPI_RAM_SIZE					(0ul)
#define TCP_SPI_RAM_BASE_ADDRESS			(0x00)

// Define names of socket types
#define TCP_SOCKET_TYPES
	#define TCP_PURPOSE_GENERIC_TCP_CLIENT 0
	#define TCP_PURPOSE_SERVER_DISCOVERY 1
	#define TCP_PURPOSE_LIVE 2
#define END_OF_TCP_SOCKET_TYPES

#if defined(__TCP_C)
	// Define what types of sockets are needed, how many of
	// each to include, where their TCB, TX FIFO, and RX FIFO
	// should be stored, and how big the RX and TX FIFOs should
	// be.  Making this initializer bigger or smaller defines
	// how many total TCP sockets are available.
	//
	// Each socket requires up to 56 bytes of PIC RAM and
	// 48+(TX FIFO size)+(RX FIFO size) bytes of TCP_*_RAM each.
	//
	// Note: The RX FIFO must be at least 1 byte in order to
	// receive SYN and FIN messages required by TCP.  The TX
	// FIFO can be zero if desired.
	#define TCP_CONFIGURATION
	ROM struct
	{
		BYTE vSocketPurpose;
		BYTE vMemoryMedium;
		WORD wTXBufferSize;
		WORD wRXBufferSize;
	} TCPSocketInitializer[] =
	{
		{TCP_PURPOSE_GENERIC_TCP_CLIENT, TCP_ETH_RAM, 350, 1000},
// 		{TCP_PURPOSE_SERVER_DISCOVERY, TCP_ETH_RAM, 200, 200},
// 		{TCP_PURPOSE_LIVE, TCP_ETH_RAM, 200, 200},
	};
	#define END_OF_TCP_CONFIGURATION

#endif

// #define MAX_SSL_CONNECTIONS		(2ul)	// Maximum connections via SSL
// #define MAX_SSL_SESSIONS		(2ul)	// Max # of cached SSL sessions
// #define MAX_SSL_BUFFERS			(4ul)	// Max # of SSL buffers (2 per socket)
// #define MAX_SSL_HASHES			(5ul)	// Max # of SSL hashes  (2 per, plus 1 to avoid deadlock)


#endif //TCPIPCONFIG_H
