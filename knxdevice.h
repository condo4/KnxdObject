#ifndef KNXDEVICE_H
#define KNXDEVICE_H

#include "knxdconnection.h"
#include <string>
#include <ctime>

class KnxDevice: public KnxdConnection
{
    std::map<uint16_t, uint16_t> m_gadDpt;
    std::map<uint16_t, std::string> m_gadCmd;
    std::map<uint16_t, double> m_gadVal;
    std::map<uint16_t, std::string> m_gadFlags;
    std::time_t m_time {0};
    std::string m_hostname;
    unsigned short m_port {0};
    bool m_running {true};
    bool m_dmz {false};

public:
    KnxDevice(const std::string &id);
    void rx(uint16_t src, uint16_t gad, unsigned char *payload) override;
    uint16_t dptFromGad(uint16_t gad);
    std::string flagFromGad(uint16_t gad);
    std::string cmdFromGad(uint16_t gad);
    void process();
    const std::string &getHostname() const;
    unsigned short getPort() const;
    void exec();
    std::time_t getTime() const;
    void setTime(std::time_t time);

    virtual double getFromDevice(std::string param) = 0;
    virtual double sendToDevice(uint16_t gad, unsigned char *payload) = 0;
};

#endif // KNXDEVICE_H
