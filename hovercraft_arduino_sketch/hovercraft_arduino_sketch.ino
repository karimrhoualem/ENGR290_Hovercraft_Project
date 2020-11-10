enum Direction {
  FRONT,
  LEFT,
  RIGHT,
  ERR
};

Direction compareSensorDistances(float frontSensorValue, float leftSensorValue, float rightSensorValue) {

  float intermediaryMax = max(leftSensorValue, rightSensorValue);
  float maxSensorValue = max(intermediaryMax, frontSensorValue);

  if (maxSensorValue == frontSensorValue) {
    return Direction::FRONT;
  }
  else if (maxSensorValue == leftSensorValue) {
    return Direction::LEFT;
  }
  else if (maxSensorValue == rightSensorValue) {
    return Direction::RIGHT;
  }
  else {
    return Direction::ERR;
  }
};


void setup() {
  // put your setup code here, to run once:
  
}

void loop() {  
  
  
}
