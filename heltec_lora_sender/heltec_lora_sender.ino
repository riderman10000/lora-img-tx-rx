#include "LoRaWan_APP.h"
#include "Arduino.h"

#include "mem_image.h"

#define RF_FREQUENCY 433000000  // Hz



#define TX_OUTPUT_POWER 5  // dBm

#define LORA_BANDWIDTH 0         // [0: 125 kHz, \
                                 //  1: 250 kHz, \
                                 //  2: 500 kHz, \
                                 //  3: Reserved]
#define LORA_SPREADING_FACTOR 7  // [SF7..SF12]
#define LORA_CODINGRATE 1        // [1: 4/5, \
                                 //  2: 4/6, \
                                 //  3: 4/7, \
                                 //  4: 4/8]
#define LORA_PREAMBLE_LENGTH 8   // Same for Tx and Rx
#define LORA_SYMBOL_TIMEOUT 0    // Symbols
#define LORA_FIX_LENGTH_PAYLOAD_ON false
#define LORA_IQ_INVERSION_ON false


#define RX_TIMEOUT_VALUE 1000
#define BUFFER_SIZE 50  // Define the payload size here

byte image[64*64];
int image_number = 0;
char txpacket[BUFFER_SIZE];
char rxpacket[BUFFER_SIZE];
char row_info[BUFFER_SIZE];
//xx,xxx,xxx,xxx,xxx,xxx,xxx,xxx,xxx 
double txNumber;
int row_number = 0;

bool lora_idle = true;

static RadioEvents_t RadioEvents;
void OnTxDone(void);
void OnTxTimeout(void);

String test = String();

void setup() {
  image_number = random(0, 2);
  Serial.println(image_number);

  if (image_number == 0){
    for(int i = 0; i < (sizeof(image)/sizeof(byte)); i++ )
        image[i] = rocket[i];
  }
  else if (image_number == 1){
    for(int i = 0; i < (sizeof(image)/sizeof(byte)); i++ )
        image[i] = spider[i];
  }

  Serial.begin(115200);
  Mcu.begin();

  txNumber = 0;

  RadioEvents.TxDone = OnTxDone;
  RadioEvents.TxTimeout = OnTxTimeout;

  Radio.Init(&RadioEvents);
  Radio.SetChannel(RF_FREQUENCY);
  Radio.SetTxConfig(MODEM_LORA, TX_OUTPUT_POWER, 0, LORA_BANDWIDTH,
                    LORA_SPREADING_FACTOR, LORA_CODINGRATE,
                    LORA_PREAMBLE_LENGTH, LORA_FIX_LENGTH_PAYLOAD_ON,
                    true, 0, 0, LORA_IQ_INVERSION_ON, 3000);
}
int counter_image = 0;
void loop() {
  Serial.println(image_number);

  if (lora_idle == true) {
    delay(100);
    txNumber += 0.01;
    // sprintf(txpacket, "Hello world number %0.2f", txNumber);  //start a package

    // Serial.printf("\r\nsending packet \"%s\" , length %d\r\n", txpacket, strlen(txpacket));
    
    counter_image = counter_image % (64*8);
    test = test + "d" + counter_image ;
    for(int i=counter_image; i< counter_image + 8; ++i)
        // Serial.print(image[i]);
        test = test + ","+image[i];
    
    int length = test.length();
    test.toCharArray(row_info, length+2);
    sprintf(txpacket, row_info);

    Radio.Send((uint8_t *)txpacket, strlen(txpacket));  //send the package out
    lora_idle = false;

    // Serial.print(String()+counter_image + " X ");
    Serial.print(row_info);
    Serial.println(); 
    counter_image += 8;
    row_number++;
    test = String();
  }
  Radio.IrqProcess();
}

void OnTxDone(void) {
  Serial.println("TX done......");
  lora_idle = true;
}

void OnTxTimeout(void) {
  Radio.Sleep();
  Serial.println("TX Timeout......");
  lora_idle = true;
}