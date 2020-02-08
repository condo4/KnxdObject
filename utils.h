#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <algorithm>

inline std::vector<std::string> split(const std::string& s, char seperator)
{
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;
    while((pos = s.find(seperator, pos)) != std::string::npos)
    {
        std::string substring( s.substr(prev_pos, pos-prev_pos) );
        output.push_back(substring);
        prev_pos = ++pos;
    }
    output.push_back(s.substr(prev_pos, pos-prev_pos)); // Last word
    return output;
}

inline std::string gadToStr(uint16_t addr, char sep='/')
{
    char buff[10];
    snprintf (buff, 10, "%d%c%d%c%d", (addr >> 11) & 0x1f, sep, (addr >> 8) & 0x07, sep, (addr) & 0xff);
    buff[9] = '\0';
    return std::string(buff);
}

inline uint16_t strToGad(const std::string &sgad)
{
    std::vector<std::string> gads = split(sgad, '/');
     if(gads.size() != 3)
         return 0;

     return (static_cast<unsigned short>( std::stoi(gads[0]) << 11 | std::stoi(gads[1]) << 8 | std::stoi(gads[2])));
}

inline uint16_t strToDpt(const std::string &stype)
{
    std::vector<std::string> types = split(stype, '.');
    if(types.size() != 2)
        return 0;
    std::replace( types[1].begin(), types[1].end(), 'x', '0');
    return (static_cast<unsigned short>( std::stoi(types[0]) << 8 |  std::stoi(types[1])));
}


inline std::string dptToStr(uint16_t type)
{
    char buff[10];
    snprintf (buff, 10, "%d.%d", (type >> 8) & 0xff, (type) & 0xff);
    buff[9] = '\0';
    return std::string(buff);
}

inline uint16_t strToPhy(const std::string &a)
{
    std::vector<std::string> ad = split(a, '.');
     if(ad.size() != 3)
         return 0;
     return (static_cast<uint16_t>( ((std::stoi(ad[0]) << 12) & 0xF000) | ((std::stoi(ad[1]) << 8) & 0xF00)  | (std::stoi(ad[2]) & 0xFF)));
}

inline std::string phyToStr(uint16_t addr, char sep='.')
{
    char buff[10];
    snprintf (buff, 10, "%d%c%d%c%d", (addr >> 12) & 0xF, sep, (addr >> 8) & 0xF, sep, (addr) & 0xFF);
    buff[9] = '\0';
    return std::string(buff);
}

#define KNX_CMD_READ            (0x00)
#define KNX_CMD_RESPONSE        (0x01)
#define KNX_CMD_WRITE           (0x02)
#define KNX_CMD_MEMWRITE        (0x0A)

enum CMD {
    READ = 0,
    RESPONSE = 1,
    WRITE = 2,
    MEMWRITE = 0xA
};

inline CMD getCmd(unsigned char *payload)
{
    return static_cast<CMD>(((payload[0] & 0x03) << 2) | ((payload[1] & 0xC0) >> 6));
}

#endif // UTILS_H
