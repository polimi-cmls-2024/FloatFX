// General
int framerate = 75;
float minDb = -120;

// Visualization
int fftOrder = 10;
float correctionFactor = 0.6;
float smoothUp = 0.02; // [0,1]
float smoothDown = 0.05; // [0,1]
float particleLifespan = 0.3; // [seconds]
float particleGravity = 2; // [seconds]
float nParticlesPerPx = 1;
float padding = 0.3; // [0,1)
color backgroundColor = color(20,20,20);
color particleColor = color(232, 201, 255);
color barColor = color(80, 50, 179);

// OSC
String ip = "127.0.0.1";
int port = 7771;
