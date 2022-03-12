#include "SDcard.h"
#include <string.h>

SDCard::SDCard(SPIClass &_spi, uint8_t _cs):
    spi(_spi),  cs(_cs), __mySD(SDFS(FSImplPtr(new VFSImpl()))){};

bool SDCard::init(){
    //spi = SPIClass(HSPI);
    //spi.begin(sck, miso, mosi, cs);
    spi.setClockDivider(SPI_CLOCK_DIV2);
    
    if(!SD.begin(cs, spi)){ //idk xD
        return false;
    }

    return true;
}

bool SDCard::write(String path, const String & dataFrame){
    File file = SD.open(path, "a");  
    if(file == 0x00){
      Serial.print("Open error: ");
      Serial.println(path);
      return false;
    }
    if(file) {
        if(!file.write((uint8_t *) dataFrame.c_str(), dataFrame.length())) {
            file.close();
            return false;
        }
    }else {
        return false;
    }
    
    file.close();

    return true;
}

bool SDCard::write(String path, char *dataFrame){
    File file = SD.open(path, "a");  
    if(!file){
      Serial.print("Open error: ");
      Serial.println(path);
      return false;
    }
    
    if(file) {
        if(!file.write((uint8_t *) dataFrame, strlen(dataFrame))) {
            file.close();
            return false;
        }
    }else {
        return false;
    }
    
    file.close();

    return true;
}

bool SDCard::fileExists(String path){
  return SD.exists(path);
}