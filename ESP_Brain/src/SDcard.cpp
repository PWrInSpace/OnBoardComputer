#include "SDcard.h"

bool SD_write(const String & path, const String & dataFrame){
    File file = SD.open(path, "a");  
    
    if(file){
        if(!file.write((uint8_t *) dataFrame.c_str(), dataFrame.length())) 
            // Inkrementacja ilości błędów SD TODO.
            return false;
        }else{
            // Inkrementacja ilości błędów SD TODO.
            return false;
        }
    
    file.close();

    return true;
}