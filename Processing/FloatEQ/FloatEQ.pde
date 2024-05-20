void setup() {
  
  // Initialize general parameters
  frameRate(framerate);
  size(600, 300);
  fullScreen();
  
  // Initialize OSC
  oscP5 = new OscP5(this, port);
  myRemoteLocation = new NetAddress(ip,port);
  
  // Initialize global variables
  nw = new PVector((int)(width*padding/2), (int)(height*padding/2));
  se = new PVector((int)(width*(1-padding/2)), (int)(height*(1-padding/2)));
  size = se.copy().sub(nw);
  barWidth = Math.round(size.x / nBeans);
  currentValues = new ArrayList<Float>(); // [0,1]
  smoothValues = new ArrayList<Float>(); // [0,1]
  prevValues = new ArrayList<Float>(); // [0,1]
  for (int i = 0; i < nBeans; i++) {
    currentValues.add(0.);
    smoothValues.add(0.);
    prevValues.add(0.);
  }
}


void draw() {
  
  // Clear
  background(backgroundColor);
  
  // Update smooth values
  for (int i = 0; i < nBeans; i++) {
    smoothValues.set(i, getSmoothValue(prevValues.get(i), currentValues.get(i)));
    prevValues.set(i, smoothValues.get(i));
  }
  
  // Draw smooth bars
  drawBars();
  
  // Update and draw particle systems
  updateParticleSystems();
  drawParticleSystems();
  
  // Draw black bottom part
  fill(backgroundColor);
  rect(0, se.y, width, height);
}

void drawBars(){
  int lastX = (int)nw.x;
  for (int i = 0; i < nBeans; i++) {
    
    // Calculate bar coordinates
    int barHeight = Math.round(smoothValues.get(i) * size.y);
    int barX = Math.round((float)i / nBeans * size.x);
    
    // Draw bar
    fill(255);
    rect(lastX + 1, nw.y + size.y - barHeight, barWidth, barHeight);
    
    lastX = (int)nw.x + barX + barWidth;
  }
}
