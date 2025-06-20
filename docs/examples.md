# Examples

To keep the npm package light, the loopback test is the only only one included. For the other examples see (the repository on Github)[https://github.com/eeemarv/io-spi].

To install from Github:

```bash
git clone https://github.com/eeemarv/io-spi.git
cd io-spi
npm install
```

## Loopback

![Loopback test on a Orange Pi 3 Zero](https://raw.githubusercontent.com/eeemarv/io-spi/main/images/opiz3_loopback.webp)

For this simple test, connect the MOSI pin to the
MISO pin and run

```bash
node loopback.js
```

The output data should match the input data.

![Loopback Test Terminal](https://raw.githubusercontent.com/eeemarv/io-spi/main/images/loopback.png)

Use the --help or -h argument to get information about the
options you can set

```bash
node loopback.js --help
Usage: node loopback.js [options]

Loopback test: simple test if the SPI device sends data
Connect the MOSI to the MISO pin
The data send should match the data received.

Options:
  --speed=<number>, -s=<number>    Set the maximum clock speed in Hz. Default is 1_000_000 (1MHz).
  --bits=<8|16|32>, -b=<8|16|32>   Set the bits per word. Default is 8.
  --mode=<0|1|2|3>, -m=<0|1|2|3>   Set the SPI mode. Default is 0.
  --device=<path>, -d=<path>       Set the SPI device path. Default is /dev/spidev0.0.
  --help, -h                       Show this help message.
```

## MFRC522

![MFRC522 test on a Orange Pi 3 Zero](https://raw.githubusercontent.com/eeemarv/io-spi/main/images/opiz3_rc522.webp)

The RC522 module (with MFRC522 NXP chip) can
communicate with the contactless Mifare tags.
This test performs a self test and then scans for
tag UIDs (4, 7 or 10 bytes).

For this example, you don't connect the RESET (RST) and IRQ. Only control via the SPI bus is needed.

```bash
node examples/mfrc522.js
```

![MFRC522 Test Terminal](https://raw.githubusercontent.com/eeemarv/io-spi/main/images/mfrc522.png)


The default device is `/dev/spidev0.0` and can be changed
with the `--device` flag. The `max_speed_hz` of 10Mhz can be change with the `--speed` flag (`--speed=<number>`). If the
self test fails (in case of a clone MFRC522), it can be
disabled with `--no-self-test`. See all options with `--help` or `-h`.


## ILI9341

![Loopback test on a Orange Pi 3 Zero](https://raw.githubusercontent.com/eeemarv/io-spi/main/images/opiz3_ili9341.webp)

In this example we draw a fractal on a small ILI3941
display and even zoom in the fractal frame by frame.

These small display modules run on 3.3V but usually
have a voltage regulator that can convert from 5v to 3.3v.
There is a small jumper J1 on the back to disable the
voltage regulator so the display can run directly on 3.3v.

Apart from the SPI interface also 2 GPIO lines are needed, DC, to select betweem Data or Command, and RESET.
The example uses package [node-libgpiod](https://www.npmjs.com/package/node-libgpiod) to control the GPIOs through the
`/dev/gpiochipX` character device.

The user needs permissions for access to `/dev/gpiochipX`.

Create a special gpiochip group:

```bash
sudo groupadd gpiochip
```

And add a file to the udev rules:

```bash
# /etc/udev/rules.d/85-gpiochip.rules
KERNEL=="gpiochip*", SUBSYSTEM=="gpio", MODE="0660", GROUP="gpiochip"
```

Add the user to the gpiochip group:

```bash
sudo usermod -aG gpiochip $USER
```
Then you need to find out the gpiochip and  GPIO pin numbers you are going to use.

E.g. on the Orange Pi 3 Zero in the image, pin PC7 is connected to DC and pin PC10 to RESET.
GPIO Pin numbers on the Orange Pi 3 Zero are found as follows:

```typescript
GPIO number = (Port number × 32) + pin number
```

When using PC7 for DC and PC10 for RESET (Port C = 2)

| Pin  | Port | Pin# on port | GPIO# = 2\*32 + pin# |
| ---- | ---- | ------------ | -------------------- |
| PC7  | 2    | 7            | 64 + 7 = 71          |
| PC10 | 2    | 10           | 64 + 10 = 74         |

`--dc-pin` and `--reset-pin` are then required arguments for running the example

```bash
node examples/ili9341.js --dc-pin=71 --reset-pin=74 --device=/dev/spidev1.1 --frames=24
```

`--help` shows all possible options.
