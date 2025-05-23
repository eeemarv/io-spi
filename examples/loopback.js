import SPIDevice from '../index.mjs';

const spi = new SPIDevice('/dev/spidev0.0');

spi.transfer([
    Buffer.from([0x01, 0x02]), // Simple transfer
    {                           // Configured transfer
        tx_buf: Buffer.from([0x03, 0x04]), // required
        speed_hz: 500000,
        delay_usecs: 100
    },
    Buffer.from([0x05, 0x06, 0x07, 0x08, 0x09]),  // Another simple transfer
    Buffer.from([0x55, 0xAA]),  // Yet another simple transfer
]).then((results) => {
    console.log(results);
}).catch((error) => {
    console.error(error);
});
