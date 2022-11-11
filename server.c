// Copyright(c)2022 Vishal Ahirwar. All rights reserved.
//Cross-platform program for listing network all the network adapters available on the machine
#if defined(_WIN32)
#ifndef _WIN32_WINT
#define _WIN32_WINT 0x0600
#endif

#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

#else

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#define SUCCESS 0
#define FAIL -1
void print();

#ifndef _WIN32
void ListAddresses(struct ifaddrs *);
struct ifaddrs *Getifaddrs_ptr();
#endif

int main(void)
{
	print();
#if defined(_WIN32)
	printf("running on win32...\n-----------------------\n");
	WSADATA d;
	if (WSAStartup(MAKEWORD(2, 2), &d))
	{
		printf("FAIL to Initialize.\n");
		return FAIL;
	};
	DWORD asize = 20000;
	PIP_ADAPTER_ADDRESSES adapters;
	do
	{
		/* code */
		adapters = (PIP_ADAPTER_ADDRESSES)malloc(asize);
		if (adapters == NULL)
		{
			printf("Couldn't allocate %ld bytes  for adapters!\n", asize);
			WSACleanup();
			return FAIL;
		};
		int r = GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_INCLUDE_PREFIX, 0, adapters, &asize);
		if (r == ERROR_BUFFER_OVERFLOW)
		{
			printf("GetAdapterAddresses wants %ld bytes.\n", asize);
			free(adapters);
		}
		else if (r == ERROR_SUCCESS)
		{
			break;
		}
		else
		{
			printf("Error From GetAdapterAddresses %d !\n", r);
			free(adapters);
			WSACleanup();
			return FAIL;
		};

	} while (!adapters);
	PIP_ADAPTER_ADDRESSES adapter = adapters;
	while (adapter)
	{
		printf("Adapter Name : %S\n", adapter->FriendlyName);
		PIP_ADAPTER_UNICAST_ADDRESS address = adapter->FirstUnicastAddress;
		while (address)
		{
			printf("\t%s\n", address->Address.lpSockaddr->sa_family == AF_INET ? "Ipv4" : "ipv6");
			char ap[100];
			getnameinfo(address->Address.lpSockaddr, address->Address.iSockaddrLength, ap, sizeof(ap), 0, 0, NI_NUMERICHOST);
			printf("\t%s\n", ap);
			address = address->Next;
		};
		adapter = adapter->Next;
		printf("\n*************************\n");
	};
	free(adapters);
	WSACleanup();
	printf("Press Any key to terminate Program...\n");
	getchar();
	return SUCCESS;
#else
	printf("running on linux machine...\n");
	struct ifaddrs *addresses = Getifaddrs_ptr();
	ListAddresses(addresses);
	freeifaddrs(addresses);
	return SUCCESS;
#endif
	return SUCCESS;
};

#ifndef _WIN32

struct ifaddrs *Getifaddrs_ptr()
{
	struct ifaddrs *addresses = NULL;
	if (getifaddrs(&addresses) == -1)
	{
		printf("getifaddrs() call FAIL!\n");
		exit(EXIT_FAILURE);
	};
	return addresses;
}
void ListAddresses(struct ifaddrs *addresses)
{
	// going through the addresses linked list
	struct ifaddrs *address = addresses;
	while (address)
	{
		int family = address->ifa_addr->sa_family;
		if (family == AF_INET || family == AF_INET6)
		{
			printf("[name] : \t\t%s\n", address->ifa_name);
			printf("[family] : \t\t%s\n", family == AF_INET ? "IPv4" : "IPv6");
			const int family_size = family == AF_INET ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6);
			char nameinfo[100];
			getnameinfo(address->ifa_addr, family_size, nameinfo, sizeof(nameinfo), 0, 0, NI_NUMERICHOST);
			printf("[IP] : \t\t%s\n", nameinfo);
			printf("\n--------------------------------------------\n");
		};
		address = address->ifa_next;
	};
};
#endif
void print()
{
#ifndef _WIN32
	system("clear");
#else
	system("cls");
#endif
	printf("Copyright(c)2022 Vishal Ahirwar. All rights reserved.\n");
};
