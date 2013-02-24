/*
  Counting Box.
 Counts the numer of times a box is opened, and stores the date.
 The data accumulated can be checked via serial.
 When a button is pressed, the serial interface is connected and the recorded info is sent,
 and the count restarts.
 (in the future, via ADK with an android phone)
 
 @author Jonatan Tierno
 
 This example code is in the public domain.
 */

// Interrupt 0 (pin 3) reads the switch. HIGH means contact, closed box. LOW means open box.
int OPEN_BOX_INTERRUPT = 0;
// Interrupt 1 (pin 2) reads the switch. HIGH means contact, closed box. LOW means open box.
int BUTTON_INTERRUPT = 1;

// Stores the last 20 open events
int const STORE_LEN = 5;
long apertureDates[STORE_LEN];
int nextDatePointer=0;

// Total aperture count.
int apertureCount = 0;

long const DEBOUNCE_INTERVAL = 500;
long lastEvent = 0;
long lastButtonPress = 0;

boolean boxEvent = false;
boolean buttonEvent = false;

// the setup routine runs once when you press reset:
void setup() {     
  //DEBUG
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  Serial.println("Start");

  disconnectSerial();

  attachInterrupt(OPEN_BOX_INTERRUPT,boxOpened, FALLING);
  attachInterrupt(BUTTON_INTERRUPT,sendDataAndReset, RISING);
}

/**
 * Execute when the box opens. increment count and register time 
 */
void boxOpened(){
  // DEBOUNCE
  long currentTime = millis();
  if (currentTime-lastEvent < DEBOUNCE_INTERVAL)
  {
    return;
  }


  lastEvent = currentTime;

  apertureCount++;
  apertureDates[nextDatePointer] = currentTime;
  nextDatePointer++;
  if (nextDatePointer >= STORE_LEN)
  {
    nextDatePointer = 0;
  }

  boxEvent = true;


}


void connectSerial()
{

  /*if (!Serial){
   Serial.begin(9600);
   while (!Serial) {
   ; // wait for serial port to connect. Needed for Leonardo only
   }
   }*/
}

void disconnectSerial()
{
  // Serial.end();
}
/**
 * Execute when the button is pressed send data via serial and reset
 */
void sendDataAndReset(){
  // DEBOUNCE
  long currentTime = millis();
  if (currentTime-lastButtonPress < DEBOUNCE_INTERVAL)
  {
    return;
  }
  lastButtonPress = currentTime;

  buttonEvent = true;


}

void loop() {
  if (boxEvent)
  {
    boxEvent = false;

    //DEBUG
    connectSerial();

    Serial.println("OPENED BOX EVENT!!");

    disconnectSerial();

    delay(1000);

  }
  if (buttonEvent)
  {
    buttonEvent = false;
    
    long currentTime = millis();
  
    connectSerial();

    Serial.print("Box opened ");
    Serial.print(apertureCount);

    Serial.print(" times.\n\n");

    if (apertureCount> 0) {
      Serial.println("Last Events:");

      long tempDate; 
      int countPointer = STORE_LEN;
      if (apertureCount < STORE_LEN){
        countPointer = apertureCount;
      }

      int t;
      for(int i = 0; i< countPointer; i++)
      {
        t = nextDatePointer-1-i;
        if (t<0){
          t = t+ STORE_LEN;
        }
        tempDate = apertureDates[t];

        Serial.print(" - Opened ");
        Serial.print(currentTime - tempDate);
        Serial.print(" ms ago\n");
      }
      Serial.print("\n");
    }

    disconnectSerial();

    // Reset   
    apertureCount = 0;
    nextDatePointer = 0;   
  }
}

