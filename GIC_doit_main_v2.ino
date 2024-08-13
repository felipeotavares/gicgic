/*
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS
 *    CMD      MOSI
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK
 *    VSS      GND
 *    D0       MISO
 *    D1       -
 */
 //SD
#include "FS.h"
#include "SD.h"
#include "SPI.h"
//rtc_external
#include "RTClib.h"
#include <Wire.h>
#include "wiring_private.h"
//Display
#include "SSD1306Wire.h"  
//esp32 time
#include <ESP32Time.h>

//#define USE_MUTEX
int shared_variable = 0;
SemaphoreHandle_t shared_var_mutex = NULL;


// Define a task function
void Menu( void *pvParameters );

#define HSPI_MISO   16
#define HSPI_MOSI   13
#define HSPI_SCLK   14
#define HSPI_SS     15

File myFile;

static const int spiClk = 1000000; // 1 MHz

SPIClass * hspi = NULL;

SSD1306Wire display(0x3c, SDA, SCL); 
//SDA - D21
//SCL - D22

RTC_DS3231 rtc_external;
ESP32Time rtc;  // offset in seconds GMT+1

#define DEBOUNCE_TIME  20 // the debounce time in millisecond, increase this time if it still chatters

bool btn5_gate = true;
bool btn7_gate = true; 

String data_str;
String data_full;

bool cartao_removido = false;

bool verifica_btn5(){
  if(touchRead(T5)<80)
  {
    for (int i = 0; i < 5; i++) 
    {
      delay(DEBOUNCE_TIME);
      if(touchRead(T5)>80) 
      {
        btn5_gate = true;
        return false;
      }   
    }
    if (btn5_gate)
    {
      btn5_gate = false;
      return true;
    } 
    else return false;
  }
  btn5_gate = true;
  return false;
}

bool verifica_btn7(){
  if(touchRead(T7)<30)
  {
    for (int i = 0; i < 5; i++) 
    {
      delay(DEBOUNCE_TIME);
      if(touchRead(T7)>30) 
      {
        btn7_gate = true;
        return false;
      }   
    }
    if (btn7_gate)
    {
      btn7_gate = false;
      return true;
    } 
    else return false;
  }
  btn7_gate = true;
  return false;
}

char daysOfTheWeek[7][12] = {"Dom", "Seg", "Ter", "Qua", "Qui", "Sex", "Sab"};

void drawTopDisplay(String text) {
  display.setColor(BLACK);
  display.fillRect(0, 0, 112, 16);
  display.setColor(WHITE);
  display.setFont(ArialMT_Plain_10);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, text);
}

void drawBottomDisplay(String text, int line = 1){
  // draw the percentage as String
  display.setColor(BLACK);
  display.fillRect(0, line*16, 128, 16);
  display.setColor(WHITE);
  display.setFont(ArialMT_Plain_16);
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, line*16, text);
}

int alive_array_x[4] = {112,112,120,120};
int alive_array_y[4] = {0,8,8,0};
int alive_index = 0;
void drawSaveIcon(){
  // draw the percentage as String
  display.setColor(BLACK);
  display.fillRect(112, 0, 16, 16);
  display.setColor(WHITE);
  display.fillRect(alive_array_x[alive_index], alive_array_y[alive_index], 8, 8);
  alive_index += 1;
  if (alive_index>3) alive_index =0;
}

void listDir(fs::FS &fs, const char * dirname, uint8_t levels){
    Serial.printf("Listing directory: %s\n", dirname);

    File root = fs.open(dirname);
    if(!root){
        Serial.println("Failed to open directory");
        return;
    }
    if(!root.isDirectory()){
        Serial.println("Not a directory");
        return;
    }

    File file = root.openNextFile();
    while(file){
        if(file.isDirectory()){
            Serial.print("  DIR : ");
            Serial.println(file.name());
            if(levels){
                listDir(fs, file.path(), levels -1);
            }
        } else {
            Serial.print("  FILE: ");
            Serial.print(file.name());
            Serial.print("  SIZE: ");
            Serial.println(file.size());
        }
        file = root.openNextFile();
    }
}

void createDir(fs::FS &fs, const char * path){
    Serial.printf("Creating Dir: %s\n", path);
    if(fs.mkdir(path)){
        Serial.println("Dir created");
    } else {
        Serial.println("mkdir failed");
    }
}

void removeDir(fs::FS &fs, const char * path){
    Serial.printf("Removing Dir: %s\n", path);
    if(fs.rmdir(path)){
        Serial.println("Dir removed");
    } else {
        Serial.println("rmdir failed");
    }
}

void readFile(fs::FS &fs, const char * path){
    Serial.printf("Reading file: %s\n", path);

    File file = fs.open(path);
    if(!file){
        Serial.println("Failed to open file for reading");
        return;
    }

    Serial.print("Read from file: ");
    while(file.available()){
        Serial.write(file.read());
    }
    file.close();
}

