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
        size_t index;
        std::string address;
        std::string subnet;
        std::string broadcast;
        size_t reassemblySize;
        int state;  // enum InterfaceState
    };

	std::vector<Interface> GetNetworkInterfaces();

    std::string CreateBroadcastAddress(const Interface& ifc);

    uint32_t ipToBytes(const std::string& ip);

}
