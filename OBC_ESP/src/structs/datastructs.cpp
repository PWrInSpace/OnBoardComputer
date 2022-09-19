#include "../include/structs/dataStructs.h"

static struct {
    PitotData       pitot;
    MainValveData   main_valve;
    TanWaData       tanwa;
    UpustValveData  upust_valve;
    RecoveryData    recovery;
    SlaveData       black_box;
    PayloadData     payload;
    MCB             mcb;
    uint32_t        mission_timer; //DRUT
    SemaphoreHandle_t mutex;
}data;

bool DF_init(void) {
    memset(&data.pitot, 0, sizeof(data.pitot));
    memset(&data.main_valve, 0, sizeof(data.main_valve));
    memset(&data.tanwa, 0, sizeof(data.tanwa));
    memset(&data.upust_valve, 0, sizeof(data.upust_valve));
    memset(&data.recovery, 0, sizeof(data.recovery));
    memset(&data.black_box, 0, sizeof(data.black_box));
    memset(&data.payload, 0, sizeof(data.payload));
    memset(&data.mcb, 0, sizeof(data.mcb));
    memset(&data.mission_timer, 0, sizeof(data.mission_timer));
    
    data.mutex = NULL;
    data.mutex = xSemaphoreCreateMutex();
    assert(data.mutex != NULL);
    if (data.mutex == NULL) {
        return false;
    }

    return true;
}

