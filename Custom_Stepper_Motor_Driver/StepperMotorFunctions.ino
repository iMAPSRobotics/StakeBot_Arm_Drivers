// Sends a pulse on the STEP pin to tell the driver to take one step, and also
//delays to control the speed of the motor.
#define fast 0.01 //0.006
#define single 0.01 //0.009
#define slow 0.05

void stepSingle(int StepPin, bool faster){
  // The STEP minimum high pulse width is 1.9 microseconds.
  if (StepPin == 2){
    if (!faster) {
      PORTD |= 1 << 1;
      _delay_ms(slow+single);
      PORTD &= ~(1 << 1);
      _delay_ms(slow+single);
    }
    else {
      PORTD |= 1 << 1;
      _delay_ms(fast+single);
      PORTD &= ~(1 << 1);
      _delay_ms(fast+single);
    }
  }
  
  if (StepPin == 8) {
    if (!faster) {
      PORTB |= 1 << 4;
      _delay_ms(slow+single);
      PORTB &= ~(1 << 4);
      _delay_ms(slow+single);
    }
    else {
      PORTB |= 1 << 4;
      _delay_ms(fast+single);
      PORTB &= ~(1 << 4);
      _delay_ms(fast+single);
    }
  }
}

void step(int StepPin, bool faster){
  // The STEP minimum high pulse width is 1.9 microseconds.
  if (StepPin == 2){
    if (!faster) {
      PORTD |= 1 << 1;
      _delay_ms(slow);
      PORTD &= ~(1 << 1);
      _delay_ms(slow);
    }
    else {      
      PORTD |= 1 << 1;
      _delay_ms(fast);
      PORTD &= ~(1 << 1);
      _delay_ms(fast);    }
  }
  if (StepPin == 8) {
    if (!faster) {
      PORTB |= 1 << 4;
      _delay_ms(slow);
      PORTB &= ~(1 << 4);
      _delay_ms(slow);
    }
    else{
      PORTB |= 1 << 4;
      _delay_ms(fast);
      PORTB &= ~(1 << 4);
      _delay_ms(fast);
    }
  }
}

// Writes a high or low value to the direction pin to specify what direction to
// turn the motor.
void setDirection(bool dir, int DirPin){
  // The STEP pin must not change for at least 200 nanoseconds before and after
  // changing the DIR pin.
  delayMicroseconds(1);
  digitalWrite(DirPin, !dir);  
  delayMicroseconds(1);
}

