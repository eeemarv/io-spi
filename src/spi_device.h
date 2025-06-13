#ifndef SPI_DEVICE_H
#define SPI_DEVICE_H

#include <napi.h>
#include <vector>
#include <linux/spi/spidev.h>

#define SPI_LOCK_GUARD std::lock_guard<std::mutex> lock(this->mutex)
#define SPI_DEVICE_LOCK_GUARD std::lock_guard<std::mutex> lock(device->mutex)

class SPIDevice : public Napi::ObjectWrap<SPIDevice> {

public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  SPIDevice(const Napi::CallbackInfo& info);
  virtual ~SPIDevice();

  Napi::Value SetMode(const Napi::CallbackInfo& info);
  Napi::Value GetMode(const Napi::CallbackInfo& info);
  Napi::Value SetBitsPerWord(const Napi::CallbackInfo& info);
  Napi::Value GetBitsPerWord(const Napi::CallbackInfo& info);
  Napi::Value SetMaxSpeedHz(const Napi::CallbackInfo& info);
  Napi::Value GetMaxSpeedHz(const Napi::CallbackInfo& info);
  Napi::Value Transfer(const Napi::CallbackInfo& info);

private:
  int fd = -1;
  std::mutex mutex;

  void IoctlOrThrow(unsigned long request, void* arg, const char* action);
  void SetModeInternal(uint8_t mode);
  void SetMaxSpeedHzInternal(uint32_t speed);
  void SetBitsPerWordInternal(uint8_t bits);

  static void ValidateMode(Napi::Env env, uint32_t mode);
  static void ValidateBitsPerWord(Napi::Env env, uint32_t bits);
  static void ValidateMaxSpeedHz(Napi::Env env, uint32_t speed);
  static uint32_t ParseBitsPerWord(const Napi::Value& val);
  static uint32_t ParseMaxSpeedHz(const Napi::Value& val);
  static uint32_t ParseMode(const Napi::Value& val);

  class TransferWorker : public Napi::AsyncWorker {
    public:
      TransferWorker(Napi::Env env, SPIDevice* device,
          std::vector<spi_ioc_transfer>&& transfers,
          std::vector<Napi::Reference<Napi::Buffer<uint8_t>>>&& txRefs,
          std::vector<Napi::Reference<Napi::Buffer<uint8_t>>>&& rxRefs,
          Napi::Promise::Deferred deferred)
        : Napi::AsyncWorker(env),
        device(device),
        transfers(std::move(transfers)),
        txRefs(std::move(txRefs)),
        rxRefs(std::move(rxRefs)),
        deferred(deferred) {}

      void Execute() override;
      void OnOK() override;
      void OnError(const Napi::Error& e) override;

    private:
      SPIDevice* device;
      std::vector<spi_ioc_transfer> transfers;
      std::vector<Napi::Reference<Napi::Buffer<uint8_t>>> txRefs;
      std::vector<Napi::Reference<Napi::Buffer<uint8_t>>> rxRefs;
      Napi::Promise::Deferred deferred;
  };
};

#endif