import SPI from '../../index.js';

const spi = new SPI('/dev/spidev0.0', {
    max_speed_hz: 2_000_000
});

function buildAddress(addr, read = false) {
  return Buffer.from([(addr << 1 & 0x7E) | (read ? 0x80 : 0x00)]);
}

async function readRegister(addr) {
  // Send address with read bit set, then dummy byte to read back data
  const [_, data] = await spi.transfer([buildAddress(addr, true), Buffer.from([0x00])]);
  return data[0];
}

(async () => {
  try {
    const versionReg = 0x37;
    const version = await readRegister(versionReg);
    console.log('MFRC522 version register: 0x' + version.toString(16).padStart(2, '0'));

    if (version === 0x91 || version === 0x92) {
      console.log('MFRC522 detected successfully!');
    } else {
      console.warn('Unexpected version — check wiring or SPI settings.');
    }
  } catch (err) {
    console.error('SPI communication failed:', err);
  }
})();
