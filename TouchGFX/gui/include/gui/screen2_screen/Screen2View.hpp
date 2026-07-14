#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <touchgfx/widgets/Image.hpp>

// Common constants
static const int16_t SCREEN_W         = 240;
static const int16_t OBJ_H            = 32;

// Road and car constant
static const int16_t NUM_ROAD_LANES    = 4;
static const int16_t MAX_CARS_PER_LANE = 3;
static const int16_t MIN_CAR_GAP       = 56;
static const int16_t NUM_CAR_TYPES     = 4;
static const int16_t CAR_SPEED_MIN     = 1;
static const int16_t CAR_SPEED_MAX     = 3;

// River and log constant
static const int16_t NUM_RIVER_LANES       = 3;
static const int16_t MAX_LOGS_PER_RIVER    = 3;
static const int16_t MIN_LOG_GAP           = 40;
static const int16_t NUM_LOG_TYPES         = 2;
static const int16_t RIVER_SPEED_MIN       = 1;
static const int16_t RIVER_SPEED_MAX       = 2;

// Finish side constant
static const int16_t NUM_FINISH_OBJS   = 4;
static const int16_t NUM_FINISH_TYPES  = 2;
static const int16_t FINISH_Y          = 0;   // Y cordinate of finish side

// COMMON STRUCT

// Car
struct CarTypeDef
{
    uint16_t bmpRight;
    uint16_t bmpLeft;
    int16_t  width;
};

struct CarSlot
{
    bool    active;
    int16_t x;
    uint8_t typeIdx;
};

struct RoadLane
{
    int8_t  dir;
    int16_t speed;
    int16_t y;
    CarSlot cars[MAX_CARS_PER_LANE];
    int16_t spawnTimer;
    int16_t spawnMin;
    int16_t spawnMax;
};

// Wood log
struct LogTypeDef
{
    uint16_t bmpId;
    int16_t  width;
};

struct LogSlot
{
    bool    active;
    int16_t x;
    uint8_t typeIdx;
};

struct RiverLane
{
    int8_t  dir;
    int16_t speed;
    int16_t y;
    LogSlot logs[MAX_LOGS_PER_RIVER];
    int16_t spawnTimer;
    int16_t spawnMin;
    int16_t spawnMax;
};

// Finish side object
struct FinishObjDef
{
    uint16_t bmpId;
    int16_t  width;
    bool     safe;    // true = leaf, false = crocodile
};

struct FinishSlot
{
    bool    active;
    int16_t x;
    uint8_t typeIdx;
};

// Screen 2 view class

class Screen2View : public Screen2ViewBase
{
public:
    Screen2View();
    virtual ~Screen2View() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void handleTickEvent();
    void moveCat(uint16_t cmd);
    void updateScore(uint32_t currentScore);

protected:
    touchgfx::Unicode::UnicodeChar scoreBuffer[16];

    // Pool widget Image
    touchgfx::Image carWidgets[NUM_ROAD_LANES][MAX_CARS_PER_LANE];      // 12
    touchgfx::Image logWidgets[NUM_RIVER_LANES][MAX_LOGS_PER_RIVER];    // 9
    touchgfx::Image finishWidgets[NUM_FINISH_OBJS];                     // 4

    // Lane data
    RoadLane   roadLanes[NUM_ROAD_LANES];
    RiverLane  riverLanes[NUM_RIVER_LANES];
    FinishSlot finishSlots[NUM_FINISH_OBJS];

    // Object type list
    static const CarTypeDef   carTypeDefs[NUM_CAR_TYPES];
    static const LogTypeDef   logTypeDefs[NUM_LOG_TYPES];
    static const FinishObjDef finishObjDefs[NUM_FINISH_TYPES];

    // Game status
    uint32_t tickCount;
    uint32_t rngState;
    int16_t  lives;
    uint8_t  hasCash;

    // Check collision and handle life for character
    void checkCollisions();
    void resetCatPositionAndLoseLife();
    void spawnRandomCash();

    // Lane's function
    void initRoadObstacles();
    void updateRoadLane(int idx);
    void trySpawnCar(int idx);
    void renderCars();
    void hideDesignerCars();

    // Wood log's function
    void initRiverLogs();
    void updateRiverLane(int idx);
    void trySpawnLog(int idx);
    void renderLogs();

    // Finish zone's function
    void initFinishZone();

    // Hide all object (init)
    void hideDesignerRiverObjs();

    // Random generator
    uint32_t nextRandom();
    int16_t  randRange(int16_t lo, int16_t hi);
};

#endif // SCREEN2VIEW_HPP
