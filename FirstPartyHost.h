#ifndef FIRST_PARTY_HOST_H_
#define FIRST_PARTY_HOST_H_

#include "node_modules/hashset-cpp/hashFn.h"

struct ST_FIRST_PARTY_HOST
{
public:
    ST_FIRST_PARTY_HOST():
        sFirstPartyHost(nullptr),
        sThirdPartyHosts(nullptr) {
    }
    
    ST_FIRST_PARTY_HOST(const ST_FIRST_PARTY_HOST &other) {
        if (nullptr != other.sFirstPartyHost) {
            sFirstPartyHost = new char[strlen(other.sFirstPartyHost) + 1];
            strcpy(sFirstPartyHost, other.sFirstPartyHost);
        }
        if (nullptr != other.sThirdPartyHosts) {
            sThirdPartyHosts = new char[strlen(other.sThirdPartyHosts) + 1];
            strcpy(sThirdPartyHosts, other.sThirdPartyHosts);
        }
    }
    
    ~ST_FIRST_PARTY_HOST() {
        if (nullptr != sFirstPartyHost) {
            delete []sFirstPartyHost;
        }
        if (nullptr != sThirdPartyHosts) {
            delete []sThirdPartyHosts;
        }
    }
    
    uint64_t hash() const {
        // Calculate hash only on first party host as we will search using it only
        HashFn fn(19);
        if (!sFirstPartyHost) {
            return 0;
        }
        
        return fn(sFirstPartyHost, static_cast<int>(strlen(sFirstPartyHost)));
        
    }
    
    bool operator==(const ST_FIRST_PARTY_HOST &rhs) const {
        int hostLen = static_cast<int>(strlen(sFirstPartyHost));
        int rhsHostLen = static_cast<int>(strlen(rhs.sFirstPartyHost));
        
        if (hostLen != rhsHostLen || 0 != memcmp(sFirstPartyHost, rhs.sFirstPartyHost, hostLen)) {
            return false;
        }
        
        return true;
    }
    
    // Nothing needs to be updated when a host is added multiple times
    void update(const ST_FIRST_PARTY_HOST&) {}
    
    uint32_t serialize(char* buffer) {
        uint32_t size = 0;
        unsigned int pos = 0;
        if (buffer) {
            snprintf(buffer, sizeof(unsigned int), "%x", (unsigned int)strlen(sFirstPartyHost));
        }
        pos += sizeof(unsigned int);
        if (buffer) {
            memcpy(buffer + pos, sFirstPartyHost, strlen(sFirstPartyHost));
        }
        pos += strlen(sFirstPartyHost);
        if (buffer) {
            snprintf(buffer + pos, sizeof(unsigned int), "%x", (unsigned int)strlen(sThirdPartyHosts));
        }
        pos += sizeof(unsigned int);
        if (buffer) {
            memcpy(buffer + pos, sThirdPartyHosts, strlen(sThirdPartyHosts));
        }
        size = pos + strlen(sThirdPartyHosts);
        
        return size;
    }
    
    uint32_t deserialize(char *buffer, uint32_t bufferSize) {
        uint32_t size = 0;
        
        if (!buffer || 0 == bufferSize) {
            return size;
        }
        
        // Get first party host
        unsigned int firstPartyHostLength = 0;
        sscanf(buffer, "%x", &firstPartyHostLength);
        if (sFirstPartyHost) {
            delete []sFirstPartyHost;
        }
        size = sizeof(firstPartyHostLength);
        sFirstPartyHost = new char[firstPartyHostLength + 1];
        if (!sFirstPartyHost) {
            return size;
        }
        memcpy(sFirstPartyHost, buffer + size, firstPartyHostLength);
        sFirstPartyHost[firstPartyHostLength] = '\0';
        size += firstPartyHostLength;
        
        // Get third party hosts
        unsigned int thirdPartyHostLength = 0;
        sscanf(buffer + size, "%x", &thirdPartyHostLength);
        if (sThirdPartyHosts) {
            delete []sThirdPartyHosts;
        }
        size += sizeof(thirdPartyHostLength);
        sThirdPartyHosts = new char[thirdPartyHostLength + 1];
        if (!sThirdPartyHosts) {
            return size;
        }
        memcpy(sThirdPartyHosts, buffer + size, thirdPartyHostLength);
        sThirdPartyHosts[thirdPartyHostLength] = '\0';
        size += thirdPartyHostLength;
        
        return size;
    }
    
    char* sFirstPartyHost;
    // Third party hosts are comma separated
    char* sThirdPartyHosts;
};

#endif  //FIRST_PARTY_HOST_H_
