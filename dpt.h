#ifndef DPT_H
#define DPT_H
#include <string.h>

inline void decode_dpt1(const unsigned char *data, bool *value) {
    *value = data[0] & 0x1;
}

inline void encode_dpt1(unsigned char *data, bool value) {
    data[0] &= 0xFE;
    if(value) data[0] |= 0x1;
}

inline void decode_dpt2(const unsigned char *data, bool *control, bool *value) {
    *value = data[0] & 0x1;
    *control = data[0] & 0x2;
}

inline void encode_dpt2(unsigned char *data, bool control, bool value) {
    data[0] &= 0xFC;
    if(value) data[0] |= 0x1;
    if(control) data[0] |= 0x2;
}

inline void decode_dpt3(const unsigned char *data, bool *control, unsigned char *stepcode) {
    *stepcode = data[0] & 0x7;
    *control = data[0] & 0x8;
}

inline void encode_dpt3(unsigned char *data, bool control, unsigned char stepcode) {
    data[0] &= 0xF0;
    if(control) data[0] |= 0x8;
    data[0] |= (0x7 & stepcode);
}

inline void decode_dpt4(const unsigned char *data, char *value) {
    *value = static_cast<char>(data[1]);
}

inline void encode_dpt4(unsigned char *data, char value) {
    data[1] = static_cast<unsigned char>(value);
}

inline void payload_to_dpt5(const unsigned char *data, unsigned char *value) {
    *value = data[2];
}

inline void dpt5_to_payload(unsigned char *data, unsigned char value) {
    data[2] = value;
}

inline void decode_dpt6(const unsigned char *data, signed char *value) {
    *value = static_cast<signed char>(data[1]);
}

inline void encode_dpt6(unsigned char *data, signed char value) {
    data[1] = static_cast<unsigned char>(value);
}

inline void decode_dpt7(const unsigned char *data, unsigned short *value) {
    *value = static_cast<unsigned short>(data[1] << 8 | data[2]);
}

inline void dpt7_to_payload(unsigned char *data, unsigned short value) {
    data[2] = (value >> 8) & 0xFF;
    data[3] = value & 0xFF;
}

inline void decode_dpt8(const unsigned char *data, signed short *value) {
    *value = static_cast<signed short>(data[1] << 8 | data[2]);
}

inline void encode_dpt8(unsigned char *data, signed short value) {
    data[1] = (value >> 8) & 0xFF;
    data[2] = value & 0xFF;
}

inline void payload_to_dpt9(const unsigned char *data, float *value) {
    unsigned char sign = (data[2] & 0x80) >>  7;
    unsigned short exp = (data[2] & 0x78) >> 3;
    int mant = ((data[2] & 0x07) << 8) | data[3];
    if(sign != 0)
        mant = -(~(mant - 1) & 0x07ff);

    *value  = static_cast<float>((1 << exp) * 0.01 * (static_cast<int>(mant)));
}

inline void dpt9_to_payload(unsigned char *data, float value) {
    unsigned int sign = (value < 0);
    unsigned int exp  = 0;
    int mant = static_cast<int>(static_cast<double>(value) * 100.0);

    while(mant > 2047 || mant <= -2048 )
    {
        mant = mant >> 1;
        ++exp;
    }
    mant &= 0x07ff;

    data[2] = static_cast<unsigned char>((sign << 7) | (exp << 3) | ((mant & 0x07ff) >> 8));
    data[3] = mant & 0xFF;
}

inline void decode_dpt10(const unsigned char *data, unsigned char *day, unsigned char *hour, unsigned char *minute, unsigned char *second) {
    *day  = ((data[1]) >>5) & 0x7;
    *hour = (data[1] & 0x1F);
    *minute = (data[2] & 0x3F);
    *second = (data[3] & 0x3F);
}

inline void encode_dpt10(unsigned char *data, unsigned char day, unsigned char hour, unsigned char minute, unsigned char second) {
    data[1] = static_cast<unsigned char>(((day & 0x7) << 5) | (hour & 0x1F));
    data[2] = minute & 0x3F;
    data[3] = second & 0x3F;
}


inline void decode_dpt11(const unsigned char *data, unsigned char *day, unsigned char *month, unsigned char *year) {
    *day = (data[1] & 0x3F);
    *month = (data[2] & 0x3F);
    *year = (data[3] & 0x7F);
}

inline void encode_dpt11(unsigned char *data, unsigned char day, unsigned char month, unsigned char year) {
    data[1] = day & 0x3F;
    data[2] = month & 0x3F;
    data[3] = year & 0x7F;
}


inline void decode_dpt12(const unsigned char *data, unsigned int *value) {
    *value = static_cast<unsigned int>(data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4]);
}

inline void encode_dpt12(unsigned char *data, unsigned int value) {
    data[1] = (value >> 24) & 0xFF;
    data[2] = (value >> 16) & 0xFF;
    data[3] = (value >>  8) & 0xFF;
    data[4] = (value >>  0) & 0xFF;
}

inline void decode_dpt13(const unsigned char *data, signed int *value) {
    *value = data[1] << 24 | data[2] << 16 | data[3] << 8 | data[4];
}

inline void encode_dpt13(unsigned char *data, signed int value) {
    data[1] = (value >> 24) & 0xFF;
    data[2] = (value >> 16) & 0xFF;
    data[3] = (value >>  8) & 0xFF;
    data[4] = (value >>  0) & 0xFF;
}

inline void decode_dpt14(const unsigned char *data, float *value) {
    unsigned int rdata = static_cast<unsigned int>((data[1]<< 24) | (data[2]<< 16) | (data[3]<< 8) | data[4]);
    memcpy(value, &rdata, sizeof data);
}

inline void encode_dpt14(unsigned char *data, float value) {
    unsigned int rdata;
    memcpy(&rdata, &value, sizeof(rdata));

    data[1] = (rdata >> 24) & 0xFF;
    data[2] = (rdata >> 16) & 0xFF;
    data[3] = (rdata >>  8) & 0xFF;
    data[4] = (rdata >>  0) & 0xFF;
}

inline void decode_dpt225(const unsigned char *data, unsigned short *time_period, unsigned char *percent) {
    *percent = data[2];
    *time_period = static_cast<unsigned short>(data[1] << 8 | data[2]);
}

inline void encode_dpt225(unsigned char *data, unsigned short time_period, unsigned char percent) {
    data[1] = (time_period & 0xFF00) >> 8;
    data[2] = time_period & 0xFF;
    data[3] = percent;
}

#define DPT(a,b) (a<<8 | b)

#endif // DPT_H
