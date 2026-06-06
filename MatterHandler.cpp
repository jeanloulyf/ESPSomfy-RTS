#include "MatterHandler.h"

#if defined(CONFIG_IDF_TARGET_ESP32C6) || defined(CONFIG_IDF_TARGET_ESP32S3) || defined(CONFIG_IDF_TARGET_ESP32C3)

extern SomfyShadeController somfy;

MatterHandler matterHandler;

// We will keep an array of pointers to the matter endpoints
MatterWindowCovering* matterShades[SOMFY_MAX_SHADES] = {nullptr};

// Helper macro to generate callbacks for each shade
#define MAKE_CALLBACKS(id) \
    bool onMatterOpen_##id() { \
        SomfyShade* shade = somfy.getShadeById(id); \
        if(shade) { \
            shade->moveToTarget(0.0f); \
            if(matterShades[id]) { \
                matterShades[id]->setLiftPercentage(0); \
                matterShades[id]->setOperationalState(MatterWindowCovering::LIFT, MatterWindowCovering::STALL); \
            } \
        } \
        return true; \
    } \
    bool onMatterClose_##id() { \
        SomfyShade* shade = somfy.getShadeById(id); \
        if(shade) { \
            shade->moveToTarget(100.0f); \
            if(matterShades[id]) { \
                matterShades[id]->setLiftPercentage(100); \
                matterShades[id]->setOperationalState(MatterWindowCovering::LIFT, MatterWindowCovering::STALL); \
            } \
        } \
        return true; \
    } \
    bool onMatterStop_##id() { \
        SomfyShade* shade = somfy.getShadeById(id); \
        if(shade) { \
            shade->sendCommand(somfy_commands::My); \
            if(matterShades[id]) { \
                matterShades[id]->setLiftPercentage(shade->currentPos); \
                matterShades[id]->setOperationalState(MatterWindowCovering::LIFT, MatterWindowCovering::STALL); \
            } \
        } \
        return true; \
    } \
    bool onMatterGoToLift_##id(uint8_t liftPercent) { \
        SomfyShade* shade = somfy.getShadeById(id); \
        if(shade) { \
            shade->moveToTarget((float)liftPercent); \
            if(matterShades[id]) { \
                matterShades[id]->setLiftPercentage(liftPercent); \
                matterShades[id]->setOperationalState(MatterWindowCovering::LIFT, MatterWindowCovering::STALL); \
            } \
        } \
        return true; \
    }

// Generate callbacks for first 32 shades
MAKE_CALLBACKS(0) MAKE_CALLBACKS(1) MAKE_CALLBACKS(2) MAKE_CALLBACKS(3)
MAKE_CALLBACKS(4) MAKE_CALLBACKS(5) MAKE_CALLBACKS(6) MAKE_CALLBACKS(7)
MAKE_CALLBACKS(8) MAKE_CALLBACKS(9) MAKE_CALLBACKS(10) MAKE_CALLBACKS(11)
MAKE_CALLBACKS(12) MAKE_CALLBACKS(13) MAKE_CALLBACKS(14) MAKE_CALLBACKS(15)
MAKE_CALLBACKS(16) MAKE_CALLBACKS(17) MAKE_CALLBACKS(18) MAKE_CALLBACKS(19)
MAKE_CALLBACKS(20) MAKE_CALLBACKS(21) MAKE_CALLBACKS(22) MAKE_CALLBACKS(23)
MAKE_CALLBACKS(24) MAKE_CALLBACKS(25) MAKE_CALLBACKS(26) MAKE_CALLBACKS(27)
MAKE_CALLBACKS(28) MAKE_CALLBACKS(29) MAKE_CALLBACKS(30) MAKE_CALLBACKS(31)

typedef bool (*OpenCloseStopCB)();
typedef bool (*LiftCB)(uint8_t);

OpenCloseStopCB openCBs[32] = {
    onMatterOpen_0, onMatterOpen_1, onMatterOpen_2, onMatterOpen_3,
    onMatterOpen_4, onMatterOpen_5, onMatterOpen_6, onMatterOpen_7,
    onMatterOpen_8, onMatterOpen_9, onMatterOpen_10, onMatterOpen_11,
    onMatterOpen_12, onMatterOpen_13, onMatterOpen_14, onMatterOpen_15,
    onMatterOpen_16, onMatterOpen_17, onMatterOpen_18, onMatterOpen_19,
    onMatterOpen_20, onMatterOpen_21, onMatterOpen_22, onMatterOpen_23,
    onMatterOpen_24, onMatterOpen_25, onMatterOpen_26, onMatterOpen_27,
    onMatterOpen_28, onMatterOpen_29, onMatterOpen_30, onMatterOpen_31
};

