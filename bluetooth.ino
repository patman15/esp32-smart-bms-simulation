// define Daly Smart BMS UUIDs for bluetooth
#define SERVICE_UUID           "0000fff0-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_RX "0000fff1-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_TX "0000fff2-0000-1000-8000-00805f9b34fb"

// define bluetooth constants
#define BLE_DATA_CHUNK_SIZE 200

// variables
BLECharacteristic *pCharacteristicWrite;
BLECharacteristic *pCharacteristicRead;
bool deviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");

        for (int i = 0; i < rxValue.length(); i++) {
          if (rxValue[i]<0x10) {
            Serial.print("0");
          }
          Serial.print(rxValue[i], HEX);
          Serial.print(' ');
        }

        Serial.println();
      }  
    }
};

void bluetoothSetupServer() {
  // Create the BLE Device
  BLEDevice::init("DL-40D63C3223A2"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristicWrite = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_WRITE
                    );

  pCharacteristicWrite->setCallbacks(new MyCallbacks());

  pCharacteristicRead = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY
                                       );

  pCharacteristicRead->addDescriptor(new BLE2902());

  // Start the service
  pService->start();  

  // Start advertising
  pServer->getAdvertising()->addServiceUUID(BLEUUID(SERVICE_UUID));
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void bluetoothSendByteArray(byte *buffer, int dataLength) {
  pCharacteristicRead->setValue(buffer, dataLength); 
  pCharacteristicRead->notify();
}

bool bluetoothIsDeviceConnected() {
  return deviceConnected;
}