import oscP5.*;
import netP5.*;

// Global variables
OscP5 oscP5;
NetAddress myRemoteLocation;

// OSC event handler
void oscEvent(OscMessage message) {
  if(message.addrPattern().equals("/spectrum")){
    for (int i = 0; i < nBeans; i++) {
      float value = clamp(message.get(i).floatValue() * 0.01, 0f, 1f);
      currentValues.set(i, value);
    }
  }
}
