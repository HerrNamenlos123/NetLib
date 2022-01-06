
#include "NetworkInterfaces.h"

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include <winsock2.h>
#include <iphlpapi.h>

#pragma comment(lib, "iphlpapi.lib")

namespace NetLib {

	std::vector<Interface> GetNetworkInterfaces() {

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
        std::vector<Interface> interfaces;
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
            ifc.broadcast2 = table[0].table[i].dwBCastAddr;

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

        return interfaces;
	}
}
