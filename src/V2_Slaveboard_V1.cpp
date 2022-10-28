#include <Arduino.h>
#include <Wire.h>
#include <BitArray.h>

// 40 Pin Assignment
// Resistor Power
byte CR1 = 1;  // 1
byte CR2 = 0;  // 33
byte CR3 = 32; // 39
byte CR4 = 31; // 38
byte CR5 = 30; // 37
byte CR6 = 29; // 36
byte CR7 = 28; // 35

// Collunm power
// C = collum number, starting on left, used to power the collunms 1 at a time.
byte C1 = 8;
byte C2 = 9;
byte C3 = 10;
byte C4 = 11;
byte C5 = 7;
byte C6 = 6;
byte C7 = 5;

// Row data
//  R = Row Number, starting at top, used to read the rows when a collunm is powered
byte R1 = 14; // 22
byte R2 = 15; // 23
byte R3 = 16; // 24
byte R4 = 17; // 25
byte R5 = 18; // 26
byte R6 = 19; // 27

// 3 usable LEDS on board;

int Led1 = 26;
int Led2 = 25;
int Led3 = 24;

// transostor for i2c line
int Sdasig = 12;

/*

//48 Pin Assigment
//Resistor Power
byte CR1 = 1;   //1
byte CR2 = 0;  //33
byte CR3 = 40;  //39
byte CR4 = 39;  //38
byte CR5 = 38;  //37
byte CR6 = 37;  //36
byte CR7 = 36;  //35

//Collunm power
//C = collum number, starting on left, used to power the collunms 1 at a time.
byte C1 = 14;
byte C2 = 15;
byte C3 = 16;
byte C4 = 17;
byte C5 = 7;
byte C6 = 6;
byte C7 = 5;


//Row data
// R = Row Number, starting at top, used to read the rows when a collunm is powered
byte R1 = 22; //22
byte R2 = 23;  //23
byte R3 = 24;  //24
byte R4 = 25;  //25
byte R5 = 26;  //26
byte R6 = 27;  //27

//3 usable LEDS on board;

int Led1 = 34;
int Led2 = 33;
int Led3 = 32;

*/

