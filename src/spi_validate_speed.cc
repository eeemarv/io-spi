#include "spi_device.h"
#include <cerrno>
#include <cstring>

void SPIDevice::ValidateMaxSpeedHz(Napi::Env env, uint32_t speed) {
    if (speed == 0) {
        throw Napi::RangeError::New(env, "'max_speed_hz' must be greater than 0");
    }
}

uint32_t SPIDevice::ParseMaxSpeedHz(const Napi::Value& val) {
    Napi::Env env = val.Env();
    if (!val.IsNumber()) {
        throw Napi::TypeError::New(env, "'max_speed_hz' must be a number");
    }
    uint32_t speed = val.As<Napi::Number>().Uint32Value();
    ValidateMaxSpeedHz(env, speed);
    return speed;
}
