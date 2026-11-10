#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <driver/i2s.h>
#include "arduinoFFT.h"
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define I2C_SDA 8
#define I2C_SCL 9
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define I2S_WS 4
#define I2S_SCK 5
#define I2S_SD 6
#define I2S_PORT I2S_NUM_0
#define SAMPLES 512        
#define SAMPLING_FREQ 16000 
double vReal[SAMPLES];
double vImag[SAMPLES];
int32_t sBuffer[SAMPLES];
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, SAMPLES, SAMPLING_FREQ);
#define NUM_BANDS 16
int bandValues[NUM_BANDS];
int peakBands[NUM_BANDS];
int cutoffs[NUM_BANDS] = {3, 4, 6, 8, 11, 15, 20, 28, 38, 52, 72, 98, 134, 182, 245, 255};
void setup() {
  Serial.begin(115200);
  Wire.begin(I2C_SDA, I2C_SCL);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.setRotation(2); 
  display.clearDisplay();
  display.display();
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLING_FREQ,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = SAMPLES,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };
  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}
void loop() {
  size_t bytesIn = 0;
  esp_err_t result = i2s_read(I2S_PORT, &sBuffer, sizeof(sBuffer), &bytesIn, portMAX_DELAY);
  if (result == ESP_OK) {
    int samples_read = bytesIn / 4; 
    for (int i = 0; i < SAMPLES; i++) {
      if (i < samples_read) {
        vReal[i] = (double)(sBuffer[i] >> 14); 
      } else {
        vReal[i] = 0.0;
      }
      vImag[i] = 0.0; 
    }
    FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);
    FFT.compute(FFTDirection::Forward);
    FFT.complexToMagnitude();
    for (int i = 0; i < NUM_BANDS; i++) {
      bandValues[i] = 0;
    }
    for (int i = 2; i < (SAMPLES / 2); i++) { 
      if (vReal[i] > 100) { // Noise gate
        int band = 0;
        while (band < NUM_BANDS && i > cutoffs[band]) {
          band++;
        }
        if (band < NUM_BANDS) {
          bandValues[band] += (int)vReal[i];
        }
      }
    }
    display.clearDisplay();
    int barWidth = SCREEN_WIDTH / NUM_BANDS;
    const int MIN_DB = -80;
    const int MAX_DB = 45; 
    float referenceMax = 50000.0; 
    for (int i = 0; i < NUM_BANDS; i++) {
      int scaledValue = 0;
      if (bandValues[i] > 0) {
        float fraction = (float)bandValues[i] / referenceMax;
        int dbValue = 20 * log10(fraction);
        scaledValue = map(dbValue, MIN_DB, MAX_DB, 0, SCREEN_HEIGHT);
        scaledValue = constrain(scaledValue, 0, SCREEN_HEIGHT);
      }
      if (scaledValue > peakBands[i]) {
        peakBands[i] = scaledValue;
      } else if (peakBands[i] > 0) {
        peakBands[i]--;
      }
      int flippedIndex = (NUM_BANDS - 1) - i;
      int x = flippedIndex * barWidth;
      display.fillRect(x, SCREEN_HEIGHT - scaledValue, barWidth - 1, scaledValue, SSD1306_WHITE);
      display.drawFastHLine(x, SCREEN_HEIGHT - peakBands[i], barWidth - 1, SSD1306_WHITE);
    }
    display.display();
  }
}