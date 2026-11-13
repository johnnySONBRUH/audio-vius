# ESP32 Audio Spectrum Visualizer

An embedded real-time audio spectrum analyzer built for the ESP32 platform. This project captures digital audio via an I2S microphone (e.g., INMP441), performs a Fast Fourier Transform (FFT) analysis to compute signal magnitude across frequency spectrums, and renders a 16-band visual bar graph with dynamic peak-hold indicators on an SSD1306 OLED display.

---

## Features

- **High-Quality I2S Sampling**: Captures 32-bit digital audio directly into DMA buffers at 16 kHz sampling frequency.
- **Real-Time FFT Signal Processing**: Employs 512-point Fast Fourier Transform with Hamming windowing.
- **Logarithmic Frequency Banding**: Maps FFT bins across 16 human-perceptual frequency bands using customizable cutoffs.
- **Logarithmic Scale Display**: Converts linear magnitude outputs to Decibels (dB) to reflect human pitch/loudness perception.
- **Dynamic Peak-Hold Indicators**: Smooth animated peak markers that drop slowly over time.
- **Noise Gating**: Built-in noise thresholding to eliminate ambient static and dynamic background noise.

---

## Hardware Requirements

| Component | Recommendation / Model | Notes |
| :--- | :--- | :--- |
| **Microcontroller** | ESP32 Board | ESP32 Dev Module, ESP32-S3, or similar |
| **Microphone** | INMP441 / MSM261S4030H0 | I2S Digital MEMS Microphone |
| **Display** | SSD1306 OLED | 128x64 resolution, I2C interface |
| **Interconnects** | Jumper Wires & Breadboard | Short connection wires recommended |

---

## Wiring Diagram / Pinout

### 1. I2S Microphone Setup

| INMP441 Pin | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **VDD** | 3.3V | Power Supply |
| **GND** | GND | Ground |
| **SD** | GPIO 6 | Serial Data |
| **SCK** | GPIO 5 | Serial Clock / Bit Clock |
| **WS** | GPIO 4 | Word Select / Left Right Clock |
| **L/R** | GND | Left Channel Selection |

### 2. SSD1306 OLED Display Setup (I2C)

| Display Pin | ESP32 Pin | Function |
| :--- | :--- | :--- |
| **VCC** | 3.3V or 5V | Power Supply |
| **GND** | GND | Ground |
| **SDA** | GPIO 8 | I2C Data Line |
| **SCL** | GPIO 9 | I2C Clock Line |

*Note: Pins can be adjusted in code.ino to suit your specific ESP32 development board mapping.*

---

## Software Dependencies & Setup

### Required Arduino Libraries

Ensure you have installed the following libraries via the **Arduino IDE Library Manager**:

1. **Adafruit SSD1306** (by Adafruit)
2. **Adafruit GFX Library** (by Adafruit)
3. **ArduinoFFT** (by kosme)

### Flashing Instructions

1. Connect your ESP32 board to your computer via USB.
2. Open code.ino in the **Arduino IDE**.
3. Select your ESP32 board under **Tools > Board** (e.g., *ESP32 Dev Module*).
4. Verify pin configurations in code.ino align with your hardware wiring.
5. Click **Upload**.

---

## Technical Overview & Architecture

For a deep-dive explanation on the project architecture, mathematical calculations (FFT, dB scaling), buffer management, and code execution flow, please refer to [TECHNICAL_DETAILS.md](TECHNICAL_DETAILS.md).

---

## License

This project is open-source and available under the MIT License.
