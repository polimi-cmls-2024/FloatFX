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
#define WINDOW_SAMPLES  50
#define SETTLE_TIME     3000    //Time in ms for the calibration



// Accelerometer values at rest from datasheet, to be overwritten by the calibration routine
uint16_t xRest = 1352;
uint16_t yRest = 1352;
uint16_t zRest = 2007;

void setup() {
  // put your setup code here, to run once:
}

void loop() {
  // put your main code here, to run repeatedly:
}


void calibrateAccelerometer() {

  //Serial OSC to tell the user to keep the glove horizontal
  //countdown instead of one delay maybe(?)
  // Time for the user to settle
  delay(3000);

  uint32_t samplingStart = millis();
  while ((millis() - samplingStart) < SETTLE_TIME) {

    // Low-pass on the read values
    xRest = (xRest * (WINDOW_SAMPLES - 1) + analogRead(X_PIN)) / WINDOW_SAMPLES;
    yRest = (yRest * (WINDOW_SAMPLES - 1) + analogRead(Y_PIN)) / WINDOW_SAMPLES;
    zRest = (zRest * (WINDOW_SAMPLES - 1) + analogRead(Z_PIN)) / WINDOW_SAMPLES;
  }

  //Serial OSC to notify of the completed calibration
}