#ifndef STATE_CHANGER_H
#define STATE_CHANGER_H

class StateChanger {

public:

    void going2abort();
    void idle2fueling();
    void fueling2countdown();
    void countdown2flight();
    void flight2firstSepar();
    void firstSep2secSep();
};

#endif