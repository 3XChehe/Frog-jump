#ifndef SCREEN2VIEW_HPP
#define SCREEN2VIEW_HPP

#include <gui_generated/screen2_screen/Screen2ViewBase.hpp>
#include <gui/screen2_screen/Screen2Presenter.hpp>
#include <touchgfx/widgets/Image.hpp>

/* ================================================================
 *  HỆ THỐNG CHƯỚNG NGẠI VẬT — Frog-jump (Happy Hopper style)
 *
 *  Layout màn hình (từ dưới lên trên):
 *  ─────────────────────────────────────────────────────────────
 *  Y=288  grass1     XUẤT PHÁT — vùng an toàn
 *  Y=256  road1      Đường làn 0 — xe chạy
 *  Y=224  road2      Đường làn 1 — xe chạy
 *  Y=192  road1      Đường làn 2 — xe chạy
 *  Y=160  road2      Đường làn 3 — xe chạy
 *  Y=128  grass2     VÙNG AN TOÀN GIỮA — không chướng ngại vật
 *  Y=96   river_1    Sông làn 0 — CHỈ CÓ KHÚC GỖ trôi
 *  Y=64   river_2    Sông làn 1 — CHỈ CÓ KHÚC GỖ trôi
 *  Y=32   river_1    Sông làn 2 — CHỈ CÓ KHÚC GỖ trôi
 *  Y=0    grass3     ĐÍCH — lá sen (safe) + cá sấu (die)
 * ================================================================ */

/* ==================== HẰNG SỐ CHUNG ========================== */
static const int16_t SCREEN_W         = 240;
static const int16_t OBJ_H            = 32;

/* ==================== ĐƯỜNG — XE CỘ ========================== */
static const int16_t NUM_ROAD_LANES    = 4;
static const int16_t MAX_CARS_PER_LANE = 3;
static const int16_t MIN_CAR_GAP       = 56;
static const int16_t NUM_CAR_TYPES     = 4;
static const int16_t CAR_SPEED_MIN     = 1;
static const int16_t CAR_SPEED_MAX     = 3;

/* ==================== SÔNG — KHÚC GỖ ================= */
static const int16_t NUM_RIVER_LANES       = 3;
static const int16_t MAX_LOGS_PER_RIVER    = 3;
static const int16_t MIN_LOG_GAP           = 40;
static const int16_t NUM_LOG_TYPES         = 2;
static const int16_t RIVER_SPEED_MIN       = 1;
static const int16_t RIVER_SPEED_MAX       = 2;

/* ==================== ĐÍCH — LÁ SEN + CÁ SẤU (TĨNH) ========= */
static const int16_t NUM_FINISH_OBJS   = 4;   // 4 vật thể ở đích
static const int16_t NUM_FINISH_TYPES  = 2;   // lá sen hoặc cá sấu
static const int16_t FINISH_Y          = 0;   // Y tọa độ đích

/* ==================== CẤU TRÚC DỮ LIỆU ====================== */

/* --- Xe cộ --- */
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

/* --- Khúc gỗ trên sông --- */
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

/* --- Vật thể ở đích (lá sen / cá sấu) — TĨNH --- */
struct FinishObjDef
{
    uint16_t bmpId;
    int16_t  width;
    bool     safe;    // true = lá sen (an toàn/đích), false = cá sấu (chết)
};

struct FinishSlot
{
    bool    active;
    int16_t x;
    uint8_t typeIdx;
};

/* ================================================================ */

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

    /* ---- Pool widget Image ---- */
    touchgfx::Image carWidgets[NUM_ROAD_LANES][MAX_CARS_PER_LANE];      // 12
    touchgfx::Image logWidgets[NUM_RIVER_LANES][MAX_LOGS_PER_RIVER];    // 9
    touchgfx::Image finishWidgets[NUM_FINISH_OBJS];                     // 4
    /* Tổng: 25 Image widgets, cấp phát tĩnh */

    /* ---- Dữ liệu làn ---- */
    RoadLane   roadLanes[NUM_ROAD_LANES];
    RiverLane  riverLanes[NUM_RIVER_LANES];
    FinishSlot finishSlots[NUM_FINISH_OBJS];

    /* ---- Bảng thông số ---- */
    static const CarTypeDef   carTypeDefs[NUM_CAR_TYPES];
    static const LogTypeDef   logTypeDefs[NUM_LOG_TYPES];
    static const FinishObjDef finishObjDefs[NUM_FINISH_TYPES];

    /* ---- Trạng thái game ---- */
    uint32_t tickCount;
    uint32_t rngState;
    int16_t  lives;
    uint8_t  hasCash;

    /* ---- Xử lý va chạm & tim ---- */
    void checkCollisions();
    void resetCatPositionAndLoseLife();
    void spawnRandomCash();

    /* ---- ĐƯỜNG ---- */
    void initRoadObstacles();
    void updateRoadLane(int idx);
    void trySpawnCar(int idx);
    void renderCars();
    void hideDesignerCars();

    /* ---- SÔNG (chỉ gỗ) ---- */
    void initRiverLogs();
    void updateRiverLane(int idx);
    void trySpawnLog(int idx);
    void renderLogs();

    /* ---- ĐÍCH (lá sen + cá sấu, tĩnh) ---- */
    void initFinishZone();

    /* ---- Ẩn vật thể tĩnh của Designer ---- */
    void hideDesignerRiverObjs();

    /* ---- PRNG ---- */
    uint32_t nextRandom();
    int16_t  randRange(int16_t lo, int16_t hi);
};

#endif // SCREEN2VIEW_HPP
