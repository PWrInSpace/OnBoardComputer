#include "dataStructs.h"

bool DataFrame::allDevicesWokeUp(){
    return (pitot.wakeUp && mainValve.wakeUp && upustValve.wakeUp);
}