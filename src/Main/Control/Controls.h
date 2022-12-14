#pragma once

#include <Basic/Mechanism.h>
#include <Hardware/HardwareManager.h>
#include <Drive/Drive.h>
#include <Shooter/Shooter.h>
#include <Hardware/IOMap.h>

class Controls : public Mechanism {
public:
    Controls(Drive* drive, Shooter* shooter);
    ~Controls();
    
    void process() override;
    void processInDisabled();
    void sendFeedback() override;

    bool getShouldPersistConfig();
    
private:
    ThunderGameController driveController {0};
    ThunderGameController auxController {1};
    ThunderGameController switchPanel {2};

    void doDrive();
    void doAux();
    void doSwitchPanel();

    bool whichCamera = false;

    bool driveTank = false;

    Drive* drive;
    Shooter* shooter;
};
