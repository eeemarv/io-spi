#include "spi_device.h"
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>  // For SPI_IOC_WR_MODE etc

Napi::Value SPIDevice::SetMaxSpeedHz(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Speed in Hz expected")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    uint32_t speed = info[0].As<Napi::Number>().Uint32Value();
    if (ioctl(this->fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed) == -1) {
        Napi::Error::New(env, "Failed to set speed")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Undefined();
}

Napi::Value SPIDevice::GetMaxSpeedHz(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    uint32_t speed;

    if (ioctl(this->fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed) == -1) {
        Napi::Error::New(env, "Failed to read speed")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, speed);
}