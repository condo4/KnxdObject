#include "knxdconnection.h"
#include <iostream>
#include <algorithm>
#include "utils.h"
#include "dpt.h"

void KnxdConnection::__send(unsigned char cmd, uint16_t gad, uint16_t dpt, double val)
{
    unsigned char message[11] = {0x00, 0x80, 0x0c, 0x65};
    /* WRITE */
    message[0] = 00;
    message[1] = cmd;
    std::cerr << "Need to write " << gadToStr(gad) << " (" << dptToStr(dpt) << "): " << val << std::endl;

    switch(dpt) {
    case DPT(9,1):
        dpt9_to_payload(message, static_cast<float>(val));
        EIBSendGroup(m_knxd, gad, 2+2, message);
        break;

    case DPT(5,5):
        dpt5_to_payload(message, static_cast<unsigned char>(val));
        EIBSendGroup(m_knxd, gad, 2+1, message);
        break;

    case DPT(7,1):
        dpt7_to_payload(message, static_cast<unsigned short>(val));
        EIBSendGroup(m_knxd, gad, 2+2, message);
        break;
    }
}

KnxdConnection::KnxdConnection(eibaddr_t addr, std::string knxd_url)
    : m_individualAddress(addr)
    , m_url(knxd_url)
{

}

KnxdConnection::~KnxdConnection()
{
    EIBClose(m_knxd);
}

bool KnxdConnection::connect()
{
    m_knxd =  EIBSocketURL(m_url.c_str());
    if(!m_knxd)
    {
        std::cerr << "Invalid socket format: " << m_url;
        return false;
    }

    if (EIBOpen_GroupSocket (m_knxd, 0) == -1)
    {
        std::cerr << "Error opening knxd group socket (" << m_url << ")" << std::endl;
        return false;
    }

    std::cout << "KNX Connected to " << m_url << std::endl;

    return true;
}


void KnxdConnection::process()
{
    unsigned char buffer[1025];  //data buffer of 1K
    eibaddr_t dest;
    eibaddr_t src;
    int len = EIBGetGroup_Src (m_knxd, sizeof (buffer), buffer, &src, &dest);
    //std::cout << "knxd fd set: " << gadToStr(dest) <<  std::endl;
    if(len < 0)
    {
        std::cerr << "Read EIBGetGroup_Src failed (" << len << ") reconnect" << std::endl;
        keepalive();
        return;
    }
    if(len < 2)
    {
        std::cerr << "Read EIBGetGroup_Src Invalid packet" << std::endl;
        return;
    }

    if(std::find(m_gads.begin(), m_gads.end(), dest) != m_gads.end())
    {
        if(m_call) m_call(src, dest, buffer);
    }
}

void KnxdConnection::subscribe(uint16_t gad)
{
    if(!(std::find(m_gads.begin(), m_gads.end(), gad) != m_gads.end()))
    {
        m_gads.push_back(gad);
    }
}

void KnxdConnection::registerCallback(gadCallback call)
{
    m_call = call;
}

void KnxdConnection::keepalive()
{
    /*
    uint8_t buf[32];
    int ret = EIB_M_ReadIndividualAddresses (m_knxd, 32, buf);
    std::cout << "KEEPALIVE: " << ret << std::endl;
    if(ret == -1)
    {
            std::cerr << "Connection error on reset, reconnect" << std::endl;
            EIBClose_sync(m_knxd);
            connect();
    }
    */
}

int KnxdConnection::fd()
{
    return EIB_Poll_FD(m_knxd);
}

void KnxdConnection::write(uint16_t gad, uint16_t dpt, double val)
{
    __send(0x80, gad, dpt, val);
}

void KnxdConnection::response(uint16_t gad, uint16_t dpt, double val)
{
    __send(0x40, gad, dpt, val);
}
