/*************************** NOTES ***************************
SAMD21G18A  12-bit ADC --> 4096 steps    (MKR WiFi 1010)
https://docs.arduino.cc/resources/datasheets/ABX00023-datasheet.pdf

MMA7361     basic output = 1.65 V + (0.80 V/g)
https://www.nxp.com/docs/en/data-sheet/MMA7361L.pdf

X --> Short side
Y --> Long side
Z --> Up face

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

#define X_UP 0x10    //--> 2.45V
#define X_DOWN 0x11  //--> 0.85V
//#define X_STLL  0x12
#define Y_UP 0x20
#define Y_DOWN 0x21
//#define Y_STILL 0x22
#define Z_UP 0x30
#define Z_DOWN 0x31
//#define Z_STILL 0x32

#define X_THRES 200
#define Y_THRES 200
#define Z_THRES 200


// Accelerometer values at rest from datasheet, to be overwritten by the calibration routine
uint16_t xRest = 1352;
uint16_t yRest = 1352;
uint16_t zRest = 2007;

const uint8_t PINS[3] = { X_PIN, Y_PIN, Z_PIN };
uint16_t axisVett[3];

uint16_t old_state, new_state;

void setup() {
  Serial.begin(250000);
}

void loop() {

  readAxis(WINDOW_SAMPLES);

  detectRotation();

  sendMessage();
}


void sendMessage() {
  if (old_state == Z_UP) {          // only act upon movements from the calibration position


    if (new_state == X_UP) {
      Serial.print("G+");
    } else if (new_state == X_DOWN) {
      Serial.print("G-");
    } else if (new_state == Y_UP) {
      Serial.print("B+");
    } else if (new_state == Y_DOWN) {
      Serial.print("B-");
    }


  }
}


void detectRotation() {

  int deltaX, deltaY, deltaZ;

  deltaX = axisVett[0] - xRest;
  deltaY = axisVett[1] - yRest;
  deltaZ = axisVett[2] - zRest;

  new_state = STILL;

  if (abs(deltaX) > abs(deltaY)) {  // distinguish X & Y rotations
    if (abs(deltaX) > X_THRES) {    // verify threshold is respected
      if (deltaX > 0) {
        new_state = X_UP;
      } else {
        new_state = X_DOWN;
      }
    }
  } else if (abs(deltaY) > abs(deltaX)) {
    if (abs(deltaY) > Y_THRES) {
      if (deltaY > 0) {
        new_state = Y_UP;
      } else {
        new_state = Y_DOWN;
      }
    }
  } else if ((axisVett[2] > axisVett[0]) && (axisVett[2] > axisVett[1])) {
    new_state = Z_UP;
  }
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

  //Serial to tell the user to keep the glove horizontal
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

  //Serial to notify of the completed calibration

  return;
}

float LPF(float old_avg, float new_val, uint samples) {

  // weighted average of #samples for the given avg and new value

  float new_avg = (old_avg * ((float)samples - 1) + new_val) / (float)samples;
  return new_avg;
}