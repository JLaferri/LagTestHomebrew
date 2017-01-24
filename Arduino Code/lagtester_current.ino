#include <EEPROM.h>

//Configurable Properties
int ledPin = 18;
int buttonPin = 19;
int lightPin = 0;
int audioPin = 1;

int measurementCountToAverage = 3; //Amount of measurements to average before reporting result

int audioThreshold = 61; //Amount of DAC counts required to detect a positive audio signal
int audioLowThreshold = 5; //DAC count values below this number are considered as no audio signal detected
int lightCountsDifferenceThreshold = 25; //Amount of DAC counts between min and max light values to allow measurement to proceed
float lightRiseRatioRequired = 0.25; //Percent towards maximum light value before the light signal is accepted 
unsigned long measurementSignalOffset_us = 24440; //Average raw frame lag of a CRT. This is basically the amount of desync time between the audio signal and the light signal on the homebrew app
unsigned long calibrationMonitorCenterTime_us = 8333; //The desired measurement reading for the center screen measurement of the calibration monitor (CRT)

unsigned long periodMicros = 1000000; //The period of the audio and light signal in microseconds
int periodCountInitialization = 3; //Amount of periods to wait to get a good concept of the min and max light values

//Sequencing Properties
unsigned long period150Percent = periodMicros + (periodMicros / 2);
unsigned long period200Percent = 2 * periodMicros;
unsigned long period25Percent = periodMicros / 4;
unsigned long periodMicrosLess10Percent = periodMicros - (periodMicros / 10);
unsigned long periodMicrosPlus10Percent = periodMicros + (periodMicros / 10);
unsigned long timeoutMicros = (periodMicros / 2) - (periodMicros / 20);

byte digits[] =
{
  B11111111,
  B11111111,
  B11111111,
  B11111111
};

int currentDigit = 0;

boolean isMeasuring = false;
boolean isCalibration = false;
boolean isButtonPressed = false;
boolean audioDetected = false;

unsigned long measurementStartTime = 0;
unsigned long audioTime = 0;

unsigned long buttonPressTime = 0;

unsigned long buttonTimeForCal_ms = 5000;

int lightMax = 0;
int lightMin = 1023;
int lightThreshold = 0;

int measurementCount = 0;
long measurementAverage = 0;

void setup()
{
  //Set all of PortD pins as outputs
  DDRD = B11111111;
  
  //Set the first 4 bits of PortB as outputs
  DDRB = DDRB | B00001111;
  
  //Initialize Status LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
  
  //Initialize button pin
  pinMode(buttonPin, INPUT);
  
  //Load calibration value
  loadCalValue();
  
  //Initialize digits
  setDigits("0.000");
}

void loadCalValue()
{
  unsigned long value = (EEPROM.read(0) << 24) | EEPROM.read(1) << 16 | EEPROM.read(2) << 8 | EEPROM.read(3);
  
  //If calibration value is not set, set it to the default
  if (value == 0xFFFF)
  {
    setCalValue(measurementSignalOffset_us);
  }
  else
  {
    measurementSignalOffset_us = value; 
  }
}

void setCalValue(unsigned long value)
{
  EEPROM.write(0, byte(value >> 24));
  EEPROM.write(1, byte(value >> 16));
  EEPROM.write(2, byte(value >> 8));
  EEPROM.write(3, byte(value));
  
  measurementSignalOffset_us = value;
}

void turnOnStatusLed()
{
  digitalWrite(ledPin, HIGH);
}

void turnOffStatusLed()
{
  digitalWrite(ledPin, LOW);
}

byte getBitsFromChar(char c)
{
  switch(c)
  {
    case '0':
      return B11000000;
    case '1':
      return B11111001;
    case '2':
      return B10100100;
    case '3':
      return B10110000;
    case '4':
      return B10011001;
    case '5':
      return B10010010;
    case '6':
      return B10000010;
    case '7':
      return B11111000;
    case '8':
      return B10000000;
    case '9':
      return B10010000;
    case 'H':
      return B10001001;
    case 'h':
      return B10001011;
    case 'E':
      return B10000110;
    case 'r':
      return B10101111;
    case 'C':
      return B11000110;
    case 'A':
      return B10001000;
    case 'L':
      return B11000111;
    case '-':
      return B10111111;
    case '.':
      return B01111111;
    default:
      return B11111111;
  }
}

void setDigits(String str)
{
  int charCount = str.length();
  int count = 0, digCount = 0;
  
  while(count < charCount && digCount < 4)
  {
    char currentCharacter = str[count];
    byte bits = getBitsFromChar(currentCharacter);
    
    if (currentCharacter != '.' && (count + 1) < charCount && str[count + 1] == '.')
    {
      digits[digCount] = bits & B01111111;
      count++;
    }
    else
    {
      digits[digCount] = bits;
    }
    
    digCount++;
    count++; 
  }
}

void setDigits(float value)
{
  char buf[12]; // needs to be at least large enough to fit the formatted text
  dtostrf(value, 4, 4, buf);
  
  setDigits(String(buf));
}

void displayAlternatingDigit()
{
  //Enable current digit
  PORTB = (PORTB & B11110000) | (B0001 << currentDigit);
  
  //Turn on bits for current digit value
  PORTD = digits[currentDigit];
  
  currentDigit = (currentDigit + 1) % 4;
}

