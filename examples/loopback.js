// @ts-check
"use strict";

/**
 * Loopback test: simple test if the SPI device sends data
 * Connect the MOSI to the MISO pin
 * The data send should match the data received.
 *
 * Run with:
 * node examples/looback.js
 *
 * Optional flags:
 *
 * --speed Set the maximum clock speed with --speed=<number>
 * E.g speed=4_000_000 for 4Mhz. The default is 1Mhz.
 *
 * --bits To select the "bits per word". The default is 8.
 * Select from 8, 16 or 32
 *
 * --mode To select a SPI mode. The default is 0.
 * Select from 0, 1, 2 or 3.
 *
 * --device Set the device e.g --device=/dev/spidev0.1
 * The default device is /dev/spidev0.0
 */

import SPIDevice from '../index.mjs';

const testData = [
  [0x3a, 0xf7, 0x42, 0x91, 0x5e, 0xc8, 0x2d, 0xb0],
  [0x67, 0x1f, 0x9d, 0xe4, 0x33, 0xaa, 0x76, 0x08],
  [0xcd, 0x50, 0xb2, 0x7f, 0x19, 0x84, 0xeb, 0x26],
  [0x72, 0x39, 0xa5, 0x0c, 0xd8, 0x63, 0xfe, 0x47],
  [0x15, 0x8e, 0x21, 0xda, 0x54, 0xbf, 0x30, 0x9a],
  [0x4d, 0xe7, 0x82, 0x3b, 0x69, 0xf0, 0x5c, 0x97],
  [0x0a, 0x75, 0xd1, 0x6e, 0xbb, 0x24, 0x8f, 0x52],
  [0x38, 0xc3, 0x7a, 0x95, 0x01, 0xec, 0x57, 0xa2]
];

(async () => {
  let speed = 1_000_000;
  let mode = 0;
  let bits = 8;
  let device = '/dev/spidev0.0';

  const args = process.argv.slice(2);

  args.forEach((arg, i) => {
    if (arg.startsWith('--speed')) {
      const speedStr = arg.includes('=') ? arg.split('=')[1] : args[i + 1];
      if (speedStr){
        speed = Number(speedStr.replace(/_/g, ''));
      }
      return;
    }

    if (arg.startsWith('--mode')) {
      const modeStr = arg.includes('=') ? arg.split('=')[1] : args[i + 1];
      if (modeStr){
        mode = Number(modeStr);
      }
      return;
    }

    if (arg.startsWith('--bits')) {
      const bitsStr = arg.includes('=') ? arg.split('=')[1] : args[i + 1];
      if (bitsStr){
        bits = Number(bitsStr);
      }
      return;
    }

    if (arg.startsWith('--device')) {
      const deviceStr = arg.includes('=') ? arg.split('=')[1] : args[i + 1];
      if (deviceStr){
        device = deviceStr;
      }
    }
  });

  const spi = new SPIDevice(device, {
    max_speed_hz: speed,
    bits_per_word: bits,
    mode
  });

  console.log(`SPI device: \x1b[1;33m${device}\x1b[0m`);
  console.log(`SPI max speed Hz: \x1b[1;33m${spi.getMaxSpeedHz()}\x1b[0m`);
  console.log(`SPI Mode: \x1b[1;33m${spi.getMode()}\x1b[0m`);
  console.log(`SPI bits per word: \x1b[1;33m${spi.getBitsPerWord()}\x1b[0m`);
  console.log('\x1b[1;36m-- Loopback test --\x1b[0m');

  const testBuffers = testData.map((a) => Buffer.from(a));
  const result = await spi.transfer(testBuffers);

  let pass = true;

  for (let a = 0; a < testData.length; a++){
    const tAry = testData[a];
    const rAry = result[a];
    let inStr = '\x1b[36mIn:  ';
    let outStr = '\x1b[33mOut:\x1b[0m ';
    for (let b = 0; b < tAry.length; b++){
      const vIn = tAry[b];
      const vOut = rAry[a];
      inStr += vIn.toString(16).padStart(2, '0');
      inStr += ' ';
      if (vIn === vOut){
        outStr += '\x1b[32m';
      } else {
        outStr += '\x1b[31m';
        pass = false;
      }
      outStr += vOut.toString(16).padStart(2, '0');
      outStr += '\x1b[0m ';
    }
    inStr += '\x1b[0m';
    console.log(inStr);
    console.log(outStr);
  }

  if (pass){
    console.log('\x1b[1;32mThe test passed\x1b[0m');
  } else {
    console.log('\x1b[1;31mThe test failed\x1b[0m');
  }
})();
