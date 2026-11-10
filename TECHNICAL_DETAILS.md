# Technical Details: Audio Spectrum Visualizer

This document explains the underlying mechanics of the ESP32 Audio Spectrum Visualizer.

## 1. Audio Data Acquisition (I2S)
The project utilizes the **I2S (Inter-IC Sound)** peripheral on the ESP32 to interface directly with the digital microphone.
- **Configuration**: The `i2s_config_t` structure sets the sample rate (16kHz), bit depth (32-bit), and channel format (left channel).
- **Reading**: The `i2s_read` function fills the `sBuffer` array. This is a non-blocking operation that allows the ESP32 to process data efficiently.

## 2. Signal Processing (FFT)
To convert the raw time-domain audio samples into frequency bands, we use the **Fast Fourier Transform (FFT)**.
- **Pre-processing**: We map the raw 32-bit I2S data into the `vReal` array, applying a bit-shift (`>> 14`) to adjust the scale.
- **Windowing**: A **Hamming window** is applied before the FFT computation to reduce spectral leakage, resulting in a more accurate frequency representation.
- **Computation**: `FFT.compute()` processes the signal, and `FFT.complexToMagnitude()` converts the complex FFT output into a usable magnitude array.

## 3. Frequency Band Mapping
The raw FFT output provides magnitudes for 512 frequency bins. To display these effectively, we group them into 16 wider "bands":
- **Noise Gate**: A threshold check (`vReal[i] > 100`) filters out background noise.
- **Bin Accumulation**: We aggregate the magnitudes of bins falling within pre-defined frequency ranges (`cutoffs[]`) into the `bandValues[]` array.

## 4. Visualization and Display
- **Logarithmic Scaling**: Audio perception is logarithmic. We convert the linear magnitude to a decibel scale (`20 * log10(...)`) and map this value to the screen height.
- **Peak Hold**: To make the visualization more readable, the `peakBands[]` array stores the highest point reached by each bar, which slowly decays over time (`peakBands[i]--`) to provide a "flicker" effect.
- **Rendering**: The `Adafruit_SSD1306` library draws the final bar graph and peak lines to the OLED display.
