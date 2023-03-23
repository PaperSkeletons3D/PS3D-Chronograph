#include <Adafruit_GFX.h> //Library for using the I2C screen
#include <Adafruit_SSD1306.h> //Library for using the I2C screen

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET 4

#define irSensorPinOne 12 //change to any digital input pin on your arduino
#define irSensorPinTwo 11 //change to any digital input pin on your arduino

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int first_sensor; //First IR sensor state (HIGH or LOW)
int second_sensor; //Second IR sensor state (HIGH or LOW)
int dartCount = 0;
byte timerRunning;
float startTime;
float endTime;
float timeDifference;
float velocity;
float microSeconds;
float averageFPS = 0;
float firstAverageFPS = 0;

void setup() {
  pinMode(irSensorPinOne, INPUT_PULLUP);
  pinMode(irSensorPinTwo, INPUT_PULLUP);

  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }
  display.display();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.clearDisplay();     // Clear display
  display.setCursor(0, 0);    // Start at top-left corner
  display.display();
  display.clearDisplay();
}

void loop(){
  if (timerRunning == 0 && digitalRead(irSensorPinOne) == LOW) { // Start timer for FPS of dart
    startTime = millis();
    timerRunning = 1; //Sets timer state to HIGH. ALlow second loop to start to record dart passing second sensor.
  }
  if (timerRunning == 1 && digitalRead(irSensorPinTwo) == LOW) { // End timer for FPS of dart
    timerRunning = 0; //Sets timer state to LOW. Allowing the loop to end after second IR sensor has recorded the dart.
    endTime = millis();
    timeDifference = endTime - startTime; //t=the time it took the dart to cross both sensors.
    microSeconds = timeDifference / 1000; //converting milliseconds to microseconds.
    velocity = 3.3 / microSeconds; //velocity = distance (inches) / time
    velocity = velocity + 8.7; //converting from IPS (inches per seconds to feet)
    velocity = velocity / 10; //converting from micro seconds to seconds
    dartCount = dartCount + 1; //tracking the dart count.
    if (dartCount == 1) {
      averageFPS = velocity;
    }
    if (dartCount > 1) {
      averageFPS = (averageFPS + velocity) / 2;
    }
    printSerial(); //Calls the function to print serial outputs.
    printLCD(); //Calls the function to print display outputs.
  }
  if ((millis() - startTime) > 1000) { //If the second sensor does not detect anything after one second it will reset the timer. This allows the program to not be stuck in a loop without an end.
    timerRunning = 0;
  }

}

void printLCD() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  if (dartCount <= 9) {
    display.print("0");
  }
  display.print(dartCount);
  display.print(":");
  display.print(velocity, 1); //Print the number of darts recorded
  display.setCursor(0, 17);
  display.print("Av:");
  display.print(averageFPS, 1); //Print the average FPS of all recordings
  display.display();
}

void printSerial() {
  Serial.print ("Milliseconds: ");
  Serial.println (timeDifference, 2);
  Serial.print ("Seconds: ");
  Serial.println (microSeconds, 2);
  Serial.print ("Feet Per Second: ");
  Serial.println (velocity, 2);
  Serial.print ("Average Feet Per Second: ");
  Serial.println (averageFPS, 2);
  Serial.print ("Dart Count: ");
  Serial.println (dartCount);
  Serial.println (" ");
}
