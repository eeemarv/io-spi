const spi = new SPIController('/dev/spidev0.0');
const rxData = await spi.transfer(Buffer.from([0x01, 0x02, 0x03]));