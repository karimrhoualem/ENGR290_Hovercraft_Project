/***********************************************************************
*                                   ENUMS                              *
************************************************************************/

// Enum that specifies the current fan setting.
enum Fan {
  OFF = 0,
  ON = 1
};

// Enum that specifies the current throttle setting for a fan.
enum Throttle {
  THROTTLE_OFF = 0,
  THROTTLE_MEDIUM = 1,
  THROTTLE_HIGH = 2
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
float frontSensorValue;
float leftSensorValue;
float rightSensorValue;

//Initialize global direction variable
Direction hovercraftDirection;

//Initialize global throttle and fan variables
Throttle leftFanThrottle;;
Throttle rightFanThrottle;
float leftConvertedThrottle;
float rightConvertedThrottle;

Fan leftFanState = ON;
Fan rightFanState = ON;

/***********************************************************************
*                                FUNCTIONS                             *
************************************************************************/

// Takes the sensor values as parameters, finds the max, and then positions the hovercraft towards the max direction.
Direction compareSensorDistances(float frontSensorValue, float leftSensorValue, float rightSensorValue) {
  
  float maxSensorValue = max(leftSensorValue, rightSensorValue);
  maxSensorValue = max(maxSensorValue, frontSensorValue);

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
void Move(Direction maxDirection, Fan leftFanState, Fan rightFanState) {
  if (maxDirection == LEFT) {
    leftFanThrottle = THROTTLE_OFF;
    rightFanThrottle = THROTTLE_HIGH;
    leftFanState = OFF;
    rightFanState = ON;
  }
  else if (maxDirection == RIGHT) {
    leftFanThrottle = THROTTLE_HIGH;
    rightFanThrottle = THROTTLE_OFF;
    leftFanState = ON;
    rightFanState = OFF;
  }
  else if (maxDirection == FRONT) {
    leftFanThrottle = THROTTLE_HIGH;
    rightFanThrottle = THROTTLE_HIGH;
    leftFanState = ON;
    rightFanState = ON;
  }
  else if (maxDirection == ERR) {
    //Handle error
  }
}

void ConvertThrottle(Throttle leftThrottle, Throttle rightThrottle) {
    switch (leftThrottle) {
      case THROTTLE_OFF:
        leftConvertedThrottle = 0;
        break;
      case THROTTLE_MEDIUM:
        leftConvertedThrottle = 0.5;
        break;
      case THROTTLE_HIGH:
        leftConvertedThrottle = 1;
        break;
    }

    switch (rightThrottle) {
      case THROTTLE_OFF:
        rightConvertedThrottle = 0;
        break;
      case THROTTLE_MEDIUM:
        rightConvertedThrottle = 0.5;
        break;
      case THROTTLE_HIGH:
        rightConvertedThrottle = 1;
        break;
    }
}

/***********************************************************************
*                               MAIN CODE                              *
************************************************************************/
// Main setup function
void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200); 
  Serial.setTimeout(5);
}

void serialWait(){
  while(Serial.peek() != 's') {
    char t = Serial.read();
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1);                      
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(1);
  }
  char t = Serial.read(); //discard the s
}

// Main loop function
void loop() { 
  //Read serial port until sensor data is sent
  serialWait();
  
  frontSensorValue = Serial.parseFloat();
  leftSensorValue = Serial.parseFloat();
  rightSensorValue = Serial.parseFloat();
  
  hovercraftDirection = compareSensorDistances(frontSensorValue, leftSensorValue, rightSensorValue);

  Move(hovercraftDirection, leftFanState, rightFanState);

  ConvertThrottle(leftFanThrottle, rightFanThrottle);
  
  Serial.print(leftFanState);
  Serial.write(",");
  Serial.print(rightFanState);
  Serial.write(",");
  Serial.print(leftConvertedThrottle);
  Serial.write(",");
  Serial.print(rightConvertedThrottle);
  Serial.write("\r\n");
}
