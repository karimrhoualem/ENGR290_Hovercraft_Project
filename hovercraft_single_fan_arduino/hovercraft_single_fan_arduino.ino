/***********************************************************************
*                                   ENUMS                              *
************************************************************************/
const float pi = 3.1415926;

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
  FAR_LEFT,
  RIGHT,
  FAR_RIGHT,
  ERR
};

// Enum that specifies the position of the servo motor.
enum ServoPosition {
  SERVO_FAR_LEFT,
  SERVO_LEFT,
  SERVO_CENTER,
  SERVO_RIGHT,
  SERVO_FAR_RIGHT
};

/***********************************************************************
*                            GLOBAL VARIABLES                          *
************************************************************************/
//Initialize global sensor variables
float frontSensorValue;
float leftSensorValue;
float farLeftSensorValue;
float rightSensorValue;
float farRightSensorValue;

//Initialize global direction variable
Direction hovercraftDirection;

//Initialize global throttle and fan variables
Throttle fanThrottle;
float convertedThrottle;

//Initialize the global servo variable
ServoPosition servoPosition;
float convertedPosition;

//Initialize the global fan state variable
Fan fanState = ON;

/***********************************************************************
*                                FUNCTIONS                             *
************************************************************************/

// Takes the sensor values as parameters, finds the max, and then positions the hovercraft towards the max direction.
Direction compareSensorDistances(float frontSensorValue, float leftSensorValue, float farLeftSensorValue, float rightSensorValue, float farRightSensorValue) {
  
  float maxSideSensorValue = max(leftSensorValue, rightSensorValue);
  float maxFarSideSensorValue = max(farLeftSensorValue, farRightSensorValue);
  float maxSide = max(maxSideSensorValue, maxFarSideSensorValue);
  float maxSensorValue = max(maxSide, frontSensorValue);
  
  if (maxSensorValue == frontSensorValue) {
    return Direction::FRONT;
  }
  else if (maxSensorValue == farLeftSensorValue) {
    if (maxSensorValue == leftSensorValue) {
      return Direction::LEFT;
    }
    else {
      return Direction::FAR_LEFT;  
    }
  }
  else if (maxSensorValue == farRightSensorValue) {
    if(maxSensorValue == rightSensorValue) {
      return Direction::RIGHT;
    }
    else {
      return Direction::FAR_RIGHT;
    }
  }
  else if(maxSensorValue == leftSensorValue) {
    return Direction::LEFT;
  }
  else if(maxSensorValue == rightSensorValue) {
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
void Move(Direction maxDirection) {
  if (maxDirection == FAR_LEFT) {
    servoPosition = SERVO_FAR_RIGHT;
    fanThrottle = THROTTLE_MEDIUM;
  }
  else if (maxDirection == LEFT) {
    servoPosition = SERVO_RIGHT;
    fanThrottle = THROTTLE_HIGH;
  }
  else if (maxDirection == FRONT) {
    servoPosition = SERVO_CENTER;
    fanThrottle = THROTTLE_HIGH;
  }
  else if (maxDirection == RIGHT) {
    servoPosition = SERVO_LEFT;
    fanThrottle = THROTTLE_HIGH;
  }
  else if (maxDirection == FAR_RIGHT) {
    servoPosition = SERVO_FAR_LEFT;
    fanThrottle = THROTTLE_MEDIUM;
  }
  else if (maxDirection == ERR) {
    //Handle error
  }
}

void ConvertThrottle(Throttle fanThrottle) {
    switch (fanThrottle) {
      case THROTTLE_OFF:
        convertedThrottle = 0;
        break;
      case THROTTLE_MEDIUM:
        convertedThrottle = 0.5;
        break;
      case THROTTLE_HIGH:
        convertedThrottle = 1;
        break;
    }
}

void ConvertPosition(ServoPosition servoPosition) {
  switch (servoPosition) {
      case SERVO_FAR_LEFT:
        convertedPosition = -pi/3.5;
        break;
      case SERVO_LEFT:
        convertedPosition = -pi/6;
        break;
      case SERVO_CENTER:
        convertedPosition = 0;
        break;
      case SERVO_RIGHT:
        convertedPosition = pi/6;
        break;
      case SERVO_FAR_RIGHT:
        convertedPosition = pi/3.5;
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

  farLeftSensorValue = Serial.parseFloat();
  leftSensorValue = Serial.parseFloat();
  frontSensorValue = Serial.parseFloat();
  rightSensorValue = Serial.parseFloat();
  farRightSensorValue = Serial.parseFloat();
  
  hovercraftDirection = compareSensorDistances(frontSensorValue, leftSensorValue, farLeftSensorValue, rightSensorValue, farRightSensorValue);

  Move(hovercraftDirection);

  ConvertThrottle(fanThrottle);
  ConvertPosition(servoPosition);
  
  Serial.print(fanState);
  Serial.write(",");
  Serial.print(convertedThrottle);
  Serial.write(",");
  Serial.print(convertedPosition);
  Serial.write("\r\n");
}
