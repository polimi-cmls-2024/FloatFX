import oscP5.*;
import netP5.*;

// Global variables
OscP5 oscP5;
NetAddress myRemoteLocation;

// OSC event handler
void oscEvent(OscMessage message) {
  
  if(message.addrPattern().equals("/accelerometer")){
    currentValues.set(0, clamp(message.get(0).floatValue() / 20 + 0.5, 0f, 1f));
    currentValues.set(1, clamp(message.get(1).floatValue() / 20 + 0.5, 0f, 1f));
    currentValues.set(2, clamp(message.get(2).floatValue() / 20 + 0.5, 0f, 1f));
  }
  
  else if(message.addrPattern().equals("/spectrum")){
    for (int i = 0; i < nBeans; i++) {
      currentValues.set(i, clamp(message.get(i).floatValue() / 50, 0f, 1f));
    }
  }
  
}
