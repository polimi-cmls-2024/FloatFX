// Clamp between Min and Max
public static float clamp(float val, float min, float max) {
  return Math.max(min, Math.min(max, val));
}

// Get intermediate value for smooth
public float getSmoothValue(float prev, float current){
  float diff = current - prev;
  return prev + diff * (1.0 - smoothAmount);
}
