/// <reference types="node" />

/**
 * Optional parameters for individual SPI transfers.
 * Mirrors the Linux `spi_ioc_transfer` structure (except `pad`).
 */
export interface SPITransfer {
  /** Transmit buffer (required) */
  tx_buf: Buffer;

  /** Delay after transfer, in microseconds */
  delay_usecs?: number;

  /** SPI speed in Hz, overrides default for this transfer */
  speed_hz?: number;

  /** Number of bits per word, overrides default for this transfer */
  bits_per_word?: number;

  /** SPI chip select change flag (0 = keep asserted, 1 = release between messages) */
  cs_change?: 0 | 1;

  /**
   * Word delay in microseconds between words.
   * This option is Raspberry Pi–specific and may not be supported on other platforms.
   */
  word_delay_usecs?: number;

  /**
   * Number of bits per word to transmit.
   * Rarely supported outside some specialized hardware.
   */
  tx_nbits?: number;

  /**
   * Number of bits per word to receive.
   * Rarely supported outside some specialized hardware.
   */
  rx_nbits?: number;
}


/**
 * Options for configuring the SPI device at initialization.
 */
export interface SPIDeviceOptions {
  /**
   * Default max clock speed in Hz.
   * @default 1000000
   */
  max_speed_hz?: number;

  /**
   * Default bits per word.
   * @default 8
   */
  bits_per_word?: number;

  /**
   * SPI mode (0–3).
   * Mode 0: CPOL = 0, CPHA = 0
   * Mode 1: CPOL = 0, CPHA = 1
   * Mode 2: CPOL = 1, CPHA = 0
   * Mode 3: CPOL = 1, CPHA = 1
   * @default 0
   */
  mode?: 0 | 1 | 2 | 3;
}

/**
 * Represents an SPI device using a Linux SPI interface.
 */
export default class SPIDevice {
  /**
   * Create a new SPI device instance.
   *
   * @param device Path to the SPI device (e.g., `/dev/spidev0.0`)
   * @param options Optional default configuration. If omitted:
   * - `max_speed_hz` defaults to `1000000`
   * - `bits_per_word` defaults to `8`
   * - `mode` defaults to `0`
   */
  constructor(device: string, options?: SPIDeviceOptions);

  /**
   * Perform a full-duplex SPI transfer.
   * Each element in the array can be a Buffer or a detailed transfer object.
   * @param transfers Buffers or SPITransfer objects
   * @returns A Promise resolving to an array of Buffers received from the SPI device
   */
  transfer(transfers: (Buffer | SPITransfer)[]): Promise<Buffer[]>;

  // --- Configuration Getters and Setters ---

  /** Set SPI mode (0–3) */
  setMode(mode: 0 | 1 | 2 | 3): void;

  /** Get current SPI mode */
  getMode(): 0 | 1 | 2 | 3;

  /** Set max SPI speed (Hz) */
  setMaxSpeedHz(hz: number): void;

  /** Get current max SPI speed (Hz) */
  getMaxSpeedHz(): number;

  /** Set bits per word */
  setBitsPerWord(bits: number): void;

  /** Get current bits per word */
  getBitsPerWord(): number;
}
