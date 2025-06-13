#include "spi_device.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>  // For ioctl()
#include <linux/spi/spidev.h>  // For SPI_IOC_WR_MODE etc

Napi::Object SPIDevice::Init(Napi::Env env, Napi::Object exports) {
  Napi::Function func = DefineClass(env, "SPIDevice", {
    InstanceMethod("setMode", &SPIDevice::SetMode),
    InstanceMethod("getMode", &SPIDevice::GetMode),
    InstanceMethod("setBitsPerWord", &SPIDevice::SetBitsPerWord),
    InstanceMethod("getBitsPerWord", &SPIDevice::GetBitsPerWord),
    InstanceMethod("setMaxSpeedHz", &SPIDevice::SetMaxSpeedHz),
    InstanceMethod("getMaxSpeedHz", &SPIDevice::GetMaxSpeedHz),
    InstanceMethod("transfer", &SPIDevice::Transfer)
  });

  // Static constants (attached to class itself)
  func.DefineProperties({
    Napi::PropertyDescriptor::Value("SPI_MODE_0", Napi::Number::New(env, SPI_MODE_0), napi_enumerable),
    Napi::PropertyDescriptor::Value("SPI_MODE_1", Napi::Number::New(env, SPI_MODE_1), napi_enumerable),
    Napi::PropertyDescriptor::Value("SPI_MODE_2", Napi::Number::New(env, SPI_MODE_2), napi_enumerable),
    Napi::PropertyDescriptor::Value("SPI_MODE_3", Napi::Number::New(env, SPI_MODE_3), napi_enumerable),

    Napi::PropertyDescriptor::Value("SPI_CPHA", Napi::Number::New(env, SPI_CPHA), napi_enumerable),
    Napi::PropertyDescriptor::Value("SPI_CPOL",Napi::Number::New(env, SPI_CPOL), napi_enumerable),
    Napi::PropertyDescriptor::Value("SPI_CS_HIGH",Napi::Number::New(env, SPI_CS_HIGH), napi_enumerable),
    Napi::PropertyDescriptor::Value("SPI_LSB_FIRST", Napi::Number::New(env, SPI_LSB_FIRST), napi_enumerable),
    Napi::PropertyDescriptor::Value("SPI_3WIRE", Napi::Number::New(env, SPI_3WIRE), napi_enumerable),
    Napi::PropertyDescriptor::Value("SPI_LOOP", Napi::Number::New(env, SPI_LOOP), napi_enumerable),
    Napi::PropertyDescriptor::Value("SPI_NO_CS", Napi::Number::New(env, SPI_NO_CS), napi_enumerable),
    Napi::PropertyDescriptor::Value("SPI_READY", Napi::Number::New(env, SPI_READY), napi_enumerable)
  });

  static Napi::FunctionReference constructor;
  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("SPIDevice", func);
  return exports;
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return SPIDevice::Init(env, exports);
}

NODE_API_MODULE(spi, InitAll)