byte assignmentArray[6][7] = {
    // array to hold assignment for possition, digital or analogue
    {1, 1, 1, 1, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
};

// 1 button 2 analog 0 empty

byte analogChanges = 0; // merge these 2 later
byte digitalChanges = 0;
int changesSent = 0;

byte changed[6][7] = {
    // 1 or 0 has the value change from previous
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
};

int analogreads = 0;
int analogArray[6][7] = {
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
};

int moduleID[6][7] = {
    // module ID array called for in initilization in the master
    {1024, 1024, 32, 1024, 512, 256, 1024},
    {0, 0, 0, 0, 1024, 0, 14},
    {0, 16, 0, 255, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
};

int keyAssignment[6][7] = {
    // byte value of the ascii chariter to be sent.
    {102, 103, 104, 105, 106, 107, 108},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
};

byte testRead = 0;

byte rowArray[6] = {R1, R2, R3, R4, R5, R6};     // put the rows in an array so the can be incremented through
byte colArray[7] = {C1, C2, C3, C4, C5, C6, C7}; // put the collunms in an array so the can be incremented through

int Row = 1;
int Col = 1;

int oldButtonArray[6][7] = {
    // previously sent buttons to the master
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
};

int dButtonArray[6][7] = {
    // new button positions and values.
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0},
};
bool sentdigital = false;

/*          collunms
r* byte 1 = 11,12,13,14,15,16,17
o* byte 2 = 21,22,23,24,25,26,27
w* byte 3 = 31,32,33,34,35,36,37
 * byte 4 = 41,42,43,44,45,46,47
 * byte 5 = 51,52,53,54,55,56,57
 * byte 6 = 61,62,63,64,65,66,67
 */
bool sendingLoad = false;
int analogread1 = 0;
int analogread2;

int digitalLoadSize = 0;
int analogLoadSize = 0;
bool sentLoadSize = false;
bool initilize = false;
int SlaveID = 253;
bool firstContact = false;
byte moduleIDChunk = 0; // Module ID has to be sent in 3 parts due to 32 bit limit on i2c, this keeps track of how much has been sent
byte runs = 0;
int makeLinear(float y)
{
  float a = 102; // pot max
  float b = 47;  // input resistor
  float c = 47;  // ouput resistor

  y = y / 1024;

  if (y < 0.0001)
  {
    y = 0.0001;
  }

  return ((-(-pow(a, 2) * y - a * b * y + c * a - sqrt(pow((pow(a, 2) * y + a * b * y - c * a), 2) + 4 * pow(a, 2) * y * (c * a * y + c * b * y))) / (2 * pow(a, 2) * y)) * 1024);
}

void sendButton(byte buttonState, int keyData)
{
  Wire.write(buttonState);
  Wire.write(highByte(keyData)); // send byte 1
  Wire.write(lowByte(keyData));  // send byte 2
}

void receiveEvent(int receiveSize)
{
  // initialize the slave with the new ID from the master and start the new wire session
  // also opens the bypass transistor to allow communication to the next slave in line.
  if (initilize == false)
  {
    SlaveID = Wire.read();
    Wire.end();
    Wire.begin(SlaveID);
    initilize = true;
    Sdasig = HIGH;
    digitalWrite(Led3, HIGH);
  }

  // prepare the load size
  if (initilize == true)
  {
    if (Wire.read() == 10)
    { // prepare load size
      sentLoadSize = false;
      digitalLoadSize = (digitalChanges * 3); // sends the 4 data points for each button change
      analogLoadSize = (analogChanges * 3);   // send the 3 datapoints for each analog change
    }
  }
}

void requestEvent()
{

  // initiates first contact and sends back data
  if (firstContact == false)
  { // when initilizing
    Wire.write(253);
    firstContact = true;
  }

  /*{ // module ID system
  //***** MODULE ASSINGMENT WORK ON LATER PLZ
  // sending the moduleID array to the master // this will be called by the master 3 times to get all the data.
  if (firstContact == true && initilize == false && moduleIDChunk == 0)
  { // used the moduleIDChunk to keep track of data sent
    for (byte i = 0; i < sizeof(rowArray); i++)
    { // sizeof(rowArray)
      for (byte j = 0; j < sizeof(colArray); j++)
      {
        Wire.write(highByte(moduleID[i][j]));
        Wire.write(lowByte(moduleID[i][j]));
      }
    }
    moduleIDChunk = 1;
  }
  }*/

  if (initilize == true)
  { // normal operation

    sendingLoad = true;

    digitalLoadSize = 0;
    for (int row = 0; row < sizeof(rowArray); row++) // calulate digital load size here
    {
      for (int col = 0; col < sizeof(colArray); col++)
      {

        if (changed[row][col] == 1)
        {
          changed[row][col] = 0;
          digitalLoadSize++; // should fully reset the changed matrix
        }
      }
    }

    if (sentLoadSize == false)
    {
      Wire.write(digitalLoadSize);
      Wire.write(analogLoadSize);
      sentLoadSize = true; // send the load size

      while (digitalLoadSize / 32 > 1) // start generating runs
      {
        runs++;
        digitalLoadSize - 32;
      }
    }
    else // start sending the load
    {

      if (sentdigital == false)
      {
        if (runs != 0)
        {
          for (byte i = 0; i < 32; i++) // sending the bytes in 32-lot chunks
          {
            // send bytes here
          }

          runs--;
        }
        else
        {
          for (byte i = 0; i < digitalLoadSize; i++) // will send the remaining digital load that is less than 32 bytes
          {
            // send bytes here
          }
          digitalLoadSize = 0;
          sentdigital = true;
          sentLoadSize = false; // reset flag for load sent // move back down to bottom after
        }
        
      }

      // sending the 6 buttons

      // for (int i = 0; i < sizeof(dButtonArray); i++)
      // {
      //   if (digitalChanges < 1)
      //   { // break out of the digital loop if all the data has been sent
      //     break;
      //   }
      //   for (int k = 0; k < 8; k++)
      //   {
      //     if (bitRead(dButtonArray[i], k) != bitRead(oldButtonArray[i], k)) // checks if the key press has changed from the previous send

      //       sendButton(bitRead(dButtonArray[i], k), keyAssignment[i][k]); // if it has send the data to the master.
      //     digitalChanges--;
      //   }
      // }
      // int changesSent = 0; // resets changes sent counter to be used again
      // for (int i = 0; i < sizeof(dButtonArray); i++)
      // {
      //   oldButtonArray[i] = dButtonArray[i];
      //   dButtonArray[i] = 0;
      // }
     
     
     
     
     
      // if (sentdigital == true)
      // {
      //   for (int i = 0; i < sizeof(rowArray); i++)
      //   { // loop through the rows

      //     if (analogChanges == changesSent)
      //     { // break if all analog changes have been sent by the end of the row
      //       break;
      //     }
      //     for (int j = 0; j < sizeof(colArray); j++) // loop through the collunms
      //     {

      //       if (analogChanges == changesSent)
      //       { // break if all analog changes have been sent by the end of the collunm
      //         break;
      //       }

      //       if (assignmentArray[i][j] == 102) // checks if the analog device has its change waiting 102 = changed 2 = not changed
      //       {

      //         analogread1 = makeLinear(analogArray[i][j]); // read the analog value to send

      //         Wire.write((i * 6) + (j + 1));     // write the possition of the analog device on the board
      //         Wire.write(highByte(analogread1)); // send byte 1
      //         Wire.write(lowByte(analogread1));  // send byte 2
      //         assignmentArray[i][j] = 2;         // set the analog flag back to no change wiating
      //         changesSent++;                     // increment the amout of changes currently set
      //       }
      //     }
      //   }

      //   analogChanges = 0; // reset counter for amount of analog changes wiating to be sent.

        
      // }

    }
  }
}

void setup()
{

  pinMode(Sdasig, OUTPUT);
  digitalWrite(Sdasig, LOW);

  pinMode(Led1, OUTPUT);
  pinMode(Led2, OUTPUT);
  pinMode(Led3, OUTPUT);

  digitalWrite(Led1, LOW);
  digitalWrite(Led2, LOW);
  digitalWrite(Led3, LOW);

  pinMode(R1, INPUT);
  pinMode(R2, INPUT);
  pinMode(R3, INPUT);
  pinMode(R4, INPUT);
  pinMode(R5, INPUT);
  pinMode(R6, INPUT);

  pinMode(C1, OUTPUT);
  pinMode(C2, OUTPUT);
  pinMode(C3, OUTPUT);
  pinMode(C4, OUTPUT);
  pinMode(C5, OUTPUT);
  pinMode(C6, OUTPUT);
  pinMode(C7, OUTPUT);

  pinMode(CR1, OUTPUT);
  pinMode(CR2, OUTPUT);
  pinMode(CR3, OUTPUT);
  pinMode(CR4, OUTPUT);
  pinMode(CR5, OUTPUT);
  pinMode(CR6, OUTPUT);
  pinMode(CR7, OUTPUT);

  digitalWrite(CR1, LOW);
  digitalWrite(CR2, LOW);
  digitalWrite(CR3, LOW);
  digitalWrite(CR4, LOW);
  digitalWrite(CR5, LOW);
  digitalWrite(CR6, LOW);
  digitalWrite(CR7, LOW);

  /*
  for (int i=0; i < sizeof(rowArray); i++  ){
      for (int j=0; j < sizeof(colArray); j++){
        digitalWrite(colArray[j], HIGH); // powers on the collunm for both button and analog read
        moduleID[i][j] = analogRead(rowArray[i]); // writes the analog value to the module ID array
        digitalWrite(colArray[j], LOW);  // de power the collunm to move onto the next one
        }
    }
  */

  Wire.begin(253); // all slaves start at 253 then get assigned a i2c ID by master
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);

  // need to put in module
}

