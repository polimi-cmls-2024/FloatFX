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
  *************************************************************/



/*************************** NOTES ***************************
      Inclination mapped to the output value
 **************************************************************/



// HARDWARE DEFINITIONS
#define X_PIN A0
#define Y_PIN A1
#define Z_PIN A2


// SOFTWARE DEFINITIONS
#define WINDOW_SAMPLES 20
#define SETTLE_TIME 3000  //Time in ms for the calibration

#define X_UP 0x10    //--> 2.45V
#define X_DOWN 0x11  //--> 0.85V
#define Y_UP 0x20
#define Y_DOWN 0x21
#define Z_UP 0x30
#define Z_DOWN 0x31

#define X_THRES 200
#define Y_THRES 200
#define Z_THRES 200
#define MAP_SIZE 10

// Accelerometer values at rest from datasheet, to be overwritten by the calibration routine
uint16_t xRest = 1352;
uint16_t yRest = 1352;
uint16_t zRest = 2007;

const uint8_t PINS[3] = { X_PIN, Y_PIN, Z_PIN };    // pins array
uint16_t axisVett[3];                               // axis values

uint16_t old_state, new_state;
int16_t value;                                      // relative value to set, from 0 to 100

const int16_t value_map[2][MAP_SIZE] = {  (300, 10),
                                          (330, 20),
                                          (370, 30),
                                          (410, 40),
                                          (450, 50),
                                          (480, 60),
                                          (520, 70),
                                          (570, 80),
                                          (630, 90),
                                          (650, 100)
                                       };

void setup() {

  Serial.begin(250000);
  calibrateAccelerometer();
  old_state = Z_UP;
  new_state = Z_UP;
  value = 0;

}

void loop() {

  readAxis(WINDOW_SAMPLES);

  detectRotation();

  sendMessage();

}

int16_t readMap(int16_t delta) {

  uint8_t min_index = 0, max_index = 0;
  uint16_t val = 0;

  delta = abs(delta);

  for (int i = 0; i < (MAP_SIZE - 1); i++) {
    if ((value_map[0][i] < delta)  &&  (value_map[0][i+1] > delta)) {
      min_index = i;
      max_index = i+1;
      break;
    }
  }

  val = 0.5 * (value_map[1][min_index] + value_map[1][max_index]);

  return val;

}


void sendMessage() {
  if (old_state == Z_UP && new_state != Z_UP) {          // only act upon movements from the calibration position


    if (new_state == X_UP) {            // X fingertips, Y palm
      Serial.print("G+");
    } else if (new_state == X_DOWN) {
      Serial.print("G-");
    } else if (new_state == Y_UP) {
      Serial.print("B+");
    } else if (new_state == Y_DOWN) {
      Serial.print("B-");
    }

    Serial.println(value);
  }

  old_state = new_state;
}


void detectRotation() {

  int deltaX, deltaY, deltaZ;

  deltaX = axisVett[0] - xRest;
  deltaY = axisVett[1] - yRest;
  deltaZ = axisVett[2] - zRest;


  new_state = Z_UP;

  if (abs(deltaX) > abs(deltaY)) {  // distinguish X & Y rotations
    if (abs(deltaX) > X_THRES) {    // verify threshold is respected
      if (deltaX > 0) {
        new_state = X_UP;
      } else {
        new_state = X_DOWN;
      }
      value = readMap(deltaX);      // compute the target value to set
    }
  } else if (abs(deltaY) > abs(deltaX)) {
    if (abs(deltaY) > Y_THRES) {
      if (deltaY > 0) {
        new_state = Y_UP;
      } else {
        new_state = Y_DOWN;
      }
      value = readMap(deltaY);
    }
  } else if ((axisVett[2] > axisVett[0]) && (axisVett[2] > axisVett[1])) {
    new_state = Z_UP;
  }
}


void readAxis(uint16_t samples, uint16_t axis) {

  for (int j = 0; j < samples; j++) {
    axisVett[axis] = LPF(axisVett[axis], analogRead(PINS[axis]), samples);
  }

  return;
}

void readAxis(uint16_t samples) {

  for (int j = 0; j < samples; j++) {
    for (int i = 0; i < 3; i++) {
      axisVett[i] = LPF(axisVett[i], analogRead(PINS[i]), samples);
    }
  }

  return;
}


void calibrateAccelerometer() {

  Serial.println("Inizio calibrazione");
  Serial.println("Mantenere la mano orizzontale");

  // Time for the user to settle
  Serial.println("3");
  delay(1000);

  Serial.println("2");
  delay(1000);

  Serial.println("1");
  delay(1000);

  uint32_t samplingStart = millis();
  while ((millis() - samplingStart) < SETTLE_TIME) {

    // Low-pass on the read values
    xRest = LPF(xRest, analogRead(X_PIN), WINDOW_SAMPLES);
    yRest = LPF(yRest, analogRead(Y_PIN), WINDOW_SAMPLES);
    zRest = LPF(zRest, analogRead(Z_PIN), WINDOW_SAMPLES);
  }

  Serial.println("Calibrazione completata");

  return;
}

float LPF(float old_avg, float new_val, uint16_t samples) {
  // weighted average of #samples for the given avg and new value
  float new_avg = (old_avg * ((float)samples - 1) + new_val) / (float)samples;
  return new_avg;
}
