// Clamp between Min and Max
public static float clamp(float val, float min, float max) {
  return Math.max(min, Math.min(max, val));
}

// Get intermediate value for smooth
public float getSmoothValue(float prev, float current){
  float diff = current - prev;
  if(diff >= 0){
    return prev + diff * (1.0 - (float)Math.pow(smoothUp, 0.1));
  }else{
    return prev + diff * (1.0 - (float)Math.pow(smoothDown, 0.1));
  }
}

public int calculateBarHeight(float value, int i){
  
    double valueLog = 20 * Math.log10(value);
    valueLog = Math.max(minDb, valueLog);
    valueLog = (valueLog - minDb) / (-minDb);
    int barHeight = (int)Math.round(valueLog * size.y);
    
    double correction = ((float)xValues.get(i) / size.x) * 2 - 1;
    correction = correction * bassCorrection * size.y * 0.3;
    
    return barHeight + (int)correction;
}

public int calculateX(int beanIndex){
    double minFDb = 20 * Math.log10(1.0/nBeans);
    double xLog = 20 * Math.log10((float)(beanIndex+1) / nBeans);
    xLog = Math.max(minFDb, xLog);
    xLog = (xLog - minFDb) / (-minFDb);
    int x = (int)Math.round(xLog * size.x);
    return x;
}
