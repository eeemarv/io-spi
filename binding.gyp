{
  "targets": [{
    "target_name": "spi",
    "sources": [
        "src/spi_mode.cc",
        "src/spi_bits.cc",
        "src/spi_speed.cc",
        "src/spi_validate_mode.cc",
        "src/spi_validate_bits.cc",
        "src/spi_validate_speed.cc",
        "src/spi_init.cc",
        "src/spi_device.cc",
        "src/spi_transfer.cc"
    ],
    "include_dirs": [
      "<!@(node -p \"require('node-addon-api').include_dir\")",
    ],
    "dependencies": [
      "<!(node -p \"require('node-addon-api').gyp\")"
    ],
    "cflags": ["-fexceptions"],
    "cflags_cc": ["-fexceptions"],
    "defines": [
      "NAPI_CPP_EXCEPTIONS",  # ← Required define
      "NAPI_VERSION=8"  # Match Node.js version (4 for Node 14+, 8 for Node 16+)
    ]
  }]
}