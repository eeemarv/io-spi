#ifndef SPI_H
#define SPI_H

#include <string>
#include <mutex>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

class SPINative {
public:
  struct Config {
    uint8_t mode = 0;
    uint32_t speed_hz = 1000000;
    uint8_t bits_per_word = 8;
    uint16_t delay_usecs = 0;
    uint8_t cs_change = 0;
  };

  SPINative(const std::string& device);
  ~SPINative();

  bool isOpen() const;
  bool configure(const Config& config);
  const Config& getConfig() const;
  int transfer(const uint8_t* tx, uint8_t* rx, size_t length);

private:
  std::mutex mutex_;
  int fd_;
  bool open_;
  Config config_;
};

#endif // SPI_H