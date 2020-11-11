/***********************************************************************
*                                   ENUMS                              *
************************************************************************/

// Enum that specifies the current fan setting.
enum Fan {
  OFF,
  ON
};

// Enum that specifies the current throttle setting for a fan.
enum Throttle {
  THROTTLE_OFF,
  THROTTLE_MEDIUM,
  THROTTLE_HIGH
};

// Enum that specifies the direction the hovercraft will move toward.
enum Direction {
  FRONT,
  LEFT,
  RIGHT,
  ERR
};

/***********************************************************************
*                            GLOBAL VARIABLES                          *
************************************************************************/
//Initialize global sensor variables
float frontSensorValue = 0;
float leftSensorValue = 0;
float rightSensorValue = 0;

//Initialize global direction variable
Direction hovercraftDirection;

//Initialize global throttle and fan variables
//Is it redundant to set throttle AND fans to ON/OFF??
Throttle leftThrottle = THROTTLE_OFF;
Throttle rightThrottle = THROTTLE_OFF;
Fan leftFan = OFF;
Fan rightFan = OFF;

/***********************************************************************
*                                FUNCTIONS                             *
************************************************************************/

// Takes the sensor values as parameters, finds the max, and then positions the hovercraft towards the max direction.
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

/*
 * Takes the direction to move the hovercraft in, the fan throttle settings, and the fan statuses as parameters, 
 * and then sets the throttle and fan settings accordingly.
 */
void Move(Direction maxDirection, Fan leftFan, Fan rightFan) {
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
*                               MAIN CODE                              *
************************************************************************/
// Main setup function
void setup() {

}

// Main loop function
void loop() {  
  hovercraftDirection = compareSensorDistances(frontSensorValue, leftSensorValue, rightSensorValue);

  Move(hovercraftDirection, leftFan, rightFan);
}
