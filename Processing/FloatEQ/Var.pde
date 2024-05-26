// Global variables
PVector nw, se, size; // drawing region
int barWidth;
int nBeans = (int)Math.pow(2, fftOrder-1);
ArrayList<Float> currentValues; // [0,1]
ArrayList<Float> smoothValues; // [0,1]
ArrayList<Float> prevValues; // [0,1]
ArrayList<ParticleSystem> particleSystems = new ArrayList<ParticleSystem>();
