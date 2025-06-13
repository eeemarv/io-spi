#include "spi_device.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>  // For ioctl()
#include <linux/spi/spidev.h>  // For SPI_IOC_WR_MODE etc

void SPIDevice::SetModeInternal(uint8_t mode) {
  uint8_t read_mode = 0;
  IoctlOrThrow(SPI_IOC_RD_MODE, &read_mode, "SPI_IOC_RD_MODE");

  if (Env().IsExceptionPending()){
    return;
  } // stop on error

  if (read_mode != mode) {
    IoctlOrThrow(SPI_IOC_WR_MODE, &mode, "SPI_IOC_WR_MODE");
  }
}

Napi::Value SPIDevice::SetMode(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Mode number (0-3) expected")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    uint32_t mode = info[0].As<Napi::Number>().Uint32Value();

    ValidateMode(env, mode);

    SetModeInternal(static_cast<uint8_t>(mode));

    return env.IsExceptionPending() ? env.Null() : env.Undefined();
}

Napi::Value SPIDevice::GetMode(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    uint8_t mode;
    IoctlOrThrow(SPI_IOC_RD_MODE, &mode, "SPI_IOC_RD_MODE");

    if (env.IsExceptionPending()){
        return env.Null();
    }

    return Napi::Number::New(env, mode);
}
