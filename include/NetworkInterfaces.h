#pragma once

#include <string>
#include <vector>

namespace NetLib {

    enum InterfaceState {
        NONE = 0,
        PRIMARY_IP = 1,
        DYNAMIC_IP = 2,
        DISCONNECTED_INTERFACE = 4,
        BEING_DELETED = 8,
        TRANSIENT = 16
    };

    struct Interface {
        size_t index;           // Windows only
        std::string address;
        std::string name;       // Non-windows only
        std::string subnet;     // Windows only
        std::string broadcast;  // Windows only
        size_t reassemblySize;  // Windows only
        InterfaceState state;   // Windows only
    };

	std::vector<Interface> GetNetworkInterfaces();

    std::string CreateBroadcastAddress(const Interface& ifc);

    uint32_t ipToBytes(const std::string& ip);

}
