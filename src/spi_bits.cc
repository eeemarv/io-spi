#include "spi_device.h"
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>  // For SPI_IOC_WR_MODE etc

Napi::Value SPIDevice::SetBitsPerWord(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Bits per word (usually 8) expected")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    uint8_t bits = info[0].As<Napi::Number>().Uint32Value();

    if (ioctl(this->fd, SPI_IOC_WR_BITS_PER_WORD, &bits) == -1) {
        Napi::Error::New(env, "Failed to set bits per word")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    return env.Undefined();
}

Napi::Value SPIDevice::GetBitsPerWord(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    uint8_t bits;

    if (ioctl(this->fd, SPI_IOC_RD_BITS_PER_WORD, &bits) == -1) {
        Napi::Error::New(env, "Failed to read bits per word")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::Number::New(env, bits);
}