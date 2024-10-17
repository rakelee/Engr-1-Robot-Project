// C++ code
//
#include <Servo.h>

Servo myservo;

//outputs
#define LED_W1 2
#define LED_W2 3
#define LED_G  4
#define LED_W3 5
#define LED_W4 6

//inputs
#define START  9
#define PLAY   8
//motor
#define MOTOR 11  

//Start states
#define START_WAITFORSTART 0
#define START_STARTGAME    1
#define START_WAITFOREND   2
int nStartState = START_WAITFORSTART;

//LED states
#define LED_WAITFORGAME 0
#define LED_ON          1
#define LED_ONWAIT      2
#define LED_OFF         3
#define LED_OFFWAIT     4
#define LED_GAMEOVER    5
int nLEDState = LED_WAITFORGAME;

int nActiveLED;
int nDelayTimeOn =  50;
int nDelayTimeOff = 50;
unsigned long nTimeOnMs;
unsigned long nTimeOffMs;
int nIncrement = 1;

//Play states
#define PLAY_WAITFORSTART 0
#define PLAY_GAMEACTIVE   1
#define PLAY_GAMEWIN      2
#define PLAY_GAMELOSE     3
#define PLAY_LOSETIME   750   
int nPlayState = PLAY_WAITFORSTART;

unsigned long nWaitTimeMs;

//Motor states
#define MOTOR_INITIALIZE      0
#define MOTOR_WAITFORWIN      1
#define MOTOR_OPEN            2
#define MOTOR_WAITFOROPEN     3
#define MOTOR_CLOSE           4
int nMotorState = MOTOR_INITIALIZE;

#define MOTOR_ANGLE          45
#define MOTOR_ANGLEINCREMENT 15
#define MOTOR_ANGLEDELAYMS   15
#define CANDY_FLOWMS      500


void setup()
{
  pinMode(LED_W1, OUTPUT);
  pinMode(LED_W2, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_W3, OUTPUT);
  pinMode(LED_W4, OUTPUT);
          
  myservo.attach(MOTOR);  
  pinMode(START,INPUT_PULLUP);
  pinMode(PLAY,INPUT_PULLUP);
}

void loop()
{
  StartFSM();
    LEDFSM();
    PlayFSM();
    MotorFSM();
}
void StartFSM()
{
     switch(nStartState)
     {
      case START_WAITFORSTART:
          nLEDState = LED_WAITFORGAME;
          nPlayState = PLAY_WAITFORSTART;
          if(digitalRead(START) == LOW)
            {
              nStartState = START_STARTGAME;
            }
          break;
       
        case START_STARTGAME:
          nLEDState = LED_ON;
          nStartState = START_WAITFOREND;
          nPlayState = PLAY_GAMEACTIVE;
            break;
       
        case START_WAITFOREND:
          break;
     }
}
void LEDFSM()
{
  switch(nLEDState)
    {
      case LED_WAITFORGAME:
          for (int LED = LED_W1; LED <= LED_W4; LED++)
            {
              digitalWrite(LED, 0);
            }
          nActiveLED = LED_W1;
          break;
      
    case LED_ON:
          digitalWrite(nActiveLED, HIGH);
          nTimeOnMs = millis();
          nLEDState = LED_ONWAIT;
         break;
      
    case LED_ONWAIT:
          if(millis() - nTimeOnMs > nDelayTimeOn)
            {
              nLEDState = LED_OFF;
            }
          break;
      
    case LED_OFF:
          digitalWrite(nActiveLED, LOW);
          nTimeOffMs = millis();
          nLEDState = LED_OFFWAIT;
          break;
      
    case LED_OFFWAIT:
          if(millis() - nTimeOffMs > nDelayTimeOff)
            {
                nActiveLED += nIncrement;
            if(nActiveLED > LED_W4)
              {
                nActiveLED = LED_W3;
                  nIncrement = -1;
              }
            if(nActiveLED < LED_W1)
              {
                nActiveLED = LED_W2;
                  nIncrement = 1;
              }
              nLEDState = LED_ON; //cycling to the next LED
            }
          
          break;
      
    case LED_GAMEOVER:
          if(nActiveLED != LED_G)
            {
              digitalWrite(nActiveLED, HIGH);
            }
          break;
      
    }
  
}
void PlayFSM()
{
  switch(nPlayState)
    {
      case PLAY_WAITFORSTART:
          break;
      
    case PLAY_GAMEACTIVE:
          if(digitalRead(PLAY) == LOW)
            {
              nLEDState = LED_GAMEOVER;
                if(nActiveLED == LED_G)
                {
                  nPlayState = PLAY_GAMEWIN;
                }
                else
                {
                  nWaitTimeMs = millis();
                    nPlayState = PLAY_GAMELOSE;
                }
            }
          break;
      
    case PLAY_GAMEWIN:
          for(int i = 0; i<3; i++)
            {
              digitalWrite(LED_G,HIGH);
                delay(100);
                digitalWrite(LED_G,LOW);
                delay(100);
            }
          digitalWrite(LED_G,HIGH);
          nMotorState = MOTOR_OPEN;
          nPlayState = PLAY_WAITFORSTART;
          break;
      
        case PLAY_GAMELOSE:
          if(millis() - nWaitTimeMs > PLAY_LOSETIME)
            {
              nStartState = START_WAITFORSTART;
            }
          
          break;
    }
}
void MotorFSM()
{
  int pos;
  
    switch(nMotorState)
    {
      case MOTOR_INITIALIZE:
          myservo.write(0);
          nMotorState = MOTOR_WAITFORWIN;
          break;
      
    case MOTOR_WAITFORWIN:
          break;
      
    case MOTOR_OPEN:
          for (pos=0; pos<=MOTOR_ANGLE; pos+=MOTOR_ANGLEINCREMENT) 
            { 
          myservo.write(pos);              
          delay(MOTOR_ANGLEDELAYMS);
      }
          nMotorState = MOTOR_WAITFOROPEN;
          break;
      
    case MOTOR_WAITFOROPEN:
          delay(CANDY_FLOWMS);
          nMotorState = MOTOR_CLOSE;
          break;
      
    case MOTOR_CLOSE:
        for (pos=MOTOR_ANGLE; pos>=0; pos-=MOTOR_ANGLEINCREMENT)
            { 
          myservo.write(pos);         
          delay(MOTOR_ANGLEDELAYMS);
            }
          nMotorState = MOTOR_INITIALIZE;
          nStartState = START_WAITFORSTART;
          break;
    }
  
}
