/*************************** NOTES ***************************
  SAMD21G18A  12-bit ADC --> 4096 steps    (MKR WiFi 1010)
      CONFIGURED FOR 10-bit --> 1023 steps
  https://docs.arduino.cc/resources/datasheets/ABX00023-datasheet.pdf

  MMA7361     basic output = 1.65 V + (0.80 V/g)
  https://www.nxp.com/docs/en/data-sheet/MMA7361L.pdf

  X --> Short side
  Y --> Long side
  Z --> Up face

  zeroG_ADCValue  = 1.65V --> 1.65/5 * 1023 = 338
  oneG_ADCValue   = 2.45V --> 2.45/5 * 1023 = 500
  *************************************************************/



/*************************** NOTES ***************************
      Inclination mapped to the output value
 **************************************************************/

#include <math.h>


// HARDWARE DEFINITIONS
#define X_PIN A5
#define Y_PIN A4
#define Z_PIN A3
#define FINGER_PIN A6


// SOFTWARE DEFINITIONS
#define WINDOW_SAMPLES 10
#define SETTLE_TIME 3000  //Time in ms for the calibration

#define X_UP 0x10    //--> 2.45V
#define X_DOWN 0x11  //--> 0.85V
#define Y_UP 0x20
#define Y_DOWN 0x21
#define Z_UP 0x30
#define Z_DOWN 0x31
#define OPEN 0x40  // finger
#define CLOSED 0x41

#define X_THRES 50
#define Y_THRES 50
#define Z_THRES 50
#define MAP_SIZE 100

// Accelerometer values at rest from datasheet, to be overwritten by the calibration routine
uint16_t xRest = 338;
uint16_t yRest = 338;
uint16_t zRest = 500;

const uint8_t PINS[3] = { X_PIN, Y_PIN, Z_PIN };  // pins array
uint16_t axisVett[3];                             // axis values

uint16_t old_state, new_state, finger_state;
int16_t value_x=0, value_y=0;  // relative value to set, from 0 to 100

float value_map[MAP_SIZE][2] = {
  // mapping the delta w.r.t the resting position to the output percentage

};



uint8_t knobX = 1, knobY = 1;  // which knob is active
int16_t fixed_value_x=0, fixed_value_y=0;

void setup() {

  Serial.begin(250000);

  pinMode(A6, INPUT_PULLUP);

  calibrateAccelerometer();
  old_state = Z_UP;
  new_state = Z_UP;
  finger_state = OPEN;
  
  float maxDelta = 220;
  float minDelta = 50;
  float increment = (maxDelta - minDelta) / 100;
  for (int i = 0; i < MAP_SIZE; i++) {
    value_map[i][0] = minDelta + i * increment;
    value_map[i][1] = 100*sin((M_PI*(1.29/440)*(value_map[i][0] - minDelta)));
    Serial.print(value_map[i][0]);
    Serial.print(", ");
    Serial.println(value_map[i][1]);
  }
  pinMode(FINGER_PIN, INPUT_PULLUP);
}

void loop() {

  readAxis(WINDOW_SAMPLES);

  detectRotation();

  sendMessage();
}

int16_t readMap(int16_t delta) {

  uint8_t min_index = 0, max_index = 0;
  static uint16_t val = 0;

  delta = abs(delta);

  for (int i = 0; i < (MAP_SIZE - 1); i++) {
    if ((value_map[i][0] < delta) && (value_map[i + 1][0] > delta)) {
      min_index = i;
      max_index = i + 1;
      val = 0.5 * (value_map[min_index][1] + value_map[max_index][1]);
      return val;
    }
  }

  if ((value_map[MAP_SIZE - 1][0] < delta)) {
    val = value_map[MAP_SIZE - 1][1];
  }




  return val;
}


void sendMessage() {
  //if (old_state == Z_UP && new_state != Z_UP) {          // only act upon movements from the calibration position


  switch (new_state) {
    case X_UP:
      if(knobX){
        Serial.print("G+");
        Serial.println(value_x);
      }
      break;
    case X_DOWN:
      if(knobX){
        Serial.print("G-");
        Serial.println(value_x);
      }

      break;
    case Y_UP:
      
      if(knobY){
        Serial.print("B+");
        Serial.println(value_y);
      } 
      break;
    case Y_DOWN:
      
      if(knobY){
        Serial.print("B-");
        Serial.println(value_y);
      } 
      break;
  }
  

  //}
  old_state = new_state;
}


void detectRotation() {

  int16_t deltaX = axisVett[0] - xRest;
  int16_t deltaY = axisVett[1] - yRest;
  int16_t deltaZ = axisVett[2] - zRest;

  int16_t deltaX_abs = abs(deltaX);
  int16_t deltaY_abs = abs(deltaY);

  uint8_t deltaX_sign = (deltaX > 0);
  uint8_t deltaY_sign = (deltaY > 0);

  uint8_t pinch = !digitalRead(FINGER_PIN);
  if(pinch)
    delay(200);

  new_state = Z_UP;

  if (deltaX_abs > deltaY_abs) {  // distinguish X & Y rotations
    if (deltaX_abs > X_THRES) {   // verify threshold is respected
      if (deltaX_sign) {
        new_state = X_UP;
      } else {
        new_state = X_DOWN;
      }
    }
  } else if (deltaY_abs > deltaX_abs) {
    if (deltaY_abs > Y_THRES) {
      if (deltaY_sign) {
        new_state = Y_UP;
      } else {
        new_state = Y_DOWN;
      }
    }
  } else if ((axisVett[2] > axisVett[0]) && (axisVett[2] > axisVett[1])) {
    new_state = Z_UP;
  }


  if (new_state == X_UP || new_state == X_DOWN) {
    if (pinch) {
      knobX = !knobX;
    }
    if (knobX) {
    value_x = readMap(deltaX);
    } else{
      fixed_value_x= value_x;
    }
  } else if (new_state == Y_UP || new_state == Y_DOWN) {
    if (pinch) {
      knobY = !knobY;
    }
    if (knobY) {
    value_y = readMap(deltaY);
    } else{
      fixed_value_y= value_y;
    }
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
