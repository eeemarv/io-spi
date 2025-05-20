#include "spi_device.h"
#include <cerrno>
#include <cstring>

void SPIDevice::ValidateBitsPerWord(Napi::Env env, uint32_t bits) {
    if (bits == 0 || bits > 32) {
        throw Napi::RangeError::New(env, "'bits_per_word' must be between 1 and 32");
    }
}

uint8_t SPIDevice::ParseBitsPerWord(const Napi::Value& val) {
    Napi::Env env = val.Env();
    if (!val.IsNumber()) {
        throw Napi::TypeError::New(env, "'bits_per_word' must be a number");
    }
    uint32_t bits = val.As<Napi::Number>().Uint32Value();
    ValidateBitsPerWord(env, bits);
    return static_cast<uint8_t>(bits);
}