void loop()
{
  digitalWrite(Led1, LOW);
  // loop through rows
  // loop through each collunm
  // turn collum on
  // is button or analog
  // is button on or off?
  // read collum
  // turn collum off

  if (initilize == true || sendingLoad == false)
  {

    for (int row = 0; row < sizeof(rowArray); row++)
    {
      for (int col = 0; col < sizeof(colArray); col++)
      {
        digitalWrite(colArray[col], HIGH); // powers on the collunm for both button and analog read

        if (assignmentArray[row][col] == 1)
        { // find if the component is a button

          if (digitalRead(rowArray[row]) == HIGH)
          { // reads the current row

            dButtonArray[row][col] = 1; // write that the button is pressed
          }
          else
          {

            dButtonArray[row][col] = 0; // else clear the button press data
          }

          if (dButtonArray[row][col] != oldButtonArray[row][col])
          {                        // checks if the key press has changed from the previous send
            changed[row][col] = 1; // enter the change into the changed matrix
                                   // will need to calulate the digital changes after
            digitalWrite(Led1, HIGH);
          }
          else
          {

            changed[row][col] = 0; // if it is the same the clear the change from the change matrix
          }
        }
        if (assignmentArray[row][col] == 2 || assignmentArray[row][col] == 102)
        { // check if it is a analog assignment

          analogreads = analogRead(rowArray[row]); // reads the current row
          if (analogArray[row][col] != analogreads)
          { // checks if the current value for the anallog device is diffrent from its previous value

            analogArray[row][col] = analogreads; // set the new analog value
            if (assignmentArray[row][col] == 2)  // checks if the analog device is already updated
            {
              analogChanges++; // add a analog change to be sent on the next data sent to the master
            }
            assignmentArray[row][col] = 102; // sets the analog device to be un a updated state

            // analogArray[i][j] = analogreads;
          }
        }

        digitalWrite(colArray[col], LOW); // de power the collunm to move onto the next one
      }
    }
  }
}