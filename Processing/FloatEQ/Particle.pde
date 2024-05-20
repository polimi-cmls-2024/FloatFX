class Particle { 
  PVector pos = new PVector();
  PVector vel = new PVector();
  PVector force = new PVector();
  float lifespan;
  float currentLife = 0;
  float size = random(3);
  
  Particle (float xCoord, float yCoord, float lifespanTime) {  
    pos.x = xCoord;
    pos.y = yCoord;
    lifespan = lifespanTime;
  }
  
  void applyForce(PVector forceVec) { 
    force = forceVec;
  }
  
  void update() {
    vel.add(force);
    pos.add(vel);
    currentLife += 1.0/frameRate;
    force.set(0, 0);
  }
  
  void draw() {
    if(currentLife > lifespan) return;
    
    fill(255, 255, 255, 255*(lifespan - currentLife) / lifespan);
    noStroke();
    ellipse(pos.x, pos.y, size, size);
  }
}