void writeFile(fs::FS &fs, const char * path, const char * message){
    Serial.printf("Writing file: %s\n", path);

    File file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }
    if(file.print(message)){
        Serial.println("File written");
    } else {
        Serial.println("Write failed");
    }
    file.close();
}

void appendFile(fs::FS &fs, const char * path, const char * message){
    //Serial.printf("Appending to file: %s\n", path);

    File file = fs.open(path, FILE_APPEND);
    if(!file){
        //Serial.println("Failed to open file for appending");
        drawBottomDisplay("Falha ao abir",2); 
        drawBottomDisplay("arquivo",3); 
        return;
    }
    if(file.print(message)){
        drawBottomDisplay("Gravação SD-OK",2);
        drawSaveIcon(); 
        
    } else {
        //Serial.println("Erro de Gravação SD",2);
        drawBottomDisplay("Erro de Gravação SD",2); 
    }
    file.close();
}

void renameFile(fs::FS &fs, const char * path1, const char * path2){
    Serial.printf("Renaming file %s to %s\n", path1, path2);
    if (fs.rename(path1, path2)) {
        Serial.println("File renamed");
    } else {
        Serial.println("Rename failed");
    }
}

void deleteFile(fs::FS &fs, const char * path){
    Serial.printf("Deleting file: %s\n", path);
    if(fs.remove(path)){
        Serial.println("File deleted");
    } else {
        Serial.println("Delete failed");
    }
}

void testFileIO(fs::FS &fs, const char * path){
    File file = fs.open(path);
    static uint8_t buf[512];
    size_t len = 0;
    uint32_t start = millis();
    uint32_t end = start;
    if(file){
        len = file.size();
        size_t flen = len;
        start = millis();
        while(len){
            size_t toRead = len;
            if(toRead > 512){
                toRead = 512;
            }
            file.read(buf, toRead);
            len -= toRead;
        }
        end = millis() - start;
        Serial.printf("%u bytes read for %u ms\n", flen, end);
        file.close();
    } else {
        Serial.println("Failed to open file for reading");
    }


    file = fs.open(path, FILE_WRITE);
    if(!file){
        Serial.println("Failed to open file for writing");
        return;
    }

    size_t i;
    start = millis();
    for(i=0; i<2048; i++){
        file.write(buf, 512);
    }
    end = millis() - start;
    Serial.printf("%u bytes written for %u ms\n", 2048 * 512, end);
    file.close();
}

void saveData(const char * data){
  if(SD.exists("GIC.csv")){ // check the card is still there
    appendFile(SD, "\GIC.csv", data);
  }
  else{
    Serial.println("Error writing to file !");
  }
}

void Aquisicao(void *pvParameters){
  for(;;)
  {
    drawTopDisplay(rtc.getTime("%d-%m-%Y %H:%M:%S"));
    if(shared_var_mutex != NULL)
    {
      int analogValue_full = analogRead(2);

      for(int i=0;i<10;i++)
      {
        analogValue_full = analogValue_full+ analogRead(2);
      }

      int analogValue = int(analogValue_full/10);
      //for use external rtc
      //DateTime now = rtc_external.now();
      //data_str = now.timestamp() + ';' + String(analogValue) + '\n';
      //for use internal rtc
      data_str = rtc.getTime("%d-%m-%Y %H:%M:%S:") + rtc.getMillis() +';' + String(analogValue) + '\n';

      //Serial.println(data_str);
      Serial.print(data_str);   // (String) returns time with specified format 
      //drawTopDisplay(data_str);
      drawBottomDisplay(String(analogValue));
      display.display();
      
      //Serial.print("Memória livre: ");
      //Serial.print(ESP.getFreeHeap());
      //Serial.println(" bytes");
      //Serial.println("Esperando MUTEX...");
      while(!(xSemaphoreTake(shared_var_mutex, portMAX_DELAY) == pdTRUE)){delay(50);};
      data_full = data_full + data_str;
      xSemaphoreGive(shared_var_mutex);              
    }
    delay(495); // Allow other task to be scheduled
  } // Infinite loop
}

