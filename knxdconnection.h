#ifndef KNXDCONNECTION_H
#define KNXDCONNECTION_H

#include <eibclient.h>
#include <string>
#include <vector>


typedef void (*gadCallback)(uint16_t src, uint16_t dest, unsigned char *payload);

class KnxdConnection
{
    EIBConnection *m_knxd {nullptr};
    eibaddr_t m_individualAddress {0};
    std::string m_url;
    std::vector<uint16_t> m_gads;
    gadCallback m_call {nullptr};

    void __send(unsigned char cmd, uint16_t gad, uint16_t dpt, double val);

public:
    explicit KnxdConnection(eibaddr_t addr, std::string knxd_url);
    virtual ~KnxdConnection();
    bool connect();
    void process();
    void subscribe(uint16_t gad);
    void registerCallback(gadCallback);
    void keepalive();
    int fd();
    void write(uint16_t gad, uint16_t dpt, double val);
    void response(uint16_t gad, uint16_t dpt, double val);

};

#endif // KNXDCONNECTION_H