void moveToPosition(double newPosA, double newPosB, int motorA[], int motorB[], int *posA, int *posB, int stepRes){
  // This function will move the motor to a desired position specfied by newPos
  
  //int stepCounter = pos;

  float posFixA = newPosA*100;
  float posFixB = newPosB*100;

  int diffA = int(posFixA) - *posA;
  int diffB = int(posFixB) - *posB;
  int stallA = 1;
  int stallB = 1;
  int A = 0;
  int B = 0;

  bool Afin = false;
  bool Bfin = false;

  if (diffA <= 0 && diffB <= 0) {  
    setDirection(1,motorA[1]);
    setDirection(1,motorB[1]);
    digitalWrite(motorA[5], HIGH);
    digitalWrite(motorB[5], HIGH);
    while (true) {
      if (PINF & (1 << PF5) && !Afin && useLimits){ // bounds reached
        Serial.println("A In Limit Reached");
        digitalWrite(motorA[5], LOW);
        Afin = true;
                
      }
      if (PINF & (1 << PF1) && !Bfin && useLimits){ // bounds reached
        Serial.println("B In Limit Reached");
        digitalWrite(motorB[5], LOW);
        Bfin = true;
      }
      
      if (stallA) {  //(!digitalRread(motor[2])){  // Stall 
        if (A < abs(diffA) && !Afin){
          if (B >= abs(diffB)){
            for (int j=0; j<stepRes; j++){        
            stepSingle(motorA[0], true);
            //_delay_ms(fast);  
            }
            *posA -= 1;
            A++;
          }
          else {
            for (int j=0; j<stepRes; j++){        
              step(motorA[0], true);
              //_delay_ms(fast);  
            }
            *posA -= 1;
            A++;
          }
        }
        else {
          Afin = true;
        }
      }
      else {
        Serial.println("Motor A Stall");
        digitalWrite(motorA[5], LOW);
        Afin = true;
        break;
      } 
      if (stallB) {  //(!digitalRread(motor[2])){  // Stall 
        if (B < abs(diffB) && !Bfin) { 
          if (A >= abs(diffA)){
            for (int j=0; j<stepRes; j++){        
            stepSingle(motorB[0], true);
            //_delay_ms(fast);  
            }
            *posB -= 1;
            B++;
          }
          else {
            for (int j=0; j<stepRes; j++){        
              step(motorB[0], true);
              //_delay_ms(fast);  
            }
            *posB -= 1;
            B++;
          }
        }
        else {
          Bfin = true;
        }
      }
      else {
        Serial.println("Motor B Stall");
        digitalWrite(motorB[5], LOW);
        Bfin = true;
        break;
      } 
      if (Afin && Bfin){
        digitalWrite(motorA[5], LOW);
        digitalWrite(motorB[5], LOW);
        break;
      }
      if (Serial.available() > 0){
        digitalWrite(motorA[5], LOW);
        digitalWrite(motorB[5], LOW);
        break;
      }
    }
  }

  else if (diffA >= 0 && diffB >= 0) {  
    setDirection(0,motorA[1]);
    setDirection(0,motorB[1]);
    digitalWrite(motorA[5], HIGH);
    digitalWrite(motorB[5], HIGH);
    while (true) {
      if (PINF & (1 << PF4) && !Afin && useLimits){ // bounds reached
        Serial.println("A Out Limit Reached");
        digitalWrite(motorA[5], LOW);
        Afin = true;
                
      }
      if (PINF & (1 << PF0) && !Bfin && useLimits){ // bounds reached
        Serial.println("B Out Limit Reached");
        digitalWrite(motorB[5], LOW);
        Bfin = true;
      }
      
      if (stallA) {  //(!digitalRread(motor[2])){  // Stall 
        if (A < abs(diffA) && !Afin){
          if (B >= abs(diffB)){
            for (int j=0; j<stepRes; j++){        
            stepSingle(motorA[0], true);
            //_delay_ms(fast);  
            }
            *posA -= 1;
            A++;
          }
          else {
            for (int j=0; j<stepRes; j++){        
              step(motorA[0], true);
              //_delay_ms(fast);  
            }
            *posA += 1;
            A++;
          }
        }
        else {
          Afin = true;
        }
      }
      else {
        Serial.println("Motor A Stall");
        digitalWrite(motorA[5], LOW);
        Afin = true;
        break;
      } 
      if (stallB) {  //(!digitalRread(motor[2])){  // Stall 
        if (B < abs(diffB) && !Bfin) { 
          if (A >= abs(diffA)){
            for (int j=0; j<stepRes; j++){        
            stepSingle(motorB[0], true);
            //_delay_ms(fast);  
            }
            *posB -= 1;
            B++;
          }
          else {
            for (int j=0; j<stepRes; j++){        
              step(motorB[0], true);
              //_delay_ms(fast);  
            }
            *posB += 1;
            B++;
          }
        }
        else {
          Bfin = true;
        }
      }
      else {
        Serial.println("Motor B Stall");
        digitalWrite(motorB[5], LOW);
        Bfin = true;
        break;
      } 
      if (Afin && Bfin){
        digitalWrite(motorA[5], LOW);
        digitalWrite(motorB[5], LOW);
        break;
      }
      if (Serial.available() > 0){
        digitalWrite(motorA[5], LOW);
        digitalWrite(motorB[5], LOW);
        break;
      }
    }
  }

  else if (diffA <= 0 && diffB >= 0) {  
    setDirection(1,motorA[1]);
    setDirection(0,motorB[1]);
    digitalWrite(motorA[5], HIGH);
    digitalWrite(motorB[5], HIGH);
    while (true) {
      if (PINF & (1 << PF5) && !Afin && useLimits){ // bounds reached
        Serial.println("A In Limit Reached");
        digitalWrite(motorA[5], LOW);
        Afin = true;
                
      }
      if (PINF & (1 << PF0) && !Bfin && useLimits){ // bounds reached
        Serial.println("B Out Limit Reached");
        digitalWrite(motorB[5], LOW);
        Bfin = true;
      }
      
      if (stallA) {  //(!digitalRread(motor[2])){  // Stall 
        if (A < abs(diffA) && !Afin){
          if (B >= abs(diffB)){
            for (int j=0; j<stepRes; j++){        
            stepSingle(motorA[0], true);
            //_delay_ms(fast);  
            }
            *posA -= 1;
            A++;
          }
          else {
            for (int j=0; j<stepRes; j++){        
              step(motorA[0], true);
             // _delay_ms(fast);  
            }
            *posA -= 1;
            A++;
          }
        }

        else {
          Afin = true;
        }
      }
      else {
        Serial.println("Motor A Stall");
        digitalWrite(motorA[5], LOW);
        Afin = true;
        break;
      } 
      if (stallB) {  //(!digitalRread(motor[2])){  // Stall 
        if (B < abs(diffB) && !Bfin) { 
          if (A >= abs(diffA)){
            for (int j=0; j<stepRes; j++){        
            stepSingle(motorB[0], true);
            // _delay_ms(fast);  
            }
            *posB -= 1;
            B++;
          }
          else {
            for (int j=0; j<stepRes; j++){        
              step(motorB[0], true);
              // _delay_ms(fast);  
            }
            *posB += 1;
            B++;
          }
        }
        else {
          Bfin = true;
        }
      }
      else {
        Serial.println("Motor B Stall");
        digitalWrite(motorB[5], LOW);
        Bfin = true;
        break;
      } 
      if (Afin && Bfin){
        digitalWrite(motorA[5], LOW);
        digitalWrite(motorB[5], LOW);
        break;
      }
      if (Serial.available() > 0){
        digitalWrite(motorA[5], LOW);
        digitalWrite(motorB[5], LOW);
        break;
      }
    }
  }

  else if (diffA >= 0 && diffB <= 0) {  
    setDirection(0,motorA[1]);
    setDirection(1,motorB[1]);
    digitalWrite(motorA[5], HIGH);
    digitalWrite(motorB[5], HIGH);
    while (true) {
      if (PINF & (1 << PF4) && !Afin && useLimits){ // bounds reached
        Serial.println("A Out Limit Reached");
        digitalWrite(motorA[5], LOW);
        Afin = true;
                
      }
      if (PINF & (1 << PF1) && !Bfin && useLimits){ // bounds reached
        Serial.println("B In Limit Reached");
        digitalWrite(motorB[5], LOW);
        Bfin = true;
      }
      
      if (stallA) {  //(!digitalRread(motor[2])){  // Stall 
        if (A < abs(diffA) && !Afin){
          if (B >= abs(diffB)){
            for (int j=0; j<stepRes; j++){        
            stepSingle(motorA[0], true);
            // _delay_ms(fast);  
            }
            *posA -= 1;
            A++;
          }
          else {
            for (int j=0; j<stepRes; j++){        
              step(motorA[0], true);
              // _delay_ms(fast);  
            }
            *posA += 1;
            A++;
          }
        }
        else {
          Afin = true;
        }
      }
      else {
        Serial.println("Motor A Stall");
        digitalWrite(motorA[5], LOW);
        Afin = true;
        break;
      } 
      if (stallB) {  //(!digitalRread(motor[2])){  // Stall 
        if (B < abs(diffB) && !Bfin) { 
          if (A >= abs(diffA)){
            for (int j=0; j<stepRes; j++){        
            stepSingle(motorB[0], true);
            // _delay_ms(fast);  
            }
            *posB -= 1;
            B++;
          }
          else {
            for (int j=0; j<stepRes; j++){        
              step(motorB[0], true);
              // _delay_ms(fast);  
            }
            *posB -= 1;
            B++;
          }
        }
        else {
          Bfin = true;
        }
      }
      else {
        Serial.println("Motor B Stall");
        digitalWrite(motorB[5], LOW);
        Bfin = true;
        break;
      } 
      if (Afin && Bfin){
        digitalWrite(motorA[5], LOW);
        digitalWrite(motorB[5], LOW);
        break;
      }
      if (Serial.available() > 0){
        digitalWrite(motorA[5], LOW);
        digitalWrite(motorB[5], LOW);
        break;
      }
    }
  }
}
 
