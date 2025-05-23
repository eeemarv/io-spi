#include "spi_device.h"
#include <sys/file.h>  // for flock()
#include <unistd.h>    // for close()
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>  // For ioctl()
#include <linux/spi/spidev.h>  // For SPI_IOC_WR_MODE etc

void SPIDevice::IoctlOrThrow(unsigned long request, void* arg, const char* action) {
  if (ioctl(fd, request, arg) == -1) {
    std::string errMsg = std::string(action) + " failed: " + strerror(errno);
    Napi::Error::New(Env(), errMsg).ThrowAsJavaScriptException();
  }
}