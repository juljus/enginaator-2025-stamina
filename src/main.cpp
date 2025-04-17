#include <Arduino.h>
#include <arduinoFFT.h>

// defines samples (N) and sampling frequency (f_s)
#define SAMPLES 64
#define SAMPLING_FREQ 4000  // 4000 samples per second (Hz). Detects max freq that can be detected 
// with 2 samples per cycle (Nyquist theorem) = 2000 Hz

// define the analog pin for audio input
#define AUDIO_PIN A5

arduinoFFT FFT = arduinoFFT();
// FFT works in complex nrs, so we need to define real and imaginary parts
// x_n = a_n + ib_n
double vReal[SAMPLES];
double vImag[SAMPLES];

int bars[8];  // array to store the height of each bar

// Simulate different sine wave frequencies for each bar (bin center frequency)
const float testFrequencies[8] = {
  125.0,  // Bar 0: bins 0–3 → ~125 Hz
  375.0,  // Bar 1: bins 4–7 → ~375 Hz
  625.0,  // Bar 2: bins 8–11 → ~625 Hz
  875.0,  // Bar 3: bins 12–15 → ~875 Hz
 1125.0,  // Bar 4: bins 16–19 → ~1125 Hz
 1375.0,  // Bar 5: bins 20–23 → ~1375 Hz
 1625.0,  // Bar 6: bins 24–27 → ~1625 Hz
 1875.0   // Bar 7: bins 28–31 → ~1875 Hz
};

int currentTest = 0; // current sine wave index
unsigned long lastSwitchTime = 0;
const unsigned long switchInterval = 500; // switch wave every 0.5 seconds

// converts the sampling frequency into microseconds per sample:
void captureSamples() {
  for (int i = 0; i < SAMPLES; i++) {
    float t = (float)i / SAMPLING_FREQ; // actual time in seconds

    // Simulate sine wave for a selected frequency
    vReal[i] = 500 * sin(2 * PI * testFrequencies[currentTest] * t);
    vImag[i] = 0;
  }
}

void processFFT() {
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
}

void groupToBands() {
  for (int i = 0; i < 8; i++) {
    int start = i * (SAMPLES / 2) / 8;
    int end = (i + 1) * (SAMPLES / 2) / 8;
    double sum = 0;
    for (int j = start; j < end; j++) {
      sum += vReal[j];
    }

    double avg = sum / (end - start);
    avg = constrain(avg, 0, 800);  // Adjust range based on observed magnitudes
    bars[i] = map(avg, 0, 800, 0, 8);  // Rescale to 0–8
  }
}

// row pins
int rows[8] = {9, 8, 7, 6, 5, 4, 3, 2};
// col pins
int cols[8] = {10, 11, 12, 13, A0, A1, A2, A3};

void lightLed(int row, int col) {
  for (int i = 0; i < 8; i++) {
    if (i != row) {
      digitalWrite(rows[i], LOW);
    }
  }
  for (int i = 0; i < 8; i++) {
    if (i != col) {
      digitalWrite(cols[i], HIGH);
    }
  }
  digitalWrite(rows[row], 1);
  digitalWrite(cols[col], 0);
}

void lightCols(int cols[8]) {
  for (int i = 0; i < 8; i++) {
    for (int j = 7; j > -1; j--) {
      if (j < cols[i]) {
        lightLed(j, i);
        delayMicroseconds(1000); // Allow LED to visibly light up
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  for (int i = 0; i < 8; i++) {
    pinMode(rows[i], OUTPUT);
    pinMode(cols[i], OUTPUT);
  }
}

void loop() {
  // Change simulated wave every few seconds
  if (millis() - lastSwitchTime > switchInterval) {
    currentTest = (currentTest + 1) % 8;
    lastSwitchTime = millis();
    Serial.print("Switched to frequency: ");
    Serial.println(testFrequencies[currentTest]);
  }

  captureSamples();
  processFFT();
  groupToBands();

  for (int i = 0; i < 8; i++) {
    Serial.print(bars[i]);
    Serial.print(" ");
  }
  Serial.println();

  lightCols(bars);
}
