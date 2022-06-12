#include "knxdevice.h"
#include "utils.h"
#include "dpt.h"
#include <iostream>
#include <systemd/sd-daemon.h>


std::time_t KnxDevice::getTime() const
{
    return m_time;
}

KnxDevice::KnxDevice(const std::string &id)
    : KnxdConnection(id)
{
    for(const std::string &var: m_conf.array("vars"))
    {
        auto s = split(var, '|');
        if(s.size() == 4)
        {
            uint16_t gad = strToGad(s[1]);
            subscribe(gad);
            m_gadDpt[gad] = strToDpt(s[0]);
            m_gadCmd[gad] = s[2];
            m_gadFlags[gad] = s[3];
        }
    }
    std::string url = m_conf["device_url"];
    if(url.size())
    {
        size_t sep = url.find(":");
        m_hostname = url.substr(0, sep);
        m_port = static_cast<unsigned short>(std::stoul((url.substr(sep + 1))));
    }
    if(m_conf["dmz"].size())
    {
        if(m_conf["dmz"] == "1")
        {
            setDmz(true);
        }
    }
}


void KnxDevice::rx(uint16_t src, uint16_t gad, unsigned char *payload)
{
    bool needtoproccess = false;
#ifdef DEBUG
    switch(getCmd(payload))
    {

    case CMD::WRITE:
        std::cout << "WRITE:";
        break;

    case CMD::RESPONSE:
        std::cout << "RESPONSE:";
        break;

    default:
        break;
    }
    std::cout << phyToStr(src) << "->" << gadToStr(gad) << std::endl;
#endif

    uint16_t dpt = m_gadDpt[gad];

    switch(getCmd(payload))
    {
    case CMD::READ:
    {
        // Need to send response, last value quickly, then trig process for update value
        if(m_gadFlags[gad].find("R") != std::string::npos)
        {
            response(gad, dpt, m_gadVal[gad]);
            needtoproccess = true;
        }
        break;
    }
    case CMD::WRITE:
    {
        if(m_gadFlags[gad].find("W") != std::string::npos)
        {
            needtoproccess = true;
            m_time = 0; // Reset cache
            m_gadVal[gad] = sendToDevice(gad, payload);
        }
        else if(dmz())
        {
            sendToDevice(gad, payload);
        }

        break;
    }
    default:
        break;
    }
    if(needtoproccess)
        process();
}

uint16_t KnxDevice::dptFromGad(uint16_t gad)
{
    return m_gadDpt[gad];
}

std::string KnxDevice::flagFromGad(uint16_t gad)
{
    return m_gadFlags[gad];
}

std::string KnxDevice::cmdFromGad(uint16_t gad)
{
    return m_gadCmd[gad];
}

void KnxDevice::process()
{
    for (auto const& x : m_gadCmd)
    {
        uint16_t gad = x.first;
        if(m_gadFlags[gad] == "CRT")
        {
            double val = getFromDevice(x.second);
            if(m_gadVal.count(gad) == 0) m_gadVal[gad] = 0;
            if(std::abs(m_gadVal[gad] - val) > 0.1)
            {
                m_gadVal[gad] = val;
                write(gad, m_gadDpt[gad], val);
            }
        }
    }
}

const std::string &KnxDevice::getHostname() const
{
    return m_hostname;
}

unsigned short KnxDevice::getPort() const
{
    return m_port;
}

void KnxDevice::exec()
{
    fd_set readfds;
    int polling = std::stoi(m_conf["polling"]);

    std::cout << "Start main loop, polling: " << polling << "s" <<  std::endl;

    std::time_t last_time = 0;
    std::time_t current = std::time(nullptr);

    /* Service is ready */
    sd_notify(0, "READY=1");

    process();
    knxProcess();


    /* EVENT LOOP */
    while(m_running)
    {
        //clear the socket set
        struct timeval tv = {polling, 0};
        if(current - last_time < polling)
            tv.tv_sec = polling - (current - last_time);
        int knxdfd = fd();
        FD_ZERO(&readfds);
        FD_SET(knxdfd, &readfds);
        int max_fd = fd();

        int activity = select( max_fd + 1 , &readfds, nullptr, nullptr, &tv);

        if ((activity < 0) && (errno != EINTR))
        {
            std::cerr << "select error" << std::endl;
        }

        current = std::time(nullptr);
        if((current - last_time) > polling || activity == 0)
        {
#ifdef DEBUG
            std::cout << "POLL:" << std::asctime(std::localtime(&current));
#endif
            last_time = current;

            keepalive();
            process();
        }

        if (FD_ISSET(knxdfd, &readfds))
        {
            knxProcess();
        }
    }
}
