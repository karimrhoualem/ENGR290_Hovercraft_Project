/*
 * Paulina Navarro Aviles (40109825)
 * Karim Rhoualem (26603157)
 * Megan Walbaum (40068567)
 * Ya Ling Wu (40111645)
 */

/***********************************************************************
*                                CONSTANTS                             *
************************************************************************/
//DO NOT CHANGE THESE VALUES
const float pi = 3.1415926;
const float epsilon = 0.001;

//ONLY CHANGE THESE VALUES
const int _delay = 200;
const float _servoFarRight = pi/2;
const float _servoAdjustedFarRight = pi/4;
const float _servoRight = pi/8;
const float _servoAdjustedRight = pi/11;
const float _servoCenter = -pi/15;

const float _throttleOff = 0;
const float _throttleTurn = 0.725;
const float _throttleMedium = 0.775;
const float _throttleMediumHigh = 0.8;
const float _throttleHigh = 0.9;
const float _throttleRamp = 1;

//DO NOT CHANGE THESE VALUES
const float _servoAdjustedLeft = -_servoAdjustedRight;
const float _servoLeft = -_servoRight;
const float _servoAdjustedFarLeft = -_servoAdjustedFarRight;
const float _servoFarLeft = -_servoFarRight;


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
  THROTTLE_OFF,
  THROTTLE_TURN,
  THROTTLE_MEDIUM,
  THROTTLE_MEDIUM_HIGH,
  THROTTLE_HIGH,
  THROTTLE_RAMP
};

// Enum that specifies the position of the servo motor.
enum ServoPosition {
  SERVO_FAR_LEFT,
  SERVO_ADJUST_FAR_LEFT,
  SERVO_LEFT,
  SERVO_ADJUST_LEFT,
  SERVO_CENTER,
  SERVO_ADJUST_RIGHT,
  SERVO_RIGHT,
  SERVO_ADJUST_FAR_RIGHT,
  SERVO_FAR_RIGHT
};

enum Direction {
  FAR_LEFT,
  LEFT,
  CENTER,
  RIGHT,
  FAR_RIGHT,
  RESET
};


/***********************************************************************
*                            GLOBAL VARIABLES                          *
************************************************************************/

//Initialize global throttle and fan variables.
Throttle _fanThrottle = THROTTLE_HIGH;
float _convertedThrottle = 0;

//Initialize the global servo variable.
ServoPosition _servoPosition;
float _convertedPosition = 0;

//Initialize the global fan state variable.
Fan _fanState = ON;

//Initialize global sensor variables.
float _frontSensorValue = 0;
float _leftSensorValue = 0;
float _farLeftSensorValue = 0;
float _rightSensorValue = 0;
float _farRightSensorValue = 0;

//Initialize previous sensor values
float _previousFrontSensorValue = 0; 
float _previousLeftSensorValue = 0;
float _previousFarLeftSensorValue = 0;
float _previousRightSensorValue = 0;
float _previousFarRightSensorValue = 0;

//Initialize boolean movement status variables.
bool _stillMovingForward = true;
bool _stillTurning = false;

//Initialize positional variables.
bool _turnLeft = false;
bool _straight = false;
bool _turnRight = false;
bool _slightLeft = false;
bool _slightRight = false;

//Initialize simulation time variables.
float _simTime = 0;
float _previousSimTime = 0;

//Initialize angular position and velocity variables.
float _angularVelocity_z = 0;
float _previousAngularVelocity_z = 0;

//Initialize numerical integration sum (aka the hovercrafts angular position)
float _numericalIntegrationSum = 0;

//Initialize boolean value to adjust angle when making large turns.
bool _adjust = false;

//Initialize boolean value to get out of stuck positions.
bool _stuck = false;
bool _stuckGoLeft = false;
bool _stuckGoRight = false;
bool _stuckGoStraight = false;

//Initialize the min and max global sensor values.
float _maxSensorValue = 0;
float _minSensorValue = 0;

/***********************************************************************
*                                FUNCTIONS                             *
************************************************************************/

void Move() {
  CompareSensorDistances(_frontSensorValue, _leftSensorValue, _farLeftSensorValue, _rightSensorValue, _farRightSensorValue);

  if(_stuck) {
    delay(_delay);
    MoveStuck();
  }
  
  SetServo(); 
}

void CheckIfStuck() {
  if(abs(_previousFarLeftSensorValue - _farLeftSensorValue) <= epsilon && abs(_previousLeftSensorValue - _leftSensorValue) <= epsilon && abs(_previousFrontSensorValue - _frontSensorValue) <= epsilon &&
    abs(_previousRightSensorValue - _rightSensorValue) <= epsilon && abs(_previousFarRightSensorValue - _farRightSensorValue) <= epsilon) {
      
      _stuck = true;

      return;
  }

  _stuck = false;
  _stuckGoLeft = false;
  _stuckGoRight = false;
  _stuckGoStraight = false;
}

