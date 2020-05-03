// https://raw.githubusercontent.com/zhicheng/base64/master/base64.c
/* This is a public domain base64 implementation written by WEI Zhicheng. */

#ifndef GATE_SENSOR_B64URL_H
#define GATE_SENSOR_B64URL_H

enum {
    BASE64_OK = 0,
    BASE64_INVALID
};

#define BASE64_ENCODE_OUT_SIZE(s)    (((s) + 2) / 3 * 4)
#define BASE64_DECODE_OUT_SIZE(s)    (((s)) / 4 * 3)

int base64url_encode(const unsigned char *in, unsigned int inlen, char *out);
int base64url_decode(const char *in, unsigned int inlen, unsigned char *out);

#endif //GATE_SENSOR_B64URL_H
