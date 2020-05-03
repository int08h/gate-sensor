#ifndef GATE_SENSOR_SIGNER_H
#define GATE_SENSOR_SIGNER_H

#include <WiFi.h>
#include <ctime>
#include <cstdio>
#include <WString.h>
#include <array>

#include "constants.h"
#include "b64url.h"
#include "mlog.h"
#include "keys.h"

#include <mbedtls/pk.h>
#include <mbedtls/error.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>

class Signer {
public:
    Signer() {
        mbedtls_entropy_init(&entropy_ctx);
        mbedtls_ctr_drbg_init(&ctr_drbg_ctx);
        mbedtls_pk_init(&pk_ctx);

        prepareEntropy();
        loadPrivateKey();
    }

    virtual ~Signer() {
        mbedtls_pk_free(&pk_ctx);
        mbedtls_ctr_drbg_free(&ctr_drbg_ctx);
        mbedtls_entropy_free(&entropy_ctx);
    }

    Signer(const Signer &) = delete;
    Signer &operator=(const Signer &) = delete;

    void sign(String& content) {
        unsigned char digest[32];
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
        int rc = mbedtls_md(
                md_info,
                (const unsigned char *) content.c_str(),
                content.length(),
                digest
        );

        if (rc != 0) {
            printf("sha256 failure %d: %s\n", rc, mbedErrToStr(rc).c_str());
            content += ".sha256_error";
            return;
        }

        unsigned char sig_buf[512];
        size_t sig_size;
        rc = mbedtls_pk_sign(
                &pk_ctx,
                MBEDTLS_MD_SHA256,
                digest,
                sizeof(digest),
                sig_buf,
                &sig_size,
                mbedtls_ctr_drbg_random,
                &ctr_drbg_ctx
        );
        LI("sig size %u", sig_size);

        if (rc != 0) {
            printf("failed to sign: %d : %s\n", rc, mbedErrToStr(rc).c_str());
            content += ".signature_error";
            return;
        }

        char sig_b64[768];
        base64url_encode(sig_buf, sig_size, sig_b64);

        content += ".";
        content += sig_b64;
    }

    void prepareEntropy() {
        configASSERT(WiFi.isConnected() && "wifi must be enabled for HWRNG");

        uint8_t seed[16];
        esp_fill_random(seed, sizeof(seed));

        int rc = mbedtls_ctr_drbg_seed(
                &ctr_drbg_ctx,
                mbedtls_entropy_func,
                &entropy_ctx,
                (unsigned char *) seed,
                sizeof(seed)
        );

        if (rc != 0) {
            LE("entropy: %d %s", rc, mbedErrToStr(rc).c_str());
        }
    }

    void loadPrivateKey() {
        int rc = mbedtls_pk_parse_key(
                &pk_ctx,
                (unsigned char *) keys::rsa_device_key,
                strlen(keys::rsa_device_key) + 1,
                nullptr,
                0
        );

        if (rc != 0) {
            LE("parsing private key: %d %s", rc, mbedErrToStr(rc).c_str());
        }
    }

    String mbedErrToStr(int code) const {
        char buffer[256];
        mbedtls_strerror(code, buffer, sizeof(buffer));
        return String(buffer);
    }

private:
    mbedtls_entropy_context entropy_ctx{};
    mbedtls_ctr_drbg_context ctr_drbg_ctx{};
    mbedtls_pk_context pk_ctx{};
};


#endif //GATE_SENSOR_SIGNER_H
