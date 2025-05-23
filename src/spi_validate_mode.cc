#include "spi_device.h"

namespace spiutil {

// Valid SPI mode flags — you can adjust if needed
constexpr uint32_t VALID_MODE_MASK =
    SPI_CPHA | SPI_CPOL | SPI_CS_HIGH |
    SPI_LSB_FIRST | SPI_3WIRE | SPI_LOOP |
    SPI_NO_CS | SPI_READY;

inline void ValidateMode(uint32_t mode, Napi::Env env) {
    if (mode & ~VALID_MODE_MASK) {
        throw Napi::Error::New(env, "Invalid SPI mode flags: unknown or unsupported bits set");
    }

    // Warn on unusual modes
    if (mode & SPI_LOOP) {
        Napi::Error::New(env, "SPI_LOOP is intended for testing and not normal operation")
            .ThrowAsJavaScriptException();
    }

    if ((mode & SPI_NO_CS) && (mode & SPI_CS_HIGH)) {
        Napi::Error::New(env, "Cannot combine SPI_NO_CS with SPI_CS_HIGH").ThrowAsJavaScriptException();
        return;
    }

    if ((mode & SPI_LSB_FIRST) && !(VALID_MODE_MASK & SPI_LSB_FIRST)) {
        Napi::Error::New(env, "SPI_LSB_FIRST may not be supported on this platform")
            .ThrowAsJavaScriptException();
    }

    // additional hardware-specific checks?
}

} // namespace spiutil

uint8_t SPIDevice::ParseMode(const Napi::Value& val) {
    Napi::Env env = val.Env();
    if (!val.IsNumber()) {
        throw Napi::TypeError::New(env, "'mode' must be a number");
    }
    uint32_t mode = val.As<Napi::Number>().Uint32Value();
    ValidateMode(env, mode);
    return static_cast<uint8_t>(mode);
}