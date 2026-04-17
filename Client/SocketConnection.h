#pragma once
#include <winsock2.h>
#include <ws2tcpip.h> 
#pragma comment(lib, "ws2_32.lib")
#include "..\Airplane-Management-And-Warranty-System\Packet.h"
#include "..\Airplane-Management-And-Warranty-System\PacketFactory.h"

#define PAGE_SIZE 4096

void handshake_with_tcp_server(SOCKET sock);
SOCKET establish_connection();
void terminate_connection(SOCKET s);