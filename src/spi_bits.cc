#include "spi_device.h"
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>  // For SPI_IOC_WR_MODE etc

void SPIDevice::SetBitsPerWordInternal(uint8_t bits) {
  uint8_t read_bits = 0;
  IoctlOrThrow(SPI_IOC_RD_BITS_PER_WORD, &read_bits, "SPI_IOC_RD_BITS_PER_WORD");

  if (Env().IsExceptionPending()){
    return;
  }// stop on error

  if (read_bits != bits) {
    IoctlOrThrow(SPI_IOC_WR_BITS_PER_WORD, &bits, "SPI_IOC_WR_BITS_PER_WORD");
  }
}

Napi::Value SPIDevice::SetBitsPerWord(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  SPI_LOCK_GUARD;

  if (info.Length() < 1 || !info[0].IsNumber()) {
    Napi::TypeError::New(env, "Bits per word (usually 8) expected")
      .ThrowAsJavaScriptException();
    return env.Null();
  }

  uint32_t bits = info[0].As<Napi::Number>().Uint32Value();

  ValidateBitsPerWord(env, bits);

  SetBitsPerWordInternal(static_cast<uint8_t>(bits));

  return env.IsExceptionPending() ? env.Null() : env.Undefined();
}

Napi::Value SPIDevice::GetBitsPerWord(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  SPI_LOCK_GUARD;

  uint8_t bits;
  IoctlOrThrow(SPI_IOC_RD_BITS_PER_WORD, &bits, "SPI_IOC_RD_BITS_PER_WORD");

  if (env.IsExceptionPending()){
    return env.Null();
  }

  return Napi::Number::New(env, bits);
}