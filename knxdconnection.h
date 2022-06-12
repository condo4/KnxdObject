#ifndef KNXDCONNECTION_H
#define KNXDCONNECTION_H

#include <eibclient.h>
#include <string>
#include <vector>
#include "UnixConfig/config.h"



class KnxdConnection
{
protected:
    Config m_conf;

private:
    EIBConnection *m_knxd {nullptr};
    eibaddr_t m_individualAddress {0};
    std::string m_url;
    std::vector<uint16_t> m_gads;
    bool m_dmz {false};

    void __send(unsigned char cmd, uint16_t gad, uint16_t dpt, double val);

public:
    explicit KnxdConnection(const std::string &id, bool dmz = false);
    virtual ~KnxdConnection();
    bool knxConnect();
    void subscribe(uint16_t gad);
    void keepalive();
    int fd();
    void write(uint16_t gad, uint16_t dpt, double val);
    void response(uint16_t gad, uint16_t dpt, double val);

    void knxProcess();
    virtual void rx(uint16_t src, uint16_t dest, unsigned char *payload) = 0;
    void setDmz(bool newDmz);
    bool dmz() const;
};

#endif // KNXDCONNECTION_H
