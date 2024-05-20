class ParticleSystem {
  float lifespan;
  float currentLife = 0;
  Particle[] particles;
  int nPart;
  
  ParticleSystem (int nParticles, float lifespanTime, PVector pos, int barWidth) {
    lifespan = lifespanTime;
    nPart = nParticles;
    particles = new Particle[nPart];
    
    for (int i = 0; i < nPart; i++) {
      particles[i] = new Particle(pos.x + random(barWidth), pos.y + random(2), lifespan);
    }
  }
  
  void update() {
    currentLife += 1.0/frameRate;
    for (int i = 0; i < nPart; i++) {
      particles[i].applyForce(new PVector(random(0.04)-0.02, random(0.04)-0.01));
      particles[i].update();
    }
    draw();
  }
  
  boolean isDead() {
    return currentLife > lifespan;
  }
  
  void draw() {
    for (int i = 0; i < nPart; i++) {
      particles[i].draw();
    }
  }
}


void updateParticleSystems(){
  
  // Remove dead systems
  for (int i = particleSystems.size() - 1; i >= 0; i--) {
    if(particleSystems.get(i).isDead()){
      particleSystems.remove(i);
    }
  }
  
  // Add new systems
  for (int i = 0; i < nBeans; i++) {
    int barHeight = Math.round(smoothValues.get(i) * size.y);
    int barX = Math.round((float)i / nBeans * size.x);
    particleSystems.add(
    new ParticleSystem (nParticlesPerCycle, particleLifespan, new PVector(nw.x + barX, nw.y + size.y - barHeight), barWidth));
  }
  
  // Update systems
  for (int i = particleSystems.size() - 1; i >= 0; i--) {
    particleSystems.get(i).update();
  }
}


void drawParticleSystems(){
  
  // Draw systems
  for (int i = particleSystems.size() - 1; i >= 0; i--) {
    particleSystems.get(i).draw();
  }
}
