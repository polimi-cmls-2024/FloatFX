/*************************** NOTES ***************************
SAMD21G18A  12-bit ADC --> 4096 steps    (MKR WiFi 1010)
https://docs.arduino.cc/resources/datasheets/ABX00023-datasheet.pdf

MMA7361     basic output = 1.65 V + (0.80 V/g)
https://www.nxp.com/docs/en/data-sheet/MMA7361L.pdf

zeroG_ADCValue  = 1.65V --> 1.65/5 * 4095 = 1352
oneG_ADCValue   = 2.45V --> 2.45/5 * 4095 = 2007
/*************************************************************/


// HARDWARE DEFINITIONS
#define X_PIN A0
#define Y_PIN A1
#define Z_PIN A2


// SOFTWARE DEFINITIONS
#define WINDOW_SAMPLES 100
#define SETTLE_TIME 3000  //Time in ms for the calibration



// Accelerometer values at rest from datasheet, to be overwritten by the calibration routine
uint16_t xRest = 1352;
uint16_t yRest = 1352;
uint16_t zRest = 2007;

const uint8_t PINS[3] = { X_PIN, Y_PIN, Z_PIN };
uint16_t axisVett[3];
int8_t rotVett[3];

void setup() {
  // put your setup code here, to run once:
}

void loop() {

  readAxis(WINDOW_SAMPLES);

  //detectRotation();
  
}


void readAxis(uint16_t samples, uint axis) {

  for (uint j = 0; j < samples; j++) {
    axisVett[axis] = LPF(axisVett[axis], analogRead(PINS[axis]), samples);
  }

  return;
}

void readAxis(uint16_t samples) {

  for (uint j = 0; j < samples; j++) {
    for (uint i = 0; i < 3; i++) {
      axisVett[i] = LPF(axisVett[i], analogRead(PINS[i]), samples);
    }
  }

  return;
}


void calibrateAccelerometer() {

  //Serial OSC to tell the user to keep the glove horizontal
  //countdown instead of one delay maybe(?)
  // Time for the user to settle
  delay(3000);

  uint32_t samplingStart = millis();
  while ((millis() - samplingStart) < SETTLE_TIME) {

    // Low-pass on the read values
    xRest = LPF(xRest, analogRead(X_PIN), WINDOW_SAMPLES);
    yRest = LPF(yRest, analogRead(Y_PIN), WINDOW_SAMPLES);
    zRest = LPF(zRest, analogRead(Z_PIN), WINDOW_SAMPLES);
  }

  for (uint i = 0; i < 3; i++) {
    rotVett[i] = 0;
  }

  //Serial OSC to notify of the completed calibration

  return;
}

float LPF(float old_avg, float new_val, uint samples) {

  // weighted average of #samples for the given avg and new value

  float new_avg = (old_avg * (samples - 1) + new_val) / samples;
  return new_avg;
}