OpenCloseStopCB closeCBs[32] = {
    onMatterClose_0, onMatterClose_1, onMatterClose_2, onMatterClose_3,
    onMatterClose_4, onMatterClose_5, onMatterClose_6, onMatterClose_7,
    onMatterClose_8, onMatterClose_9, onMatterClose_10, onMatterClose_11,
    onMatterClose_12, onMatterClose_13, onMatterClose_14, onMatterClose_15,
    onMatterClose_16, onMatterClose_17, onMatterClose_18, onMatterClose_19,
    onMatterClose_20, onMatterClose_21, onMatterClose_22, onMatterClose_23,
    onMatterClose_24, onMatterClose_25, onMatterClose_26, onMatterClose_27,
    onMatterClose_28, onMatterClose_29, onMatterClose_30, onMatterClose_31
};

OpenCloseStopCB stopCBs[32] = {
    onMatterStop_0, onMatterStop_1, onMatterStop_2, onMatterStop_3,
    onMatterStop_4, onMatterStop_5, onMatterStop_6, onMatterStop_7,
    onMatterStop_8, onMatterStop_9, onMatterStop_10, onMatterStop_11,
    onMatterStop_12, onMatterStop_13, onMatterStop_14, onMatterStop_15,
    onMatterStop_16, onMatterStop_17, onMatterStop_18, onMatterStop_19,
    onMatterStop_20, onMatterStop_21, onMatterStop_22, onMatterStop_23,
    onMatterStop_24, onMatterStop_25, onMatterStop_26, onMatterStop_27,
    onMatterStop_28, onMatterStop_29, onMatterStop_30, onMatterStop_31
};

LiftCB liftCBs[32] = {
    onMatterGoToLift_0, onMatterGoToLift_1, onMatterGoToLift_2, onMatterGoToLift_3,
    onMatterGoToLift_4, onMatterGoToLift_5, onMatterGoToLift_6, onMatterGoToLift_7,
    onMatterGoToLift_8, onMatterGoToLift_9, onMatterGoToLift_10, onMatterGoToLift_11,
    onMatterGoToLift_12, onMatterGoToLift_13, onMatterGoToLift_14, onMatterGoToLift_15,
    onMatterGoToLift_16, onMatterGoToLift_17, onMatterGoToLift_18, onMatterGoToLift_19,
    onMatterGoToLift_20, onMatterGoToLift_21, onMatterGoToLift_22, onMatterGoToLift_23,
    onMatterGoToLift_24, onMatterGoToLift_25, onMatterGoToLift_26, onMatterGoToLift_27,
    onMatterGoToLift_28, onMatterGoToLift_29, onMatterGoToLift_30, onMatterGoToLift_31
};

void MatterHandler::begin() {
    if(initialized) return;
    
    Serial.println("Initializing Matter Integration...");
    
    bool hasShades = false;
    // Create an endpoint for each active shade
    for(uint8_t i=0; i<SOMFY_MAX_SHADES; i++) {
        SomfyShade* shade = &somfy.shades[i];
        if(shade->getShadeId() != 255 && strlen(shade->name) > 0) {
            matterShades[i] = new MatterWindowCovering();
            
            // Assume ROLLERSHADE by default
            matterShades[i]->begin(shade->currentPos, 0, MatterWindowCovering::ROLLERSHADE);
            
            matterShades[i]->onOpen(openCBs[i]);
            matterShades[i]->onClose(closeCBs[i]);
            matterShades[i]->onStop(stopCBs[i]);
            matterShades[i]->onGoToLiftPercentage(liftCBs[i]);

            hasShades = true;
        }
    }
    
    if (hasShades) {
        Matter.begin();
    }
    initialized = true;
}

void MatterHandler::loop() {
    if(!initialized) return;
    
    // Sync physical shade state to Matter state
    for(uint8_t i=0; i<SOMFY_MAX_SHADES; i++) {
        SomfyShade* shade = &somfy.shades[i];
        if(shade->getShadeId() != 255 && matterShades[i]) {
            uint8_t matterPos = matterShades[i]->getLiftPercentage();
            uint8_t realPos = (uint8_t)shade->currentPos;
            
            // Only update if there is a discrepancy and the motor is idle
            if(shade->isIdle() && matterPos != realPos) {
                matterShades[i]->setLiftPercentage(realPos);
                matterShades[i]->setOperationalState(MatterWindowCovering::LIFT, MatterWindowCovering::STALL);
            }
        }
    }
}

#endif // CONFIG_IDF_TARGET_ESP32C6
