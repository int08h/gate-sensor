#ifndef GATE_SENSOR_JWT_H
#define GATE_SENSOR_JWT_H

#include <array>
#include <cstdio>
#include <ctime>
#include <WString.h>

#include "b64url.h"
#include "constants.h"
#include "mlog.h"
#include "signer.h"

class Jwt {
 public:
  Jwt() = default;

  Jwt(const Jwt &) = delete;
  Jwt &operator=(const Jwt &) = delete;

  bool isExpired(time_t now) const {
    return now > exp;
  }

  void regenerate(time_t now) {
    auto start = millis();
    LI("JWT generate start");

    iat = now;
    exp = now + c::JWT_VALID_SEC;

    char claims[64];
    snprintf(claims, sizeof(claims), c::JWT_CLAIMS_FMT, iat, exp, c::GCP_PROJECT);
    LI("JWT claims : %s", claims);

    char claims_b64[96];
    base64url_encode((unsigned char *) claims, strlen(claims), claims_b64);

    String newJwt = String{};
    newJwt.reserve(512);
    newJwt += c::JWT_RSA_HEADER_B64;
    newJwt += claims_b64;

    Signer signer{secrets::rsa_device_key};
    signer.sign(newJwt);

    jwt.fill(0);
    memcpy(jwt.data(), newJwt.c_str(), newJwt.length());

    auto elapsed = millis() - start;
    LI("JWT generate finish; %u ms", elapsed);
    LI("JWT = %s", jwt.data());
  }

  const char *value() {
    return jwt.data();
  }

  String asHeader() {
    return String("Bearer ") + jwt.data();
  }

 private:
  std::array<char, 512> jwt;
  time_t iat;
  time_t exp;
};

#endif //GATE_SENSOR_JWT_H
