# Node Native SPI Addon

![TypeScript](https://img.shields.io/badge/types-included-blue.svg)

A high-performance Node.js native addon for SPI communication on Linux, leveraging direct spidev.h APIs.
Features

* Promise-based transfers with configurable per-transfer settings.

* Dynamic reconfiguration of mode, speed_hz, and bits_per_word.

* Full support for Linux SPI parameters (e.g., delay_usecs, cs_change).

* Getters for current device settings.

* Built-in TypeScript type declarations.

## Concurrency & Multiple Chip Select (CS)

### Non-Blocking API

All transfers are __asynchronous__ by design:

```js
import SPIDevice from '..not_yet_published..';
spi = new SPIDevice('/dev/spidev0.0');

// Fire-and-forget transfer
spi.transfer([txBuffer])
   .then([rxBuffer] => console.log('Done!'))
   .catch(err => console.error('Error:', err));

```

### Key notes

* Transfers are queued at the OS level but don't block Node.js's event loop.
* Each transfer() call returns a native Promise (no manual threading required).

---

### Multiple Chip Select (CS) Pins

To control multiple SPI slaves, create separate instances per CS

```js
// Each CS line gets its own instance
// each instance contains its own configuration:
// mode, max_speed_hz and bits_per_word

const spiCS0 = new SPIDevice('/dev/spidev0.0');  // Uses CS0
const spiCS1 = new SPIDevice('/dev/spidev0.1',{
  max_speed_hz: 500_000
});  // Uses CS1

const txBufferForDevice0 = Buffer.from([0x55, 0xAA]);
const txBufferForDevice1 = Buffer.from([0x66, 0xBB]);

// Concurrent operations
Promise.all([
  spiCS0.transfer([txBufferForDevice0]),
  spiCS1.transfer([txBufferForDevice1])
]).then(([[rxBufferfromDevice0], [rxBufferFromDevice1]]) => {
  console.log(rxBufferfromDevice0);
  console.log(rxBufferfromDevice1);
}).catch((error) => {
  console.error(error);
});

```

### Hardware Setup

* Ensure each slave has a dedicated CS line (e.g., CS0, CS1).
* Kernel must expose multiple /dev/spidevX.Y devices (check `ls /dev/spidev* -l`).

__Example Wiring (Raspberry Pi):__

Pi (Master) | Peripheral 0 | Peripheral 1
---|---|---
MOSI (GPIO10) | MOSI | MOSI
MISO (GPIO9) | MISO | MISO
SCLK (GPIO11) | SCLK | SCLK
CE0 (GPIO8) | CS | -
CE1 (GPIO7) | - | CS

## Installation

```bash

#
# Not published on npm yet, will follow later.
#
```

## Prerequisites

* Linux (e.g., Raspberry Pi, Orange Pi) with SPI kernel support.

* Node.js v20+

* Build tools:

```bash
sudo apt-get install build-essential python3
```

## Usage

### Import

```js
const SPIDevice = require('--not-yet-published');
// OR
import SPIDevice from '--not-yet-published';
```

### Initialize

```js
const spi = new SPIDevice('/dev/spidev0.0', {
  mode: 3,          // SPI mode (default 0)
  max_speed_hz: 500_000,  // Clock speed (default 1_000_000 or 1MHz)
  bits_per_word: 16    // Bits per word (default 8)
});
```

Or configure dynamically:

```js

spi.setMode(2);           // Switch to mode 2
spi.setMaxSpeedHz(250_000); // Reduce speed to 250kHz
spi.setBitsPerWord(8);

console.log(spi.getMode()); // e.g., 2
```

### Transfer Data

#### Simple Transfer (Uses Device Defaults)

```javascript
spi.transfer([
  Buffer.from([0x01, 0x02])
]).then(([result]) => {
  console.log(result); // Buffer with received data
}).catch((error) => {
  console.log(error);
});
```

#### Advanced Transfer (Per-Transfer Settings)

```javascript

spi.transfer([
  Buffer.from([0x01, 0x02]), // Uses device defaults
  {                           // Overrides settings for this transfer
    tx_buf: Buffer.from([0x03, 0x04]), // required
    speed_hz: 500000,         // Temporary speed change
    delay_usecs: 100,            // Delay after transfer (microseconds)
    cs_change: true           // Toggle CS after this transfer
  },
  Buffer.from([0x05, 0x06])  // Reverts to device defaults
]).then((results) => {
    console.log(results); // `results` is an array of Buffers (one per transfer)
}).catch((error) => {
    console.log(error);
});
```

## API Reference

### new SPIDevice(path[, options])

* path (string): SPI device path (e.g., /dev/spidev0.0).

* options (object):
  * mode: SPI mode 0-3 (CPOL/CPHA), more rare modes are also supported. Defaults to 0.
  * max_speed_hz (number): Clock speed in Hz. Defaults to 1_000_000 (1Mhz)
  * bits_per_word (number): Bits per word. Defaults to 8

### Methods

Method | Description
---|---
transfer(transfers) | Returns a Promise<Buffer[]> for all transfers. Each transfer can override settings (see below).
setMode(mode) | Sets SPI mode. Throws if invalid.
getMode() | Returns current mode.
setMaxSpeedHz(hz) | Sets clock speed (Hz).
getMaxSpeedHz() | Returns current speed.
setBitsPerWord(bits) | Sets bits per word (usually 8).
getBitsPerWord() | Returns current bits per word.
close() | Closes the SPI device.

### Transfer Object Parameters

Each transfer can specify:

Parameter | Type | Description
---|---|---
`tx_buf` | Buffer | Data to send. Required.
`speed_hz` | number | Temporary clock speed (overrides max_speed_hz).
`delay_usecs` | number | Delay after transfer (microseconds).
`cs_change` | number (0,1) | Toggle chip select after this transfer. default is 0.

See [Linux spidev.h](https://github.com/torvalds/linux/blob/master/include/uapi/linux/spi/spidev.h) for full documentation of all parameters.
Parameters `tx_nbits`, `rx_nbits` and `word_delay_usecs` can also be used, but these are not widely implemented.

## TypeScript Support

This package includes built-in TypeScript type declarations via `index.d.ts`.

If you're using TypeScript, you'll get autocompletion and type checking automatically:

```ts
import SPIDevice from '--not-yet-published';

const spi = new SPIDevice('/dev/spidev0.0', {
  max_speed_hz: 1_000_000,
  mode: 0
});
```

Type definitions include

* Constructor options
* transfer() method with buffer/object overloads
* Getter/setter methods for mode, speed, and bits-per-word

No need to install @types/... — types are bundled with the package.

## Examples

### Loopback Test

With this test you can see if the SPI device works without the involvement of a slave device. Connect the MOSI pin directly to the MISO pin, run the test and see if the data matches.

```bash
node examples/loopback.js
```

## Troubleshooting

### Permission Denied

If you get a "Permission denied" error when trying to open `/dev/spidev0.0`, it's likely because your user doesn't have the right group permissions.

#### ✅ Step 1: Check device permissions

List SPI devices:

```bash
ls -l /dev/spidev*
```

Example output:

```bash
crw-rw----+ 1 root dialout 153, 0 ... /dev/spidev0.0
```

This shows the device belongs to group dialout.

#### ✅ Step 2: Add your user to the dialout group

```bash
sudo usermod -aG dialout $USER
```

On Raspberry Pi OS or other systems that use spi or gpio groups (e.g., Raspberry Pi OS with Desktop):

```bash
sudo usermod -aG spi,gpio $USER
```

  You can check which groups exist by running `getent group` or `grep spi /etc/group`.

#### ✅ Step 3: Log out and back in

Group changes only apply after re-logging into your session, or you can reboot:

```bash
sudo reboot
```

### Invalid Arguments

Verify parameters match spidev.h constraints (e.g., valid mode).

### Build Issues

Rebuild with `node-gyp rebuild --verbose`

## License

MIT
