/***********************************************************************
*                                   ENUMS                              *
************************************************************************/
const float pi = 3.1415926;
const float epsilon = 0.001;

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
//Initialize simulations times
float simTime;
float previoussimTime;

//Initialize global sensor variables
float frontSensorValue;
float leftSensorValue;
float farLeftSensorValue;
float rightSensorValue;
float farRightSensorValue;

//Initialize previous global sensor values
float previousFrontSensorValue = 0;
float previousLeftSensorValue = 0;
float previousFarLeftSensorValue = 0;
float previousRightSensorValue = 0;
float previousFarRightSensorValue = 0;

//Initialize global direction variable
Direction hovercraftDirection;
Direction previousHovercraftDirection = FRONT;

//Initialize global throttle and fan variables
Throttle fanThrottle = THROTTLE_HIGH;
float convertedThrottle;

//Initialize the global servo variable
ServoPosition servoPosition;
ServoPosition previousServoPosition;
float convertedPosition;

//Initialize the global fan state variable
Fan fanState = ON;

//Initialize the global IMU variable and angular pos
float angularVelocity_x;
float angularVelocity_z;
float angularPosition = 0;
float previousangularVelocity_z = 0;
bool adjust=true;

//Boolean value to determine whether hovercraft is stuck
bool stuck = false;

float maxSensorValue = 0;
float minSensorValue = 0;
String closest;

/***********************************************************************
*                                FUNCTIONS                             *
************************************************************************/

// Takes the sensor values as parameters, finds the max, and then positions the hovercraft towards the max direction.
Direction compareSensorDistances(float simTime, float angularVelocity_z, float frontSensorValue, float leftSensorValue, float farLeftSensorValue, float rightSensorValue, float farRightSensorValue) {

  if(adjust){
    adjust = false;
    return Direction::RIGHT;
  }


  if(FullCircle(simTime, angularVelocity_z)){
    return Direction::FRONT;
  }

  
  float maxSideSensorValue = max(leftSensorValue, rightSensorValue);
  float maxFarSideSensorValue = max(farLeftSensorValue, farRightSensorValue);
  float maxSide = max(maxSideSensorValue, maxFarSideSensorValue);
  maxSensorValue = max(maxSide, frontSensorValue);

  float minSideSensorValue = min(leftSensorValue, rightSensorValue);
  float minFarSideSensorValue = min(farLeftSensorValue, farRightSensorValue);
  float minSide = min(minSideSensorValue, minFarSideSensorValue);
  minSensorValue = min(minSide, frontSensorValue);
  
  if(abs(previousFarLeftSensorValue - farLeftSensorValue) <= epsilon && abs(previousLeftSensorValue - leftSensorValue) <= epsilon && abs(previousFrontSensorValue - frontSensorValue) <= epsilon &&
      abs(previousRightSensorValue - rightSensorValue) <= epsilon && abs(previousFarRightSensorValue - farRightSensorValue) <= epsilon) {

        if (minSensorValue == leftSensorValue || minSensorValue == farLeftSensorValue) {
          closest = "left";
        }
        else if(minSensorValue == rightSensorValue || minSensorValue == farRightSensorValue) {
          closest = "right";
        }
        else {
          closest = "front";
        }
        
        stuck = true;

        return previousHovercraftDirection;
  }

  stuck = false;
  
  if (maxSensorValue == frontSensorValue) {
    return Direction::FRONT;
  }
  else if (maxSensorValue == farRightSensorValue) {
    if(maxSensorValue == rightSensorValue) {
      return Direction::RIGHT;
    }
    else {
      return Direction::FAR_RIGHT;
    }
  }
  else if (maxSensorValue == farLeftSensorValue) {
    if (maxSensorValue == leftSensorValue) {
      return Direction::LEFT;
    }
    else {
      return Direction::FAR_LEFT;  
    }
  }
  else if(maxSensorValue == rightSensorValue) {
    return Direction::RIGHT;
  }
  else if(maxSensorValue == leftSensorValue) {
    return Direction::LEFT;
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
    fanThrottle = THROTTLE_HIGH;
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
    fanThrottle = THROTTLE_HIGH;
  }
  else if (maxDirection == ERR) {
    //Handle error
  }
}

//IMU detect ramps
void RampDetect(float angularVelocity_x){
  if (angularVelocity_x < -0.005 || angularVelocity_x > 0.005){
    fanThrottle = THROTTLE_HIGH;
  }
}



float MoveStuck() {
  if(closest == "left") {
    if(farLeftSensorValue > farRightSensorValue || leftSensorValue > rightSensorValue || frontSensorValue < (leftSensorValue && rightSensorValue)) {
      return -pi/2;    
    }
    return pi/2;
  }
  else if(closest == "right") {
    if(farLeftSensorValue > farRightSensorValue || leftSensorValue > rightSensorValue || frontSensorValue < (leftSensorValue && rightSensorValue)) {
      return pi/2;    
    }
    return -pi/2;
  }
  else if(closest == "front") {
    if(farLeftSensorValue > farRightSensorValue || leftSensorValue > rightSensorValue || frontSensorValue < (leftSensorValue && rightSensorValue)) {
      return pi/2;    
    }
    return -pi/2;
  }
  else {
    return 0;
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
        convertedPosition = -pi/3;
        break;
      case SERVO_LEFT:
        convertedPosition = -pi/7;
        break;
      case SERVO_CENTER:
        convertedPosition = 0;
        break;
      case SERVO_RIGHT:
        convertedPosition = pi/7;
        break;
      case SERVO_FAR_RIGHT:
        convertedPosition = pi/3;
        break;
    }
}
bool FullCircle(float simTime, float angularVelocity_z){

  angularPosition += ((angularVelocity_z + previousangularVelocity_z)/2)*(simTime - previoussimTime);
  previousangularVelocity_z=angularVelocity_z;
  previoussimTime = simTime;

  if(  abs(angularPosition) > 0.09 ){
    //not sure what to do here
   angularPosition = 0;
   return true;
   }
   else{
    return false;
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

  simTime = Serial.parseFloat();
  farLeftSensorValue = Serial.parseFloat();
  leftSensorValue = Serial.parseFloat();
  frontSensorValue = Serial.parseFloat();
  rightSensorValue = Serial.parseFloat();
  farRightSensorValue = Serial.parseFloat();
  angularVelocity_z = Serial.parseFloat();
  //angularVelocity_x = Serial.parseFloar();

  

  FullCircle(simTime, angularVelocity_z);

  
  hovercraftDirection = compareSensorDistances(simTime, angularVelocity_z, frontSensorValue, leftSensorValue, farLeftSensorValue, rightSensorValue, farRightSensorValue);

  if(stuck) {
    delay(200);
    convertedPosition = MoveStuck();
  }
  else {
    Move(hovercraftDirection);
    ConvertPosition(servoPosition);
  }

  ConvertThrottle(fanThrottle);

  previousFrontSensorValue = frontSensorValue; 
  previousLeftSensorValue = leftSensorValue;
  previousFarLeftSensorValue = farLeftSensorValue;
  previousRightSensorValue = rightSensorValue;
  previousFarRightSensorValue = farRightSensorValue;
  previousHovercraftDirection = hovercraftDirection;

  
  Serial.print(fanState);
  Serial.write(",");
  Serial.print(convertedThrottle);
  Serial.write(",");
  Serial.print(convertedPosition);
  Serial.write(",");
  Serial.print(stuck);
  Serial.write(",");
  Serial.print(angularPosition);
  Serial.write("\r\n");
}
