#include <Arduino.h>
#include <RadioLib.h> // https://jgromes.github.io/RadioLib/class_s_x1262.html
#include <LgLoraBoard.h>

LgLoraBoard board;
volatile bool buttonClicked = false;

void onButtonClick()
{
  buttonClicked = true;
}

void buttonTask(void *pvParameters)
{
  for (;;)
  {
    if (buttonClicked)
    {
      buttonClicked = false;

      Serial.println(F("Button clicked, sending acknowledgment packet ..."));
      byte ackData[] = {0xAC, 0xDC}; // example acknowledgment data
      int errorState = board.transmitRadioBytes(sizeof(ackData), ackData);
      if (!errorState)
      {
        Serial.println(F("Acknowledgment packet sent successfully"));
      }
      else
      {
        Serial.println(F("Failed to send acknowledgment packet"));
      }
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void receiveTask(void *pvParameters)
{
  int packetSize = 0;
  int errorState = 0;

  for (;;)
  {
    packetSize = board.hasRadioPacket();
    if (packetSize)
    {
      byte receivedData[packetSize];
      errorState = board.readRadioBytes(packetSize, receivedData);
      if (!errorState)
      {
        Serial.print(F("Data: ["));
        for (int i = 0; i < packetSize; i++)
        {
          Serial.printf("0x%02X", receivedData[i]);
          if (i < packetSize - 1)
            Serial.print(F(", "));
        }
        Serial.println(F("]"));
        Serial.println(F("Blinking LED to indicate packet received, if supported by the board ..."));
        board.blinkLed();
        delay(100);
        board.disableLed();
      }
      else
      {
        Serial.println(F("Failed to read received data"));
      }
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void setup()
{
  Serial.begin(115200);
  Serial.println("LoRa Transceiver example. Receives packets and sends an acknowledgment.");

  board.setupRadioBoard();
  board.setupButton(onButtonClick);

  xTaskCreatePinnedToCore(
    buttonTask,    // Function name of the task
    "Button Task", // Name of the task (e.g. for debugging)
    2048,          // Stack size (bytes), minimum 1024 for OneButton library, but we give it more to be safe
    NULL,          // Parameter to pass
    1,             // Task priority
    NULL,          // Assign task handle
    0              // Run on core 0
  );

  xTaskCreatePinnedToCore(
    receiveTask,    // Function name of the task
    "Receive Task", // Name of the task (e.g. for debugging)
    4096,          // Stack size (bytes), we give it more to be safe
    NULL,          // Parameter to pass
    0,             // Task priority
    NULL,          // Assign task handle
    0              // Run on core 0
  );
}

void loop()
{
  // delete the loop task since we're using FreeRTOS tasks for button and receive handling
  vTaskDelete(NULL); // current task is not needed anymore or delete this line to keep the loop running

  // delay(2);
}