void calibrate(int motorA[], int motorB[]){
  bool stall = false;
  bool doneP1A = false;
  bool doneP1B = false;
  bool doneP2A = false;
  bool doneP2B = false;
  // int stallA = motorA[2];
  // int stallB = motorB[2];
  int stallA = 1;
  int stallB = 1;
  setDirection(1,motorA[1]);
  setDirection(1,motorB[1]);
  digitalWrite(motorA[5], HIGH);
  digitalWrite(motorB[5], HIGH);

  while(true){
    step(motorA[0], true);
    // _delay_ms(fast);
    step(motorB[0], true);
    // _delay_ms(fast);

    if (PINF & (1 << PF5) && !doneP1A){
        digitalWrite(motorA[5], LOW);
        doneP1A = true;
    }
    if (PINF & (1 << PF1) && !doneP1B){
      digitalWrite(motorB[5], LOW);
      doneP1B = true;
    }
    if (!stallA){
      digitalWrite(motorA[5], LOW);
      Serial.println("Stall");
      stall = true;
      break;
    } 
    if (!stallB){
      digitalWrite(motorB[5], LOW);
      Serial.println("Stall");
      stall = true;
      break;      
    }          
    if (doneP1A && doneP1B){
      break;
    }
    if (Serial.available() > 0){
      digitalWrite(motorA[5], LOW);
      digitalWrite(motorB[5], LOW);
      break;
    }
  } 
  delay(1000);
  if (!stall){  
    setDirection(0,motorA[1]);
    setDirection(0,motorB[1]);
    digitalWrite(motorA[5], HIGH);
    digitalWrite(motorB[5], HIGH);
    for (int i=0;i<2560;i++){
      step(motorA[0], false);
      // _delay_ms(slow); 
      step(motorB[0], false);  
      // _delay_ms(slow); 
      if (Serial.available() > 0){
        digitalWrite(motorA[5], LOW);
        digitalWrite(motorB[5], LOW);
        break;
      } 
    }
    setDirection(1, motorA[1]);
    setDirection(1, motorB[1]);
  }
  delay(1000);
  while(!stall){
    step(motorA[0], false);
    // _delay_ms(slow);
    step(motorB[0], false);
    // _delay_ms(slow);
    if (Serial.available() > 0){
      digitalWrite(motorA[5], LOW);
      digitalWrite(motorB[5], LOW);      
      break;
    } 
    if (PINF & (1 << PF5) && !doneP2A){
      digitalWrite(motorA[5], LOW);
      doneP2A = true;
    }
    if (PINF & (1 << PF1) && !doneP2B){
      digitalWrite(motorB[5], LOW);
      doneP2B = true;
    }
    if (!stallA){
      digitalWrite(motorA[5], LOW);
      Serial.println("Stall");
      stall = true;
      break;
    } 
    if (!stallB){
      digitalWrite(motorB[5], LOW);
      Serial.println("Stall");
      stall = true;
      break;      
    }          
    if (doneP2A && doneP2B){
      break;
    }
  } 
}

