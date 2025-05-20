#include "spi_device.h"
#include <sys/ioctl.h>
#include <cerrno>
#include <cstring>

namespace {
    std::string GetSystemError() {
        return std::strerror(errno);
    }

    const size_t MAX_SPI_TRANSFER_SIZE = 4096; // Adjust per your hardware

    void ValidateBuffer(Napi::Env env, const Napi::Buffer<uint8_t>& buf) {
        // Check buffer exists
        if (buf.IsEmpty()) {
            throw Napi::Error::New(env, "Buffer is empty");
        }

        // Check buffer size
        if (buf.Length() == 0) {
            throw Napi::Error::New(env, "Buffer cannot be empty");
        }

        if (buf.Length() > MAX_SPI_TRANSFER_SIZE) {
            throw Napi::Error::New(env,
                "Buffer too large (max " + std::to_string(MAX_SPI_TRANSFER_SIZE) + " bytes)");
        }
    }

    void ValidateBitLength(Napi::Env env, uint32_t bits, const std::string& paramName) {
        const uint32_t MIN_BITS = 1;    // Theoretical minimum
        const uint32_t MAX_BITS = 64;   // Linux SPI header limit

        if (bits < MIN_BITS || bits > MAX_BITS) {
            throw Napi::Error::New(env,
                paramName + " must be between " +
                std::to_string(MIN_BITS) + " and " +
                std::to_string(MAX_BITS));
        }

        // Warning for uncommon values
        if (bits != 8 && bits != 16 && bits != 32) {
            std::string warn = "Warning: " + paramName + "=" +
                            std::to_string(bits) +
                            " is unconventional (typical: 8, 16, 32)";
            Napi::Error::New(env, warn);
        }
    }
}

Napi::Value SPIDevice::Transfer(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    auto deferred = Napi::Promise::Deferred::New(env);

    if (info.Length() < 1 || !info[0].IsArray()) {
        Napi::TypeError::New(env, "Array of transfer messages expected")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Array msgArray = info[0].As<Napi::Array>();
    std::vector<spi_ioc_transfer> transfers;
    std::vector<Napi::Reference<Napi::Buffer<uint8_t>>> bufferRefs;
    transfers.reserve(msgArray.Length());
    bufferRefs.reserve(msgArray.Length());

    for (uint32_t i = 0; i < msgArray.Length(); i++) {

        Napi::Value val = msgArray[i];
        spi_ioc_transfer tr = {};

        Napi::Buffer<uint8_t> txBuf;
        Napi::Buffer<uint8_t> rxBuf;

        try {
            if (val.IsBuffer()) {
                // Simple buffer case
                txBuf = val.As<Napi::Buffer<uint8_t>>();
                ValidateBuffer(env, txBuf);
                rxBuf = Napi::Buffer<uint8_t>::New(env, txBuf.Length());
            }
            else if (val.IsObject()) {
                // Configured transfer case
                Napi::Object obj = val.As<Napi::Object>();
                if (!obj.Has("tx_buf") || !obj.Get("tx_buf").IsBuffer()) {
                    throw Napi::Error::New(env, "Transfer object requires tx_buf Buffer");
                }

                txBuf = obj.Get("tx_buf").As<Napi::Buffer<uint8_t>>();
                ValidateBuffer(env, txBuf);
                rxBuf = Napi::Buffer<uint8_t>::New(env, txBuf.Length());

                // Apply optional parameters
                if (obj.Has("speed_hz")){
                    tr.speed_hz = obj.Get("speed_hz").As<Napi::Number>().Uint32Value();
                }

                if (obj.Has("bits_per_word")) {
                    uint32_t bits_per_word = obj.Get("bits_per_word").As<Napi::Number>().Uint32Value();
                    ValidateBitLength(env, bits_per_word, "bits_per_word");
                    tr.bits_per_word = static_cast<uint8_t>(bits_per_word);
                }

                if (obj.Has("delay_usecs")) {
                    uint32_t delay_usecs = obj.Get("delay_usecs").As<Napi::Number>().Uint32Value();

                    // Absolute maximum defined by Linux SPI headers
                    const uint32_t MAX_DELAY_US = 65535;

                    if (delay_usecs > MAX_DELAY_US) {
                        throw Napi::Error::New(env,
                            "delay_usecs cannot exceed " +
                            std::to_string(MAX_DELAY_US) + " µs");
                    }

                    tr.delay_usecs = static_cast<uint16_t>(delay_usecs);
                }

                if (obj.Has("cs_change")) {
                    uint32_t cs_change = obj.Get("cs_change").As<Napi::Number>().Uint32Value();

                    if (cs_change != 0 && cs_change != 1) {
                        throw Napi::Error::New(env,
                            "cs_change must be 0 (keep CS active) or 1 (release CS)");
                    }

                    tr.cs_change = static_cast<uint8_t>(cs_change);
                }

                if (obj.Has("word_delay_usecs")) {
                    uint32_t word_delay_usecs = obj.Get("word_delay_usecs").As<Napi::Number>().Uint32Value();

                    // Kernel-defined maximum (from spidev.h)
                    const uint32_t MAX_WORD_DELAY_US = 255;

                    if (word_delay_usecs > MAX_WORD_DELAY_US) {
                        throw Napi::Error::New(env,
                            "word_delay_usecs cannot exceed " +
                            std::to_string(MAX_WORD_DELAY_US) + " µs");
                    }

                    tr.word_delay_usecs = static_cast<uint16_t>(word_delay_usecs);
                }

                if (obj.Has("tx_nbits")) {
                    uint32_t tx_nbits = obj.Get("tx_nbits").As<Napi::Number>().Uint32Value();
                    ValidateBitLength(env, tx_nbits, "tx_nbits");
                    tr.tx_nbits = static_cast<uint8_t>(tx_nbits);
                }

                if (obj.Has("rx_nbits")) {
                    uint32_t rx_nbits = obj.Get("rx_nbits").As<Napi::Number>().Uint32Value();
                    ValidateBitLength(env, rx_nbits, "rx_nbits");
                    tr.rx_nbits = static_cast<uint8_t>(rx_nbits);
                }
            }
            else {
                throw Napi::Error::New(env, "Each transfer must be a Buffer or Object");
            }

            // Set up transfer struct
            tr.tx_buf = (unsigned long)txBuf.Data();
            tr.rx_buf = (unsigned long)rxBuf.Data();
            tr.len = txBuf.Length();

            transfers.push_back(tr);
            bufferRefs.push_back(Napi::Persistent(rxBuf));
        }
        catch (const Napi::Error& e) {
            e.ThrowAsJavaScriptException();
            return env.Null();
        }
    }

    auto* worker = new TransferWorker(env, this,
        std::move(transfers),
        std::move(bufferRefs),
        deferred);
    worker->Queue();

    return deferred.Promise();
}

void SPIDevice::TransferWorker::Execute() {

    SPI_DEVICE_LOCK_GUARD;

    if (transfers.empty()) {
        SetError("No transfers specified");
        return;
    }

    if (ioctl(device->fd, SPI_IOC_MESSAGE(transfers.size()), transfers.data()) < 1) {
        SetError(std::string("SPI transfer failed: ") + GetSystemError());
    }
}

void SPIDevice::TransferWorker::OnOK() {
    Napi::Env env = Env();
    Napi::Array result = Napi::Array::New(env, bufferRefs.size());

    for (size_t i = 0; i < bufferRefs.size(); i++) {
        result.Set(i, bufferRefs[i].Value());
    }

    deferred.Resolve(result);
}

void SPIDevice::TransferWorker::OnError(const Napi::Error& e) {
    deferred.Reject(e.Value());
}
