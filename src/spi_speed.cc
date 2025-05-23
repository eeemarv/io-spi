#include "spi_device.h"
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>  // For SPI_IOC_WR_MODE etc

void SPIDevice::SetMaxSpeedHzInternal(uint32_t speed) {
  uint32_t read_speed = 0;
  IoctlOrThrow(SPI_IOC_RD_MAX_SPEED_HZ, &read_speed, "SPI_IOC_RD_MAX_SPEED_HZ");

  if (Env().IsExceptionPending()){
    return;
  } // stop on error

  if (read_speed != speed) {
    IoctlOrThrow(SPI_IOC_WR_MAX_SPEED_HZ, &speed, "SPI_IOC_WR_MAX_SPEED_HZ");
  }
}

Napi::Value SPIDevice::SetMaxSpeedHz(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    if (info.Length() < 1 || !info[0].IsNumber()) {
        Napi::TypeError::New(env, "Speed in Hz expected")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    uint32_t speed = info[0].As<Napi::Number>().Uint32Value();
    SetMaxSpeedHzInternal(speed);

    return env.IsExceptionPending() ? env.Null() : env.Undefined();
}

Napi::Value SPIDevice::GetMaxSpeedHz(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    SPI_LOCK_GUARD;

    uint32_t speed;
    IoctlOrThrow(SPI_IOC_RD_MAX_SPEED_HZ, &speed, "SPI_IOC_RD_MAX_SPEED_HZ");

    if (env.IsExceptionPending()){
        return env.Null();
    }

    return Napi::Number::New(env, speed);
}
