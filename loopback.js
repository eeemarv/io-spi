// @ts-check
"use strict";

/**
 * Loopback test: simple test if the SPI device sends data
 * Connect the MOSI to the MISO pin
 * The data send should match the data received.
 *
 * Run with:
 * node looback.js
 *
 * Optional flags:
 *
 * --speed, -s Set the maximum clock speed with --speed=<number>
 * E.g speed=4_000_000 for 4Mhz. The default is 1Mhz.
 *
 * --bits, -b To select the "bits per word". The default is 8.
 * Select from 8, 16 or 32
 *
 * --mode, -m To select a SPI mode. The default is 0.
 * Select from 0, 1, 2 or 3.
 *
 * --device, -d Set the device e.g --device=/dev/spidev0.1
 * The default device is /dev/spidev0.0
 */

import SPIDevice from '@eeemarv/io-spi';

/**
 * Test data to send and receive. (just some random bytes)
 * Each array is a separate transfer.
 * The data is sent on the MOSI pin and received on the MISO pin.
 * The data should match when the MOSI is connected to the MISO pin.
 */
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

const showHelp = () => {
  console.log(`Usage: node loopback.js [options]

Loopback test: simple test if the SPI device sends data
Connect the MOSI to the MISO pin
The data send should match the data received.

Options:
  --speed=<number>, -s=<number>    Set the maximum clock speed in Hz. Default is 1_000_000 (1MHz).
  --bits=<8|16|32>, -b=<8|16|32>   Set the bits per word. Default is 8.
  --mode=<0|1|2|3>, -m=<0|1|2|3>   Set the SPI mode. Default is 0.
  --device=<path>, -d=<path>       Set the SPI device path. Default is /dev/spidev0.0.
  --help, -h                       Show this help message. `);
};

(async () => {
  let speed = 1_000_000;
  let mode = 0;
  let bits = 8;
  let device = '/dev/spidev0.0';
  let skipArg = false;

  const args = process.argv.slice(2);

  try {
    args.forEach((arg, i) => {
      let key = undefined;
      let value = undefined;
      if (skipArg) {
        skipArg = false;
        return;
      }

      if (arg.includes('=')) {
        // If the argument is in the form --key=value, we can skip the next argument
        [key, value] = arg.split('=');
      } else {
        // If the argument is in the form --key value, we need to check the next argument
        key = arg;
        value = args[i + 1];
        skipArg = true; // Skip the next argument since it's the value for this key
      }

      if (key === '--speed' || key === '-s') {
        if (!value) {
          throw new Error('Missing value for --speed');
        }
        speed = Number(value.replace(/_/g, ''));
        return;
      }

      if (key === '--mode' || key === '-m') {
        if (!value) {
          throw new Error('Missing value for --mode');
        }
        mode = Number(value);
        if (![0, 1, 2, 3].includes(mode)) {
          throw new Error('Invalid mode. Use 0, 1, 2 or 3.');
        }
        return;
      }

      if (key === '--bits' || key === '-b') {
        if (!value) {
          throw new Error('Missing value for --bits');
        }
        bits = Number(value);
        if (![8, 16, 32].includes(bits)) {
          throw new Error('Invalid bits per word. Use 8, 16 or 32.');
        }
        return;
      }

      if (key === '--device' || key === '-d') {
        if (!value) {
          throw new Error('Missing value for --device');
        }
        device = value;
        return;
      }

      if (arg == '--help' || arg === '-h') {
        showHelp();
        process.exit(0);
      }

      throw new Error(`Unknown argument: ${arg}`);
    });

  } catch (err) {
    console.error('\x1b[1;31mError: \x1b[0m', err.message);
    showHelp();
    process.exit(1);
  }

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
      const vOut = rAry[b];
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