void MoveStuck() {
  if(_minSensorValue == _farLeftSensorValue || _minSensorValue == _leftSensorValue) {
    _stuckGoRight = true;
  }
  else if (_minSensorValue == _farRightSensorValue || _minSensorValue == _rightSensorValue) {
    _stuckGoLeft = true;
  }
  else {
    if(_maxSensorValue == _frontSensorValue) {
      _stuckGoStraight = true; //This is for when we get stuck over an obstacle but are still facing forward.
    }
    else {
      if(_farRightSensorValue > _farLeftSensorValue) {
        _stuckGoRight = true;
      }
      else {
        _stuckGoLeft = true;
      }
    }
  }
}

void CompareSensorDistances(float frontSensorValue, float leftSensorValue, float farLeftSensorValue, float rightSensorValue, float farRightSensorValue) {

  float maxSideSensorValue = max(leftSensorValue, rightSensorValue);
  float maxFarSideSensorValue = max(farLeftSensorValue, farRightSensorValue);
  float maxSide = max(maxSideSensorValue, maxFarSideSensorValue);
  _maxSensorValue = max(maxSide, frontSensorValue);

  float minSideSensorValue = min(leftSensorValue, rightSensorValue);
  float minFarSideSensorValue = min(farLeftSensorValue, farRightSensorValue);
  float minSide = min(minSideSensorValue, minFarSideSensorValue);
  _minSensorValue = min(minSide, frontSensorValue);
  
  CheckIfStuck();
  
  if(rightSensorValue == 100) {
    _stillTurning = false;
    _stillMovingForward = true;
    
    _slightRight = true;
    _turnLeft = false;
    _straight = false;
    _turnRight = false;
    _slightLeft = false;

    _adjust = !_adjust;
  }
  else if(leftSensorValue == 100) {
    _stillTurning = false;
    _stillMovingForward = true;
    
    _slightLeft = true;
    _turnLeft = false;
    _straight = false;
    _turnRight = false;
    _slightRight = false;

    _adjust = !_adjust;
  }
  else if (frontSensorValue == 100) { 
    _stillTurning = false;
    _stillMovingForward = true;
    
    _straight = true;
    _turnLeft = false;
    _turnRight = false;
    _slightLeft = false;
    _slightRight = false;
  }
  else if(farRightSensorValue == 100) {
    _stillTurning = true;
    _stillMovingForward = false;
    
    _turnRight = true;
    _straight = false;
    _turnLeft = false;
    _slightLeft = false;
    _slightRight = false;

    _adjust = !_adjust;
  }
  else if(farLeftSensorValue == 100) {
    _stillTurning = true;
    _stillMovingForward = false;
    
    _turnLeft = true;
    _straight = false;
    _turnRight = false;
    _slightLeft = false;
    _slightRight = false;

    _adjust = !_adjust;
  }
  else if (_maxSensorValue == frontSensorValue) {
    _stillTurning = false;
    _stillMovingForward = true;
    
    _straight = true;
    _turnLeft = false;
    _turnRight = false;
    _slightLeft = false;
    _slightRight = false;
  }
  else if(_maxSensorValue == rightSensorValue) {
    _stillTurning = false;
    _stillMovingForward = true;
    
    _slightRight = true;
    _turnLeft = false;
    _straight = false;
    _turnRight = false;
    _slightLeft = false;

    _adjust = !_adjust;
  }
  else if(_maxSensorValue == leftSensorValue) {
    _stillTurning = false;
    _stillMovingForward = true;
    
    _slightLeft = true;
    _turnLeft = false;
    _straight = false;
    _turnRight = false;
    _slightRight = false;

    _adjust = !_adjust;
  }
  else if(_maxSensorValue == farRightSensorValue) {
    _stillTurning = true;
    _stillMovingForward = false;
    
    _turnRight = true;
    _straight = false;
    _turnLeft = false;
    _slightLeft = false;
    _slightRight = false;

    _adjust = !_adjust;
  }
  else if(_maxSensorValue == farLeftSensorValue) {
    _stillTurning = true;
    _stillMovingForward = false;
    
    _turnLeft = true;
    _straight = false;
    _turnRight = false;
    _slightLeft = false;
    _slightRight = false;

    _adjust = !_adjust;
  }
  else {
    _stillTurning = false;
    _stillMovingForward = true;
    
    _straight = true;
    _turnLeft = false;
    _turnRight = false;
    _slightLeft = false;
    _slightRight = false;
  }
}