void Menu(void *pvParameters){
  for(;;)
  {
    if(verifica_btn5())
    {
      //Serial.println("btn 5 pressionado"); //Retirar o cartão - > take MUTEX
      drawBottomDisplay("btn 5 pressionado",1);
      if(shared_var_mutex != NULL)
      {
        //Serial.println("Esperando MUTEX...");
        drawBottomDisplay("Esperando MUTEX...",1);
        while(!(xSemaphoreTake(shared_var_mutex, portMAX_DELAY) == pdTRUE)){delay(200);};
        cartao_removido = true;
        xSemaphoreGive(shared_var_mutex);
        drawBottomDisplay("Retire o cartao e",2);
        drawBottomDisplay("aperte VERDE.",3);
        while(!verifica_btn5()){delay(200);};
        drawBottomDisplay("Insira cartão e",2);
        drawBottomDisplay("aperte VERDE.",3);
        while(!verifica_btn5()){delay(200);};
        drawBottomDisplay("Retomando ao",2);
        drawBottomDisplay("estado normal",3); 
        delay(5000);
        drawBottomDisplay("",2);
        drawBottomDisplay("",3); 

        cartao_removido = false;          
      }
    }
    if(verifica_btn7())
    {
      Serial.println("bnt 7");
    }        
    delay(10); // Allow other task to be scheduled
  } // Infinite loop
}

void DataToSD(void *pvParameters){

  for(;;)
  {
    if(shared_var_mutex != NULL)
    { // Sanity check if the mutex exists     // Try to take the mutex and wait indefintly if needed
      if(xSemaphoreTake(shared_var_mutex, portMAX_DELAY) == pdTRUE)
      {// Mutex successfully taken
        if(!cartao_removido)
        {
          appendFile(SD, "/gic.csv", data_full.c_str());
          Serial.println("dado transferido para SD...");
          data_full = "";
          DateTime now = rtc_external.now();

          rtc.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());  // 26 Set 2023 16:13:00
          drawBottomDisplay("Data update",3);
        }
      }
      xSemaphoreGive(shared_var_mutex);
      delay(10000);
    }
    delay(10); // Allow other task to be scheduled
  } // Infinite loop
}

void setup(){

  shared_var_mutex = xSemaphoreCreateMutex(); // Create the mutex
  
    hspi = new SPIClass(HSPI);

    hspi->begin(HSPI_SCLK, HSPI_MISO, HSPI_MOSI, HSPI_SS); //SCLK, MISO, MOSI, SS

    pinMode(hspi->pinSS(), OUTPUT); //HSPI SS

    Serial.begin(115200);
    Serial.println("Iniciando...");
    delay(1000);

    //rtc.setTime(0, 13, 16, 26, 9, 2023);  // 26 Set 2023 16:13:00

    if (!rtc_external.begin(&Wire)) {
    Serial.println("Couldn't find rtc_external");
    Serial.flush();
    //while (1) delay(10);
    }

    //To adjust the time uncomment the nest line. //ex: 08 de Março 21 de 2023 at 17:54:00 you would call:
    //rtc_external.adjust(DateTime(2023, 10, 5, 14, 4, 0));
    DateTime now = rtc_external.now();

    rtc.setTime(now.second(), now.minute(), now.hour(), now.day(), now.month(), now.year());  // 26 Set 2023 16:13:00
    delay(10);


    Serial.println(now.timestamp());

    Serial.println("Iniciando Display...");
    delay(1000);

    // Initialising the UI will init the display too.
    display.init();

    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_10);
    display.drawString(0, 10, "Display iniciado");
    display.display();

    analogReadResolution(12);

    if(!SD.begin(HSPI_SS, *hspi, spiClk)){
        Serial.println("Card Mount Failed");
        display.drawString(0, 10, "Erro no SD!");
        return;
    }
    delay(1000);
    uint8_t cardType = SD.cardType();
    

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);

    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));

    static int task_number0 = 0;
    xTaskCreatePinnedToCore(
      Menu
      ,  "Menu" // A name just for humans
      ,  5120          // The stack size
      ,  (void*)&task_number0 // Pass reference to a variable describing the task number
      //,  5  // High priority
      ,  1  // priority
      ,  NULL // Task handle is not used here - simply pass NULL
      ,1
    );
    static int task_number1 = 1;
    xTaskCreatePinnedToCore(
      DataToSD
      ,  "Menu" // A name just for humans
      ,  5120          // The stack size
      ,  (void*)&task_number1 // Pass reference to a variable describing the task number
      //,  5  // High priority
      ,  1  // priority
      ,  NULL // Task handle is not used here - simply pass NULL
      ,1
    );
    static int task_number2 = 2;
    xTaskCreatePinnedToCore(
      Aquisicao
      ,  "Verbose" // A name just for humans
      ,  5120          // The stack size
      ,  (void*)&task_number2 // Pass reference to a variable describing the task number
      //,  5  // High priority
      ,  5  // priority
      ,  NULL // Task handle is not used here - simply pass NULL
      ,0
      );
    /*static int task_number3 = 3;
    xTaskCreatePinnedToCore(
      updateDate
      ,  "UpdateDate" // A name just for humans
      ,  512          // The stack size
      ,  (void*)&task_number3 // Pass reference to a variable describing the task number
      //,  5  // High priority
      ,  1  // priority
      ,  NULL // Task handle is not used here - simply pass NULL
      ,1
      );*/
}


void loop(){

}
