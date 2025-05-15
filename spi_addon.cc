#include <napi.h>
#include "spi.h"

class SPIWrapper : public Napi::ObjectWrap<SPIWrapper> {
public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  SPIWrapper(const Napi::CallbackInfo& info);

  // Configuration methods
  Napi::Value Configure(const Napi::CallbackInfo& info);
  Napi::Value GetConfig(const Napi::CallbackInfo& info);

  // Transfer methods
  Napi::Value Transfer(const Napi::CallbackInfo& info);
  Napi::Value TransferSync(const Napi::CallbackInfo& info);

  // Status methods
  Napi::Value IsOpen(const Napi::CallbackInfo& info);
  void Close(const Napi::CallbackInfo& info);

private:
  static Napi::FunctionReference constructor;
  SPINative* spi_;

  Napi::Object ConfigToObject(Napi::Env env, const SPINative::Config& config);
};

Napi::FunctionReference SPIWrapper::constructor;

Napi::Object SPIWrapper::Init(Napi::Env env, Napi::Object exports) {
  Napi::HandleScope scope(env);

  Napi::Function func = DefineClass(env, "SPI", {
    InstanceMethod("configure", &SPIWrapper::Configure),
    InstanceMethod("getConfig", &SPIWrapper::GetConfig),
    InstanceMethod("transfer", &SPIWrapper::Transfer),
    InstanceMethod("transferSync", &SPIWrapper::TransferSync),
    InstanceMethod("isOpen", &SPIWrapper::IsOpen),
    InstanceMethod("close", &SPIWrapper::Close)
  });

  constructor = Napi::Persistent(func);
  constructor.SuppressDestruct();

  exports.Set("SPI", func);
  return exports;
}

SPIWrapper::SPIWrapper(const Napi::CallbackInfo& info)
  : Napi::ObjectWrap<SPIWrapper>(info), spi_(nullptr) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsString()) {
    Napi::TypeError::New(env, "Device path string expected").ThrowAsJavaScriptException();
    return;
  }

  std::string device = info[0].As<Napi::String>();
  spi_ = new SPINative(device);

  if (!spi_->isOpen()) {
    Napi::Error::New(env, "Failed to open SPI device").ThrowAsJavaScriptException();
  }
}

Napi::Value SPIWrapper::Configure(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsObject()) {
    Napi::TypeError::New(env, "Configuration object expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::Object configObj = info[0].As<Napi::Object>();
  SPINative::Config config;

  if (configObj.Has("mode")) {
    config.mode = configObj.Get("mode").As<Napi::Number>().Uint32Value();
  }
  if (configObj.Has("speedHz")) {
    config.speed_hz = configObj.Get("speedHz").As<Napi::Number>().Uint32Value();
  }
  if (configObj.Has("bitsPerWord")) {
    config.bits_per_word = configObj.Get("bitsPerWord").As<Napi::Number>().Uint32Value();
  }
  if (configObj.Has("delayUsecs")) {
    config.delay_usecs = configObj.Get("delayUsecs").As<Napi::Number>().Uint32Value();
  }
  if (configObj.Has("csChange")) {
    config.cs_change = configObj.Get("csChange").As<Napi::Number>().Uint32Value();
  }

  bool success = spi_->configure(config);
  return Napi::Boolean::New(env, success);
}

Napi::Value SPIWrapper::GetConfig(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return ConfigToObject(env, spi_->getConfig());
}

Napi::Value SPIWrapper::Transfer(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();

  if (info.Length() < 1 || !info[0].IsBuffer()) {
    Napi::TypeError::New(env, "Buffer expected").ThrowAsJavaScriptException();
    return env.Null();
  }

  Napi::Buffer<uint8_t> txBuffer = info[0].As<Napi::Buffer<uint8_t>>();
  Napi::Buffer<uint8_t> rxBuffer = Napi::Buffer<uint8_t>::New(env, txBuffer.Length());

  int status = spi_->transfer(txBuffer.Data(), rxBuffer.Data(), txBuffer.Length());

  if (status < 0) {
    Napi::Error::New(env, "SPI transfer failed").ThrowAsJavaScriptException();
    return env.Null();
  }

  return rxBuffer;
}

Napi::Value SPIWrapper::TransferSync(const Napi::CallbackInfo& info) {
  return Transfer(info); // Same as Transfer in our implementation
}

Napi::Value SPIWrapper::IsOpen(const Napi::CallbackInfo& info) {
  Napi::Env env = info.Env();
  return Napi::Boolean::New(env, spi_->isOpen());
}

void SPIWrapper::Close(const Napi::CallbackInfo& info) {
  delete spi_;
  spi_ = nullptr;
}

Napi::Object SPIWrapper::ConfigToObject(Napi::Env env, const SPINative::Config& config) {
  Napi::Object obj = Napi::Object::New(env);
  obj.Set("mode", Napi::Number::New(env, config.mode));
  obj.Set("speedHz", Napi::Number::New(env, config.speed_hz));
  obj.Set("bitsPerWord", Napi::Number::New(env, config.bits_per_word));
  obj.Set("delayUsecs", Napi::Number::New(env, config.delay_usecs));
  obj.Set("csChange", Napi::Number::New(env, config.cs_change));
  return obj;
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  return SPIWrapper::Init(env, exports);
}

NODE_API_MODULE(spi, Init)