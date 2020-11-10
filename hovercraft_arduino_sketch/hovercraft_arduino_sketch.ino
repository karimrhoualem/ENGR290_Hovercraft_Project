enum Fan {
  OFF,
  ON
};

enum Throttle {
  THROTTLE_OFF,
  THROTTLE_MEDIUM,
  THROTTLE_HIGH
};

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

void Move(Direction maxDirection, Throttle leftThrottle, Throttle rightThrottle, Fan leftFan, Fan rightFan) {
  if (maxDirection == LEFT) {
    leftThrottle = THROTTLE_OFF;
    rightThrottle = THROTTLE_MEDIUM;
    leftFan = OFF;
    rightFan = ON;
  }
  else if (maxDirection == RIGHT) {
    leftThrottle = THROTTLE_MEDIUM;
    rightThrottle = THROTTLE_OFF;
    leftFan = ON;
    rightFan = OFF;
  }
  else if (maxDirection == FRONT) {
    leftThrottle = THROTTLE_HIGH;
    rightThrottle = THROTTLE_HIGH;
    leftFan = ON;
    rightFan = ON;
  }
  else if (maxDirection == ERR) {
    //Handle error
  }
}

/***********************************************************************
*                            GLOBAL VARIABLES                          *
************************************************************************/
//Define sensor variables
float frontSensorValue = 0;
float leftSensorValue = 0;
float rightSensorValue = 0;

//Define direction variable
Direction hovercraftDirection;

//Define throttle and fan variables
//Is it redundant to set throttle AND fans to ON/OFF??
Throttle leftThrottle = THROTTLE_OFF;
Throttle rightThrottle = THROTTLE_OFF;
Fan leftFan = OFF;
Fan rightFan = OFF;

void setup() {

}

void loop() {  
  hovercraftDirection = compareSensorDistances(frontSensorValue, leftSensorValue, rightSensorValue);

  Move(hovercraftDirection, leftThrottle, rightThrottle, leftFan, rightFan);
}
