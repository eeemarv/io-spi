#include "spi_device.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>  // For ioctl()
#include <linux/spi/spidev.h>  // For SPI_IOC_WR_MODE etc

Napi::Value SPIDevice::SetMode(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Mode number (0-3) expected")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    uint8_t mode = info[0].As<Napi::Number>().Uint32Value() & 0x03;

    if (ioctl(this->fd, SPI_IOC_WR_MODE, &mode) == -1) {
        Napi::Error::New(env, "Failed to set SPI mode")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Undefined();
}

Napi::Value SPIDevice::GetMode(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    uint8_t mode;

    if (ioctl(this->fd, SPI_IOC_RD_MODE, &mode) == -1) {
        Napi::Error::New(env, "Failed to read SPI mode")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, mode);
}