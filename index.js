const { SPI } = require('./build/Release/spi.node');

class SPIDevice {
  constructor(device = '/dev/spidev0.0', options = {}) {
    this.native = new SPI(device);
    if (!this.native.isOpen()) {
      throw new Error(`Failed to open SPI device ${device}`);
    }

    this.configure(options);
  }

  configure(options = {}) {
    const config = {
      mode: options.mode !== undefined ? options.mode : 0,
      speedHz: options.speedHz || 1000000,
      bitsPerWord: options.bitsPerWord || 8,
      delayUsecs: options.delayUsecs || 0,
      csChange: options.csChange ? 1 : 0
    };

    if (!this.native.configure(config)) {
      throw new Error('Failed to configure SPI device');
    }
  }

  getConfig() {
    return this.native.getConfig();
  }

  transferSync(txBuffer) {
    if (!Buffer.isBuffer(txBuffer)) {
      throw new TypeError('Expected Buffer for transfer');
    }
    return this.native.transferSync(txBuffer);
  }

  close() {
    this.native.close();
  }
}

module.exports = SPIDevice;