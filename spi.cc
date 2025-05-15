#include "spi.h"

SPINative::SPINative(const std::string& device) : fd_(-1), open_(false) {
  fd_ = open(device.c_str(), O_RDWR);
  open_ = (fd_ >= 0);

  // Default configuration
  config_ = {
    .mode = 0,
    .speed_hz = 1000000,
    .bits_per_word = 8,
    .delay_usecs = 0,
    .cs_change = 0
  };
}

SPINative::~SPINative() {
  if (open_) close(fd_);
}

bool SPINative::isOpen() const {
  return open_;
}

const SPINative::Config& SPINative::getConfig() const {
  return config_;
}

bool SPINative::configure(const Config& config) {
  if (!open_) return false;
  std::lock_guard<std::mutex> lock(mutex_);

  if (ioctl(fd_, SPI_IOC_WR_MODE, &config.mode) == -1) return false;
  if (ioctl(fd_, SPI_IOC_WR_MAX_SPEED_HZ, &config.speed_hz) == -1) return false;
  if (ioctl(fd_, SPI_IOC_WR_BITS_PER_WORD, &config.bits_per_word) == -1) return false;

  config_ = config;
  return true;
}

int SPINative::transfer(const uint8_t* tx, uint8_t* rx, size_t length) {
  if (!open_) return -1;

  spi_ioc_transfer tr = {
    .tx_buf = (unsigned long)tx,
    .rx_buf = (unsigned long)rx,
    .len = static_cast<uint32_t>(length),
    .speed_hz = config_.speed_hz,
    .delay_usecs = config_.delay_usecs,
    .bits_per_word = config_.bits_per_word,
    .cs_change = config_.cs_change
  };

  return ioctl(fd_, SPI_IOC_MESSAGE(1), &tr);
}