void SetServo() {
  if(_stuckGoLeft) {
    _fanThrottle = THROTTLE_RAMP;
    _servoPosition = SERVO_FAR_RIGHT;
  }
  else if(_stuckGoRight) {
    _fanThrottle = THROTTLE_RAMP;
    _servoPosition = SERVO_FAR_LEFT;
  }
  else if(_stuckGoStraight) {
    _fanThrottle = THROTTLE_RAMP;
    _servoPosition = SERVO_CENTER;
  }
  else if(_stillTurning && _turnLeft) {
    if(_adjust) {
      _fanThrottle = THROTTLE_TURN;
      _servoPosition = SERVO_ADJUST_FAR_RIGHT;
    }
    else {
      _fanThrottle = THROTTLE_TURN;
      _servoPosition = SERVO_FAR_RIGHT;
    }
  }
  else if(_stillTurning && _turnRight) {
    if(_adjust) {
      _fanThrottle = THROTTLE_TURN;
      _servoPosition = SERVO_ADJUST_FAR_LEFT;
    }
    else {
      _fanThrottle = THROTTLE_TURN;
      _servoPosition = SERVO_FAR_LEFT;
    }
  }
  else if(_stillMovingForward && _straight) {
    _fanThrottle = THROTTLE_MEDIUM_HIGH;
    _servoPosition = SERVO_CENTER;
  }
  else if(_stillMovingForward && _slightRight) {
    if(_adjust) {
      _fanThrottle = THROTTLE_MEDIUM;
      _servoPosition = SERVO_ADJUST_LEFT;
    }
    else {
      _fanThrottle = THROTTLE_MEDIUM;
      _servoPosition = SERVO_LEFT;
    }
  }
  else if(_stillMovingForward && _slightLeft) {
    if(_adjust) { 
      _fanThrottle = THROTTLE_MEDIUM;
      _servoPosition = SERVO_ADJUST_RIGHT;
    }
    else {
      _fanThrottle = THROTTLE_MEDIUM;
      _servoPosition = SERVO_RIGHT;
    }
  }
  
  ConvertPosition(_servoPosition);
  ConvertThrottle(_fanThrottle);
}

//void RampDetect(float angularVelocity_x){
//  if (angularVelocity_x < -0.005 || angularVelocity_x > 0.005){
//    _fanThrottle = THROTTLE_HIGH;
//  }
//}

void ConvertThrottle(Throttle fanThrottle) {
  
  switch (fanThrottle) {
    case THROTTLE_OFF:
      _convertedThrottle = _throttleOff;
      break;
    case THROTTLE_TURN:
      _convertedThrottle = _throttleTurn;
      break;
    case THROTTLE_MEDIUM:
      _convertedThrottle = _throttleMedium;
      break;
    case THROTTLE_MEDIUM_HIGH:
      _convertedThrottle = _throttleMediumHigh;
      break;
    case THROTTLE_HIGH:
      _convertedThrottle = _throttleHigh;
      break;
    case THROTTLE_RAMP:
      _convertedThrottle = _throttleRamp;
      break;
  }
}

void ConvertPosition(ServoPosition servoPosition) {
  
  switch (servoPosition) {
    case SERVO_FAR_LEFT:
      _convertedPosition = _servoFarLeft;
      break;
    case SERVO_ADJUST_FAR_LEFT:
      _convertedPosition = _servoAdjustedFarLeft;
      break;
    case SERVO_LEFT:
      _convertedPosition = _servoLeft;
      break;
    case SERVO_ADJUST_LEFT:
      _convertedPosition = _servoAdjustedLeft;
      break;
    case SERVO_CENTER:
      _convertedPosition = _servoCenter; //Set servo position a bit to the right to compensate for fan torque pull to the left.
      break;
    case SERVO_ADJUST_RIGHT:
      _convertedPosition = _servoAdjustedRight;
      break;
    case SERVO_RIGHT:
      _convertedPosition = _servoRight;
      break;
    case SERVO_ADJUST_FAR_RIGHT:
      _convertedPosition = _servoAdjustedFarRight;
      break;
    case SERVO_FAR_RIGHT:
      _convertedPosition = _servoFarRight;
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

void StorePreviousSensorValues() {
  _previousFrontSensorValue = _frontSensorValue; 
  _previousLeftSensorValue = _leftSensorValue;
  _previousFarLeftSensorValue = _farLeftSensorValue;
  _previousRightSensorValue = _rightSensorValue;
  _previousFarRightSensorValue = _farRightSensorValue;
}

// Main loop function
void loop() { 
  //Read serial port until sensor data is sent
  serialWait();

  _simTime = Serial.parseFloat();
  _farLeftSensorValue = Serial.parseFloat();
  _leftSensorValue = Serial.parseFloat();
  _frontSensorValue = Serial.parseFloat();
  _rightSensorValue = Serial.parseFloat();
  _farRightSensorValue = Serial.parseFloat();
  _angularVelocity_z = Serial.parseFloat();
  
  Move();

  StorePreviousSensorValues();
  
  Serial.print(_fanState);
  Serial.write(",");
  Serial.print(_convertedThrottle);
  Serial.write(",");
  Serial.print(_convertedPosition);
  Serial.write(",");
  Serial.print(_numericalIntegrationSum);
  Serial.write(",");
  Serial.print(_turnLeft);
  Serial.write(",");
  Serial.print(_slightLeft);
  Serial.write(",");
  Serial.print(_straight);
  Serial.write(",");
  Serial.print(_slightRight);
  Serial.write(",");
  Serial.print(_turnRight);
  Serial.write(",");
  Serial.print(_stillMovingForward);
  Serial.write(",");
  Serial.print(_stillTurning);
  Serial.write(",");
  Serial.print(_stuck);
  Serial.write("\r\n");
}
