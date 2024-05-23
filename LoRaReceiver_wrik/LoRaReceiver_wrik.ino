#include <SPI.h>
#include <LoRa.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

/* Uncomment the initialize the I2C address , uncomment only one, If you get a totally blank screen try the other*/
#define i2c_Address 0x3c  //initialize with the I2C addr 0x3C Typically eBay OLED's
//#define i2c_Address 0x3d //initialize with the I2C addr 0x3D Typically Adafruit OLED's

#define SCREEN_WIDTH 128  // OLED display width, in pixels
#define SCREEN_HEIGHT 64  // OLED display height, in pixels
#define OLED_RESET -1     //   QT-PY / XIAO
Adafruit_SH1106G display = Adafruit_SH1106G(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


#define ss 5
#define rst 32
#define dio0 33

int num_index = 0;
unsigned char image_info[64 * 64];
char char_array[50];

bool display_image = false;

void setup() {
  Serial.begin(9600);
  while (!Serial)
    ;
  Serial.println("LoRa Sender");

  delay(250);                        // wait for the OLED to power up
  display.begin(i2c_Address, true);  // Address 0x3C default

  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  LoRa.setPins(ss, rst, dio0);  //setup LoRa transceiver module

  while (!LoRa.begin(433E6))  //433E6 - Asia, 866E6 - Europe, 915E6 - North America
  {
    Serial.println(".");
    delay(500);
  }
  //   LoRa.setSyncWord(0xA5);
  Serial.println("LoRa Initializing OK!");
}

void loop() {
  // try to parse packet
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    String test = String();

    // read packet
    while (LoRa.available()) {
      test = test + (char)LoRa.read();
    }
    Serial.print(test);

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());

    Serial.println(test[0]);
    if (test[0] == 'd') {
      Serial.println("inside if condition");
      num_index = 1;
      String temp = String();
      do {
        temp += test[num_index];
        num_index = num_index + 1;
      } while (test.charAt(num_index) != ',');
      num_index += 1;
      int row_number = temp.toInt();
      // Serial.print(row_number);
      // Serial.println("inside the do while loop" );
      int j = 0;
      temp = String();
      for (int i = num_index; i < test.length() + 1; i++) {
        if (test[i] == ',') {
          image_info[row_number + j] = temp.toInt();
          Serial.print(image_info[row_number + j]);
          Serial.print("xx");
          j++;
          temp = String();
        } else {
          temp += test[i];
        }
      }
      image_info[row_number + j] = (unsigned int)temp.toInt();
      Serial.print(image_info[row_number + j]);
      Serial.print("x");
      Serial.print(row_number + j);
      Serial.print("x");
      if (row_number + j == (64*8 -1)) {
        display_image = true;
      }
    }
  }

  if (display_image) {
    // write code to display image
    display.clearDisplay();
    display.drawBitmap(0, 0,  image_info, 64, 64, 1);
    display.display();
    delay(1);
    display_image = false;
  }
}