void displaySingleDigit(char digit, byte locations)
{
  //Enable digits specified by locations byte
  PORTB = (PORTB & B11110000) | (locations & B1111);
  
  //Set digit to passed digit
  PORTD = getBitsFromChar(digit);
}

void initializeMeasurement()
{
  isMeasuring = true;
  measurementStartTime = micros();
  
  audioTime = 0;
  audioDetected = false;
  
  measurementCount = 0;
  measurementAverage = 0;
  
  lightMax = 0;
  lightMin = 1023;

  //Measurement is starting, synchronize procedure by waiting for one entire period while polling the light sensor.
  //This will also keep track of the min and max light values for calculating percent of max brightness (75% default).
  //This is so that CRTs and LCDs have a more fair comparison because LCDs take time to reach full brightness.
  unsigned long timeToAccumulateLight = periodMicros * periodCountInitialization;
  while (getTimeSpanValue(measurementStartTime, micros()) < timeToAccumulateLight)
  {
    int lightCounts = analogRead(lightPin);
  
    if (lightCounts > lightMax) lightMax = lightCounts;
    if (lightCounts < lightMin) lightMin = lightCounts;
  }
  
  lightThreshold = (int)floor(lightMin + lightRiseRatioRequired * (lightMax - lightMin));
  
  //Wait until signal is low
  unsigned long audioTimeoutTime = timeToAccumulateLight + periodMicros;
  while (analogRead(audioPin) > audioLowThreshold)
  {
    //If time since measurement start is greater than 1 second, throw an error.
    if (getTimeSpanValue(measurementStartTime, micros()) > audioTimeoutTime)
    {
      setDigits("Err2");
      isMeasuring = false; //Return to displaying digits
    }
  }
  
  //If min and max light values are not different enough, throw error. This would mean light is saturated or no light is being detected
  if (isMeasuring && lightMax - lightMin < lightCountsDifferenceThreshold)
  {
    setDigits("Err3");
    isMeasuring = false; //Return to displaying digits
  }
}

unsigned long getTimeSpanValue(unsigned long past, unsigned long current)
{
    //Add overflow logic to this function
    return current - past;
}

void loop()
{
  if (!isMeasuring)
  {
    //Until measurement starts display different digits
    displayAlternatingDigit();
    delay(2);
      
    byte buttonResult = digitalRead(buttonPin);
    
    if (buttonResult == LOW && !isButtonPressed)
    {
      //Button press falling edge
      buttonPressTime = millis();
      isButtonPressed = true;
      
      //Turn off cal LED when button is pressed
      turnOffStatusLed();
      isCalibration = false;
    }
    else if (buttonResult == HIGH && isButtonPressed)
    {
       //Button was released, start measurement
       
       //Set display to a single digit
       displaySingleDigit('-', B1111);
       
       initializeMeasurement();
       
       isButtonPressed = false;
    }
    else if (buttonResult == LOW && isButtonPressed && !isCalibration && getTimeSpanValue(buttonPressTime, millis()) > buttonTimeForCal_ms)
    {
      //Button held down for cal time trigger
      isCalibration = true;
      setDigits("CAL ");
      
      //Turn on status LED when a cal measurement is triggered
      turnOnStatusLed();
    }
  }
  else
  {
    //Get time and time since last audio
    unsigned long time = micros();
    unsigned long timeDiff = getTimeSpanValue(audioTime, time);
      
    if (!audioDetected && timeDiff > periodMicrosLess10Percent)
    {
      int audioValue = analogRead(audioPin);
      
      //Until audio is detected, do nothing else
      if (audioValue > audioThreshold)
      {
        //Audio is detected, save time and start monitoring light
        audioDetected = true;
        audioTime = time;
      }
      else if (getTimeSpanValue(measurementStartTime, micros()) > (periodMicros * (periodCountInitialization + 1)) && timeDiff > periodMicrosPlus10Percent)
      {
        //Audio hasn't been detected in at least the period of the signal. This likely means the cable isn't plugged in. Show error
        setDigits("Err0");
        isMeasuring = false; //Return to displaying digits
      }
    }
    else if (audioDetected)
    {
      int lightValue = analogRead(lightPin);
      
      //Audio has been detected. Check for light or reset condition
      unsigned long timeSinceAudio = getTimeSpanValue(audioTime, time);
      
      if (lightValue > lightThreshold)
      {
        measurementAverage = (measurementCount * measurementAverage + timeSinceAudio) / (measurementCount + 1);
        measurementCount++;
        
        //If enough results have been averaged, show result and stop measurement
        if (measurementCount >= measurementCountToAverage)
        {
          //If this was a calibration measurement, write the new calibration value to EEPROM
          if (isCalibration)
          {
            unsigned long newCalValue = long(measurementAverage - calibrationMonitorCenterTime_us);
            setCalValue(newCalValue);
            
            setDigits(newCalValue / 1000.0);
          }
          else
          {
            setDigits((measurementAverage - measurementSignalOffset_us) / 1000.0); 
          }
          
          isMeasuring = false; //Return to displaying digits
        }
        
        audioDetected = false;
      }
      else if (timeSinceAudio > timeoutMicros)
      {
        setDigits("Err1");
        isMeasuring = false; //Return to displaying digits
      }
    }
  }
}
