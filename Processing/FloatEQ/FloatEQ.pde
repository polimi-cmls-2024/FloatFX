void setup() {
  
  // Initialize general parameters
  frameRate(framerate);
  size(1100, 500);
  //fullScreen();
  
  // Initialize OSC
  myRemoteLocation = new NetAddress(ip,port);
  OscProperties properties = new OscProperties();
  properties.setListeningPort(port);
  properties.setDatagramSize(99999999);
  oscP5 = new OscP5(this, properties);

  // Initialize global variables
  nw = new PVector((int)(width*padding/2), (int)(height*padding/2));
  se = new PVector((int)(width*(1-padding/2)), (int)(height*(1-padding/2)));
  size = se.copy().sub(nw);
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
  for (int i = 0; i < nBeans; i++) {
    
    // Calculate bar coordinates
    int barHeight = calculateBarHeight(smoothValues.get(i));
    int barXLeft = calculateX(i);
    int barXRight = calculateX(i+1);
    int barWidth = Math.round(barXRight - barXLeft);
    
    // Draw bar
    fill(barColor);
    rect(
      nw.x + barXLeft,
      nw.y + size.y - barHeight, 
      barWidth+1,
      barHeight + barWidth, 
      barWidth/2, barWidth/2, 0, 0);
  }
}
