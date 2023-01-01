#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <SSD1306.h>

// OLED screen dimensions
const int OLED_WIDTH = 128;
const int OLED_HEIGHT = 64;

// OLED screen I2C address
const int OLED_ADDRESS = 0x3c;

// RFID reader SDA, SCK, and MOSI pins
const int RFID_SDA_PIN = D8;
const int RFID_SCK_PIN = D7;
const int RFID_MOSI_PIN = D5;

// RFID reader reset pin
const int RFID_RST_PIN = D6;

// Create an OLED screen object
SSD1306 screen(OLED_ADDRESS, D1, D2);

// Create an RFID reader object
MFRC522 rfid(RFID_SDA_PIN, RFID_SCK_PIN, RFID_MOSI_PIN, RFID_RST_PIN);

// Array to store RFID data
byte rfidData[4][4];

// Index of the current RFID card
int currentCard = 0;

// Flag to indicate if we are emulating an RFID card
bool emulating = false;

void setup() {
  // Initialize OLED screen
  screen.init();
  screen.flipScreenVertically();
  screen.setFont(ArialMT_Plain_10);

  // Initialize RFID reader
  rfid.PCD_Init();
}

void loop() {
  // Check if we are emulating an RFID card
  if (emulating) {
    // Emulate the current RFID card
    rfid.PCD_StopCrypto1();
    MFRC522::StatusCode status = rfid.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, 1, &rfidData[currentCard][0], &rfidData[currentCard][1]);
if (status != MFRC522::STATUS_OK) {
// If there was an error, stop emulating and display an error message on the OLED screen
emulating = false;
screen.clear();
screen.drawString(0, 0, "Error emulating");
screen.display();
}
}
else {
// Check for a new RFID card
if (rfid.PICC_IsNewCardPresent()) {
// Select the new RFID card
MFRC522::PICC_Type piccType = rfid.PICC_ReadCardSerial();
if (piccType != MFRC522::PICC_TYPE_UNKNOWN) {
// Read the RFID data
rfid.PCD_ReadMemoryBlock(1, 4, &rfidData[currentCard][0]);
// Increment the current card index
currentCard = (currentCard + 1) % 4;
// Display the RFID data on the OLED screen
displayData();
}
}
// Check if the button was pressed
if (digitalRead(D3) == LOW) {
  // Toggle the emulation flag
  emulating = !emulating;
  // Display the current state on the OLED screen
  displayData();
  // Wait for the button to be released
  while (digitalRead(D3) == LOW);
}
