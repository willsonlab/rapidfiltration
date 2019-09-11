#include <Streaming.h>
#include <elapsedMillis.h>

// Controlling frequency signal output 
//Output pins
#define pulsePin 5
#define enablePin 6
#define solenoidPin 4

//Input pins
#define switchInterrupt 0
#define switchPin 2
#define contactPin 7

elapsedMillis timer1;
elapsedMillis timeElapsed; //

int pumpAtBottom=0;
String inputString;
String incomingStr;
String Cmd1_IDtag="GO";
String Cmd2_IDtag="CAL";
String Flowrate_setting;
String Time_setting;
boolean GoCommand, CalCommand, GoCalFlag, errorFlag,contactStatus,deadendStatus;
boolean runningStarted=false;
int  pulseSetting;
int  runTime;
int updatetime =1000;

boolean runReady=false;

void endswitchISR()
{
      if (digitalRead(switchPin)== HIGH)
       {
       digitalWrite(solenoidPin,LOW);
       digitalWrite(enablePin,LOW);
       timeElapsed=0; 
       noTone(pulsePin); 
       digitalWrite(pulsePin,LOW);
       GoCommand=false;
       CalCommand=false;
       errorFlag=true;
       deadendStatus=true;
       incomingStr="";
       Serial <<"SW1";
       delay(1000);
       Serial<<"Stop";
       }
       else
       {
        Serial <<"SW0";
       deadendStatus=false;
       }
}


void setup() 
{ 
 attachInterrupt(switchInterrupt,endswitchISR,CHANGE);
  Serial.begin(9600);
  pinMode(pulsePin,OUTPUT);
  pinMode(enablePin,OUTPUT);
  pinMode(solenoidPin,OUTPUT);
  pinMode(contactPin,INPUT);
  pinMode(switchPin,INPUT);
  digitalWrite(pulsePin,LOW);
  digitalWrite(solenoidPin,LOW);
  digitalWrite(enablePin,LOW);
     
  CalCommand=false;
  GoCalFlag=true;
  GoCommand=false;
  CalCommand=false;
  deadendStatus=false;
  
elapsedMillis timer1;
} 
 
void loop() 
{ 
if (timer1>500)
{
  contactStatus=!(digitalRead(contactPin));
  deadendStatus=(digitalRead(switchPin));  
  if (deadendStatus)
  Serial<<"SW1";
  else
  Serial<<"SW0";
  timer1=0;
}
   
if (Serial.available () > 0){
    processIncomingByte (Serial.read ());}

if (GoCommand && !deadendStatus)
{
  runProtocol (true,pulseSetting,runTime);
}
else if (CalCommand && !deadendStatus)
{
  runProtocol (false,pulseSetting,runTime);
}

}






void processData (String data)
  {

if (data.length()>3)
  {
   int Cmd1_labelpos = data.indexOf(Cmd1_IDtag);      // position of "Cmd" in str
   int Cmd2_labelpos = data.indexOf(Cmd2_IDtag); 
   Serial << (Cmd1_labelpos>0);
   if  (Cmd1_labelpos>=0 && Cmd1_labelpos <data.length())
    {
      GoCommand=true;
      CalCommand=false;
      GoCalFlag=true;
      int Flowrate_endpos = data.indexOf(',');
      Flowrate_setting=data.substring(Cmd1_labelpos+Cmd1_IDtag.length(),Flowrate_endpos);
      Time_setting= data.substring(Flowrate_endpos+1,data.length());
      pulseSetting=Flowrate_setting.toInt();
      runTime=Time_setting.toInt();
    // Serial << Flowrate_setting<< ":"<<Time_setting;
     Serial << data;
    }
    else if  (Cmd2_labelpos>=0 && Cmd2_labelpos <data.length())
    {
      CalCommand=true;
      GoCommand=false;
      GoCalFlag=false;
      int Flowrate_endpos = data.indexOf(',');
      Flowrate_setting=data.substring(Cmd2_labelpos+Cmd2_IDtag.length(),Flowrate_endpos);
      Time_setting= data.substring(Flowrate_endpos+1,data.length());
      pulseSetting=Flowrate_setting.toInt();
      runTime=Time_setting.toInt();
     //Serial << Flowrate_setting<< ":"<<Time_setting;
      Serial << data;
    }
   
    }
data="";
  }  // end of processData
  

void processIncomingByte (char c)
  {
  switch (c)
    {
    case '!':   // end of text
      // terminator reached! process inputLine here ...
      processData (incomingStr);
      // reset for next time
      incomingStr="";  
      break;
  
    default:
      // keep adding
      incomingStr += c;
      break;
  
    }  // end of switch
  } // end of processIncomingByte



void runProtocol (boolean GoCalFlag, int pulseOut,int duration)
{
 if (GoCalFlag)
  {   
   digitalWrite(solenoidPin,HIGH);
   if (digitalRead(contactPin)==LOW)
    runReady=true;
  } 
  else
    runReady=true;

  if (runReady);
    { 
       if (!runningStarted)
          {
            
            
            timeElapsed=0;
            runningStarted=true;
          }
       if (timeElapsed<duration)
       {
        Serial<<"Start";
       tone(pulsePin,pulseOut);
       digitalWrite(enablePin,HIGH);
      // delay(runTime);
       }
       else
       {
       digitalWrite(solenoidPin,LOW);
       digitalWrite(enablePin,LOW);
       timeElapsed=0; 
       noTone(pulsePin); 
       digitalWrite(pulsePin,LOW);
       GoCommand=false;
       CalCommand=false;
       runReady=false;
       runningStarted=false;
       Serial<<"Stop";
      } 
  }

  }


