
#include "NetworkInterfaces.h"

#ifdef _WIN32
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iphlpapi.h>
#pragma comment(lib, "iphlpapi.lib")
#else   // ELSE _WIN32
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <arpa/inet.h>
#endif  // END _WIN32

#include <iostream>

namespace NetLib {

    std::vector<std::string> SplitString(std::string str, char delimeter) {
        std::string::size_type b = 0;
        std::vector<std::string> result;

        while ((b = str.find_first_not_of(delimeter, b)) != std::string::npos) {
            auto e = str.find_first_of(delimeter, b);
            result.push_back(str.substr(b, e - b));
            b = e;
        }

        return result;
    }

    std::string JoinStrings(std::vector<std::string> strings, std::string spacer) {
        std::string str = "";

        for (size_t i = 0; i < strings.size(); i++) {
            str += strings[i];

            if (i < strings.size() - 1 && spacer != "") {
                str += spacer;
            }
        }

        return str;
    }

	std::vector<Interface> GetNetworkInterfaces() {
        std::vector<Interface> interfaces;

#ifdef _WIN32
        // Get the required buffer size
        DWORD bufferSize = 0;
        if (GetIpAddrTable(nullptr, &bufferSize, 0) != ERROR_INSUFFICIENT_BUFFER) {
            return std::vector<Interface>();
        }

        // Setup the address table
        std::vector<MIB_IPADDRTABLE> table;
        table.reserve(bufferSize / sizeof(MIB_IPADDRTABLE));
        for (size_t i = 0; i < table.capacity(); i++) {
            table.push_back(MIB_IPADDRTABLE());
        }
        
        // Retrieve the Win32 address table
        unsigned long size = (unsigned long)(table.size() * sizeof(MIB_IPADDRTABLE));
        DWORD ret = GetIpAddrTable(&table[0], &size, 0);
        if (ret != 0) {
            return std::vector<Interface>();
        }

        // Construct the address table
        for (size_t i = 0; i < (size_t)table[0].dwNumEntries; i++) {

            IN_ADDR IPAddr;
            Interface ifc;
            
            ifc.index = table[0].table[i].dwIndex;

            IPAddr.S_un.S_addr = (u_long)table[0].table[i].dwAddr;
            ifc.address = inet_ntoa(IPAddr);

            IPAddr.S_un.S_addr = (u_long)table[0].table[i].dwMask;
            ifc.subnet = inet_ntoa(IPAddr);

            IPAddr.S_un.S_addr = (u_long)table[0].table[i].dwBCastAddr;
            ifc.broadcast = inet_ntoa(IPAddr);

            ifc.reassemblySize = table[0].table[i].dwReasmSize;

            ifc.state = NONE;
            if (table[0].table[i].wType & MIB_IPADDR_PRIMARY)
                ifc.state |= InterfaceState::PRIMARY_IP;
            if (table[0].table[i].wType & MIB_IPADDR_DYNAMIC)
                ifc.state |= InterfaceState::DYNAMIC_IP;
            if (table[0].table[i].wType & MIB_IPADDR_DISCONNECTED)
                ifc.state |= InterfaceState::DISCONNECTED_INTERFACE;
            if (table[0].table[i].wType & MIB_IPADDR_DELETED)
                ifc.state |= InterfaceState::BEING_DELETED;
            if (table[0].table[i].wType & MIB_IPADDR_TRANSIENT)
                ifc.state |= InterfaceState::TRANSIENT;

            interfaces.push_back(ifc);
        }

#else   // ELSE _WIN32

        struct ifaddrs *ifaddr;     // Get all network interfaces
        if (getifaddrs(&ifaddr) == -1)
            return interfaces;

        // Loop through all network interfaces
        for (struct ifaddrs *ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {

            if (ifa->ifa_addr == NULL)
                continue;

            if (ifa->ifa_addr->sa_family == AF_INET) {  // Only accept IPv4 addresses
                
                std::string address(INET_ADDRSTRLEN, '\0');
                std::string subnet(INET_ADDRSTRLEN, '\0');

                void* tmpAddrPtr = &((struct sockaddr_in *)(ifa->ifa_addr))->sin_addr;
                inet_ntop(AF_INET, tmpAddrPtr, &address[0], INET_ADDRSTRLEN);
                tmpAddrPtr = &((struct sockaddr_in *)(ifa->ifa_netmask))->sin_addr;
                inet_ntop(AF_INET, tmpAddrPtr, &subnet[0], INET_ADDRSTRLEN);

                Interface interface;
                interface.index = 0;
                interface.address = std::string(address);
                interface.name = std::string(ifa->ifa_name);
                interface.subnet = std::string(subnet);
                interface.reassemblySize = 0;
                interface.state = InterfaceState::NONE;

                interfaces.push_back(interface);
            }
        }

        freeifaddrs(ifaddr);

#endif  // END _WIN32

        return interfaces;
	}

    std::string CreateBroadcastAddress(const Interface& ifc) {
        auto ipParts = SplitString(ifc.address, '.');
        auto subParts = SplitString(ifc.subnet, '.');

        if (subParts.size() != 4 || ipParts.size() != 4) {  // Weird
            return "";
        }

        for (int i = 0; i < 4; i++) {
            if (subParts[i] != "255") {
                ipParts[i] = "255";
            }
        }

        return JoinStrings(ipParts, ".");
    }

    uint32_t ipToBytes(const std::string& ip) {
        return inet_addr(ip.c_str());
    }
}
