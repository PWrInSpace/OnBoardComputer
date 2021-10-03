#include "SDcard.h"

extern MainDataFrame mainDataFrame;

bool SD_write(const String & path, const String & dataFrame){
    
    File file = SD.open(path, "a");  
    
    if(file) {

        if(!file.write((uint8_t *) dataFrame.c_str(), dataFrame.length())) {
            
            mainDataFrame.sdErrorCounter++;
            return false;
        }
        
    }else {
            
        mainDataFrame.sdErrorCounter++;
        return false;
    }
    
    file.close();

    return true;
}