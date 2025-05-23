#include "spi_device.h"
#include <sys/file.h>  // for flock()
#include <unistd.h>    // for close()
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>  // For ioctl()
#include <linux/spi/spidev.h>  // For SPI_IOC_WR_MODE etc

SPIDevice::SPIDevice(const Napi::CallbackInfo& info)
    : Napi::ObjectWrap<SPIDevice>(info) {

    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    if (info.Length() < 1 || !info[0].IsString()) {
        Napi::TypeError::New(env, "Device path string expected")
            .ThrowAsJavaScriptException();
        return;
    }

    std::string device = info[0].As<Napi::String>().Utf8Value();
    this->fd = open(device.c_str(), O_RDWR);

    if (this->fd < 0) {
        Napi::Error::New(env, "Failed to open SPI device")
            .ThrowAsJavaScriptException();
    }

    if (flock(this->fd, LOCK_EX | LOCK_NB) < 0) {
        close(this->fd);
        this->fd = -1;  // Prevent dangling fd usage
        Napi::Error::New(env, "SPI device is already locked by another process")
            .ThrowAsJavaScriptException();
        return;
    }

    // --- Defaults
    uint8_t mode = 0;
    uint8_t bits = 8;
    uint32_t speed = 1000000;

    // --- Optional second argument: options object
    if (info.Length() >= 2 && info[1].IsObject()) {
        Napi::Object options = info[1].As<Napi::Object>();

        if (options.Has("mode")) {
            mode = ParseMode(options.Get("mode"));
        }

        if (options.Has("bits_per_word")) {
            bits = ParseBitsPerWord(options.Get("bits_per_word"));
        }

        if (options.Has("max_speed_hz")) {
            speed = ParseMaxSpeedHz(options.Get("max_speed_hz"));
        }
    }

    // --- Set options
    SetModeInternal(mode);
    if (env.IsExceptionPending()) {
        return;
    }

    SetMaxSpeedHzInternal(speed);
    if (env.IsExceptionPending()){
        return;
    }

    SetBitsPerWordInternal(bits);
    if (env.IsExceptionPending()){
        return;
    }
}

SPIDevice::~SPIDevice() {
    if (this->fd >= 0) {
        close(this->fd);
        this->fd = -1;  // Prevent reuse
    }
}