double measureBounds(int motor[], int stepRes){

  int stallV = 1;
  int stall = 0;
  double pos = 0;

  setDirection(1,motor[1]);
  digitalWrite(motor[5], HIGH);

  while (true) {
    for(int i = 0; i<stepRes; i++){
      stepSingle(motor[0], true);
      // _delay_ms(fast);
    }

    if (Serial.available() > 0){
      digitalWrite(motor[5], LOW);
      break;
    } 
    if (digitalRead(motor[3])){
      //digitalWrite(motor[5], LOW);
      break;
    }
    if (!stallV) {
      digitalWrite(motor[5], LOW);
      Serial.print("Stall");
      stall = 1;
      break;
    }
  }

  setDirection(0,motor[1]);
  while (!stall) {
    //Serial.println("HERE");
    for(int i = 0; i<stepRes; i++){
      stepSingle(motor[0], true);
      // _delay_ms(fast);
    }
    pos += 0.01;
    if (Serial.available() > 0){
      digitalWrite(motor[5], LOW);
      break;
    }
    if (!stallV) {
      digitalWrite(motor[5], LOW);
      Serial.print("Stall");
      break;
    }

    if (digitalRead(motor[4])){
      digitalWrite(motor[5], LOW);
      break;
    }
    
  }
  digitalWrite(motor[5], LOW);
  
  return pos;
}






