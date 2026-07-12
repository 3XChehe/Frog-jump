/* ================================================================
 *  Screen2View.cpp — OBSTACLE SYSTEM (Frog-jump)
 *
 *  3 hệ thống chướng ngại vật (Happy Hopper style):
 *  ─────────────────────────────────────────────────────────────
 *  A) ĐƯỜNG (4 làn): xe cộ di chuyển → chạm = chết
 *  B) SÔNG  (3 làn): CHỈ CÓ KHÚC GỖ trôi → đứng trên = safe
 *                     rơi xuống nước (không trên gỗ) = chết
 *  C) ĐÍCH  (1 hàng): lá sen TĨNH = an toàn/đích đến
 *                      cá sấu TĨNH = chạm = chết
 *
 *  Vùng an toàn:
 *  — grass1 (Y=288): xuất phát
 *  — grass2 (Y=128): giữa đường và sông, KHÔNG có chướng ngại vật
 * ================================================================ */

#include <gui/screen2_screen/Screen2View.hpp>
#include <BitmapDatabase.hpp>
#include "main.h"

extern "C"
{
    uint32_t HAL_GetTick(void);
}

/* ================================================================
 *  BẢNG THÔNG SỐ CÁC LOẠI XE
 * ================================================================ */
const CarTypeDef Screen2View::carTypeDefs[NUM_CAR_TYPES] =
{
    { BITMAP_CAR_1_R_ID, BITMAP_CAR_1_L_ID, 48 },   // ô tô nhỏ
    { BITMAP_CAR_2_R_ID, BITMAP_CAR_2_L_ID, 64 },   // pickup
    { BITMAP_CAR_3_R_ID, BITMAP_CAR_3_L_ID, 80 },   // xe tải
    { BITMAP_CAR_5_R_ID, BITMAP_CAR_5_L_ID, 64 },   // SUV
};

/* ================================================================
 *  BẢNG THÔNG SỐ KHÚC GỖ (chỉ gỗ, KHÔNG có cá sấu trên sông)
 * ================================================================ */
const LogTypeDef Screen2View::logTypeDefs[NUM_LOG_TYPES] =
{
    { BITMAP_LOG_1_ID, 64 },    // gỗ nhỏ (64 × 32)
    { BITMAP_LOG_2_ID, 96 },    // gỗ lớn (96 × 32)
};

/* ================================================================
 *  BẢNG THÔNG SỐ VẬT THỂ Ở ĐÍCH (lá sen + cá sấu, TĨNH)
 *
 *  Người chơi phải giẫm vào lá sen mới qua được.
 *  Giẫm vào cá sấu → chết.
 * ================================================================ */
const FinishObjDef Screen2View::finishObjDefs[NUM_FINISH_TYPES] =
{
    { BITMAP_LOTUS_PAD_ID, 32,  true  },   // lá sen — AN TOÀN / ĐÍCH
    { BITMAP_CROCCODIE_ID, 64,  false },   // cá sấu — NGUY HIỂM
};

/* ================================================================
 *  CONSTRUCTOR
 * ================================================================ */
Screen2View::Screen2View()
    : tickCount(0)
    , rngState(0)
{
}

/* ================================================================
 *  SETUP SCREEN
 * ================================================================ */
void Screen2View::setupScreen()
{
    Screen2ViewBase::setupScreen();

    rngState = HAL_GetTick();
    if (rngState == 0) rngState = 42;

    /* Ẩn tất cả vật thể tĩnh từ Designer */
    hideDesignerCars();
    hideDesignerRiverObjs();

    /* Khởi tạo 3 hệ thống */
    initRoadObstacles();    // A) Đường
    initRiverLogs();        // B) Sông
    initFinishZone();       // C) Đích (lá sen + cá sấu tĩnh)

    /* Đảm bảo cat luôn nằm trên cùng (z-order cao nhất) */
    remove(cat);
    add(cat);

    /* Khởi tạo số tim (3 tim ban đầu) */
    lives = 3;
    heart.setVisible(true);
    heart.invalidate();
    heart2.setVisible(true);
    heart2.invalidate();
    heart3.setVisible(true);
    heart3.invalidate();
}

void Screen2View::tearDownScreen()
{
    Screen2ViewBase::tearDownScreen();
}

/* ================================================================
 *  GAME LOOP — mỗi frame
 * ================================================================ */
void Screen2View::handleTickEvent()
{
    tickCount++;

    /* Ô tô chạy ở tốc độ 1/3 (update mỗi 3 tick -> 20 fps) */
    if (tickCount % 3 == 0)
    {
        /* Cập nhật ĐƯỜNG (xe di chuyển) */
        for (int i = 0; i < NUM_ROAD_LANES; i++)
        {
            updateRoadLane(i);
        }
        renderCars();
    }

    /* Khúc gỗ chạy nhanh hơn ở tốc độ 1/2 (update mỗi 2 tick -> 30 fps) */
    if (tickCount % 2 == 0)
    {
        /* Cập nhật SÔNG (gỗ trôi) */
        for (int i = 0; i < NUM_RIVER_LANES; i++)
        {
            updateRiverLane(i);
        }
        renderLogs();

        /* Nếu cat đang đứng trên sông và trên gỗ -> cuốn theo gỗ trôi */
        int16_t catY = cat.getY();
        for (int i = 0; i < NUM_RIVER_LANES; i++)
        {
            if (catY == riverLanes[i].y)
            {
                int16_t catX = cat.getX();
                int16_t catW = cat.getWidth();
                bool onLog = false;

                for (int j = 0; j < MAX_LOGS_PER_RIVER; j++)
                {
                    if (!riverLanes[i].logs[j].active) continue;
                    int16_t logX = riverLanes[i].logs[j].x;
                    int16_t logW = logTypeDefs[riverLanes[i].logs[j].typeIdx].width;

                    /* Kiểm tra cat đứng trên khúc gỗ (dung sai 6px từ 2 bên mép) */
                    if (catX + catW - 6 > logX && catX + 6 < logX + logW)
                    {
                        onLog = true;
                        break;
                    }
                }

                if (onLog)
                {
                    int16_t newCatX = catX + (riverLanes[i].speed * riverLanes[i].dir);
                    /* Nếu bị cuốn trôi hẳn ra ngoài màn hình -> mất mạng như rơi xuống sông */
                    if (newCatX + catW < 0 || newCatX > SCREEN_W)
                    {
                        resetCatPositionAndLoseLife();
                        return;
                    }
                    if (newCatX != catX)
                    {
                        cat.invalidate();
                        cat.setXY(newCatX, catY);
                        cat.invalidate();
                    }
                }
                break;
            }
        }
    }

    /* ĐÍCH không cần update — vật thể TĨNH */

    /* Kiểm tra va chạm xe sau mỗi tick */
    checkCollisions();
}

/* ================================================================
 *                       P H Ầ N   Đ Ư Ờ N G
 * ================================================================ */

void Screen2View::hideDesignerCars()
{
    car_1_r.setVisible(false);  car_1_r.invalidate();
    car_1_l.setVisible(false);  car_1_l.invalidate();
    car_2_r.setVisible(false);  car_2_r.invalidate();
    car_2_l.setVisible(false);  car_2_l.invalidate();
    car_3_r.setVisible(false);  car_3_r.invalidate();
    car_3_l.setVisible(false);  car_3_l.invalidate();
    car_5_r.setVisible(false);  car_5_r.invalidate();
    car_5_l.setVisible(false);  car_5_l.invalidate();
}

void Screen2View::initRoadObstacles()
{
    static const int16_t laneY[NUM_ROAD_LANES] = { 256, 224, 192, 160 };
    int8_t firstDir = (nextRandom() % 2 == 0) ? 1 : -1;

    for (int i = 0; i < NUM_ROAD_LANES; i++)
    {
        RoadLane& lane = roadLanes[i];
        lane.y     = laneY[i];
        lane.dir   = (i % 2 == 0) ? firstDir : -firstDir;
        lane.speed = randRange(CAR_SPEED_MIN, CAR_SPEED_MAX);

        int16_t baseInterval = (int16_t)(90 / lane.speed);
        lane.spawnMin   = baseInterval;
        lane.spawnMax   = baseInterval * 3;
        lane.spawnTimer = randRange(5, 30);

        for (int j = 0; j < MAX_CARS_PER_LANE; j++)
        {
            lane.cars[j].active  = false;
            lane.cars[j].x       = 0;
            lane.cars[j].typeIdx = 0;
            carWidgets[i][j].setVisible(false);
            carWidgets[i][j].setXY(-100, lane.y);
            add(carWidgets[i][j]);
        }
    }

    /* Đặt 1-2 xe ban đầu */
    for (int i = 0; i < NUM_ROAD_LANES; i++)
    {
        RoadLane& lane = roadLanes[i];
        int numInit = randRange(1, 2);
        for (int c = 0; c < numInit && c < MAX_CARS_PER_LANE; c++)
        {
            uint8_t cType = (uint8_t)(nextRandom() % NUM_CAR_TYPES);
            int16_t cW    = carTypeDefs[cType].width;
            int16_t startX = randRange(10, SCREEN_W - cW - 10);

            bool canPlace = true;
            for (int k = 0; k < c; k++)
            {
                if (!lane.cars[k].active) continue;
                int16_t kW = carTypeDefs[lane.cars[k].typeIdx].width;
                int16_t kX = lane.cars[k].x;
                if (startX < kX + kW + MIN_CAR_GAP &&
                    startX + cW + MIN_CAR_GAP > kX)
                { canPlace = false; break; }
            }

            if (canPlace)
            {
                lane.cars[c].active  = true;
                lane.cars[c].x       = startX;
                lane.cars[c].typeIdx = cType;
                uint16_t bmpId = (lane.dir > 0)
                    ? carTypeDefs[cType].bmpLeft
                    : carTypeDefs[cType].bmpRight;
                carWidgets[i][c].setBitmap(touchgfx::Bitmap(bmpId));
                carWidgets[i][c].setWidth(cW);
                carWidgets[i][c].setHeight(OBJ_H);
                carWidgets[i][c].setXY(startX, lane.y);
                carWidgets[i][c].setVisible(true);
                carWidgets[i][c].invalidate();
            }
        }
    }
}

void Screen2View::updateRoadLane(int idx)
{
    RoadLane& lane = roadLanes[idx];
    for (int j = 0; j < MAX_CARS_PER_LANE; j++)
    {
        if (!lane.cars[j].active) continue;
        lane.cars[j].x += lane.speed * lane.dir;
        int16_t cW = carTypeDefs[lane.cars[j].typeIdx].width;

        if (lane.dir > 0 && lane.cars[j].x > SCREEN_W + 20)
        {
            lane.cars[j].active = false;
            carWidgets[idx][j].setVisible(false);
            carWidgets[idx][j].invalidate();
        }
        else if (lane.dir < 0 && lane.cars[j].x + cW < -20)
        {
            lane.cars[j].active = false;
            carWidgets[idx][j].setVisible(false);
            carWidgets[idx][j].invalidate();
        }
    }

    lane.spawnTimer--;
    if (lane.spawnTimer <= 0)
    {
        trySpawnCar(idx);
        lane.spawnTimer = randRange(lane.spawnMin, lane.spawnMax);
    }
}

void Screen2View::trySpawnCar(int idx)
{
    RoadLane& lane = roadLanes[idx];
    int freeSlot = -1;
    for (int j = 0; j < MAX_CARS_PER_LANE; j++)
    {
        if (!lane.cars[j].active) { freeSlot = j; break; }
    }
    if (freeSlot < 0) return;

    uint8_t cType = (uint8_t)(nextRandom() % NUM_CAR_TYPES);
    int16_t cW    = carTypeDefs[cType].width;
    int16_t spawnX = (lane.dir > 0) ? -cW : SCREEN_W;

    for (int j = 0; j < MAX_CARS_PER_LANE; j++)
    {
        if (!lane.cars[j].active) continue;
        int16_t eW = carTypeDefs[lane.cars[j].typeIdx].width;
        int16_t eX = lane.cars[j].x;
        int16_t gap = (lane.dir > 0)
            ? (eX - (spawnX + cW))
            : (spawnX - (eX + eW));
        if (gap < MIN_CAR_GAP) return;
    }

    lane.cars[freeSlot].active  = true;
    lane.cars[freeSlot].x       = spawnX;
    lane.cars[freeSlot].typeIdx = cType;
    uint16_t bmpId = (lane.dir > 0)
        ? carTypeDefs[cType].bmpLeft
        : carTypeDefs[cType].bmpRight;
    carWidgets[idx][freeSlot].setBitmap(touchgfx::Bitmap(bmpId));
    carWidgets[idx][freeSlot].setWidth(cW);
    carWidgets[idx][freeSlot].setHeight(OBJ_H);
    carWidgets[idx][freeSlot].setXY(spawnX, lane.y);
    carWidgets[idx][freeSlot].setVisible(true);
    carWidgets[idx][freeSlot].invalidate();
}

void Screen2View::renderCars()
{
    for (int i = 0; i < NUM_ROAD_LANES; i++)
    {
        for (int j = 0; j < MAX_CARS_PER_LANE; j++)
        {
            if (!roadLanes[i].cars[j].active) continue;
            touchgfx::Image& w = carWidgets[i][j];
            w.invalidate();
            w.setXY(roadLanes[i].cars[j].x, roadLanes[i].y);
            w.invalidate();
        }
    }
}

/* ================================================================
 *                       P H Ầ N   S Ô N G
 *  ─────────────────────────────────────────────────────────────
 *  • Gỗ trôi theo dòng nước (1 chiều/làn, tốc độ khác nhau)
 *  • Người chơi phải nhảy lên gỗ để băng sông
 *  • Rơi xuống nước (không trên gỗ) = chết
 *  • Khi đứng trên gỗ → trôi theo gỗ (cần code player sau)
 *
 *  Layout sông:
 *    Làn 0: Y=96  (river_1) — gần vùng an toàn giữa
 *    Làn 1: Y=64  (river_2) — giữa
 *    Làn 2: Y=32  (river_1) — gần đích
 * ================================================================ */

void Screen2View::hideDesignerRiverObjs()
{
    /* Ẩn vật thể tĩnh do Designer đặt */
    lotus_pad.setVisible(false);  lotus_pad.invalidate();
    crocodie.setVisible(false);   crocodie.invalidate();
    log_1.setVisible(false);      log_1.invalidate();
    log_2.setVisible(false);      log_2.invalidate();
}

void Screen2View::initRiverLogs()
{
    static const int16_t rlaneY[NUM_RIVER_LANES] = { 96, 64, 32 };
    int8_t firstDir = (nextRandom() % 2 == 0) ? 1 : -1;

    for (int i = 0; i < NUM_RIVER_LANES; i++)
    {
        RiverLane& rl = riverLanes[i];
        rl.y     = rlaneY[i];
        rl.dir   = (i % 2 == 0) ? firstDir : -firstDir;
        rl.speed = randRange(RIVER_SPEED_MIN, RIVER_SPEED_MAX);

        /* Sông spawn thường xuyên hơn đường vì
         * người chơi CẦN gỗ để bước lên */
        int16_t base = (int16_t)(60 / rl.speed);
        rl.spawnMin   = base;
        rl.spawnMax   = base * 2;
        rl.spawnTimer = randRange(5, 20);

        for (int j = 0; j < MAX_LOGS_PER_RIVER; j++)
        {
            rl.logs[j].active  = false;
            rl.logs[j].x       = 0;
            rl.logs[j].typeIdx = 0;
            logWidgets[i][j].setVisible(false);
            logWidgets[i][j].setXY(-100, rl.y);
            add(logWidgets[i][j]);
        }
    }

    /* Đặt 1-2 khúc gỗ ban đầu trên mỗi làn sông
     * để người chơi thấy ngay có đường đi */
    for (int i = 0; i < NUM_RIVER_LANES; i++)
    {
        RiverLane& rl = riverLanes[i];
        int numInit = randRange(1, 2);

        for (int c = 0; c < numInit && c < MAX_LOGS_PER_RIVER; c++)
        {
            uint8_t logType = (uint8_t)(nextRandom() % NUM_LOG_TYPES);
            int16_t oW = logTypeDefs[logType].width;
            int16_t startX = randRange(10, SCREEN_W - oW - 10);

            bool canPlace = true;
            for (int k = 0; k < c; k++)
            {
                if (!rl.logs[k].active) continue;
                int16_t kW = logTypeDefs[rl.logs[k].typeIdx].width;
                int16_t kX = rl.logs[k].x;
                if (startX < kX + kW + MIN_LOG_GAP &&
                    startX + oW + MIN_LOG_GAP > kX)
                { canPlace = false; break; }
            }

            if (canPlace)
            {
                rl.logs[c].active  = true;
                rl.logs[c].x       = startX;
                rl.logs[c].typeIdx = logType;

                logWidgets[i][c].setBitmap(
                    touchgfx::Bitmap(logTypeDefs[logType].bmpId));
                logWidgets[i][c].setWidth(oW);
                logWidgets[i][c].setHeight(OBJ_H);
                logWidgets[i][c].setXY(startX, rl.y);
                logWidgets[i][c].setVisible(true);
                logWidgets[i][c].invalidate();
            }
        }
    }
}

void Screen2View::updateRiverLane(int idx)
{
    RiverLane& rl = riverLanes[idx];

    for (int j = 0; j < MAX_LOGS_PER_RIVER; j++)
    {
        if (!rl.logs[j].active) continue;

        rl.logs[j].x += rl.speed * rl.dir;
        int16_t oW = logTypeDefs[rl.logs[j].typeIdx].width;

        if (rl.dir > 0 && rl.logs[j].x > SCREEN_W + 20)
        {
            rl.logs[j].active = false;
            logWidgets[idx][j].setVisible(false);
            logWidgets[idx][j].invalidate();
        }
        else if (rl.dir < 0 && rl.logs[j].x + oW < -20)
        {
            rl.logs[j].active = false;
            logWidgets[idx][j].setVisible(false);
            logWidgets[idx][j].invalidate();
        }
    }

    rl.spawnTimer--;
    if (rl.spawnTimer <= 0)
    {
        trySpawnLog(idx);
        rl.spawnTimer = randRange(rl.spawnMin, rl.spawnMax);
    }
}

void Screen2View::trySpawnLog(int idx)
{
    RiverLane& rl = riverLanes[idx];

    int freeSlot = -1;
    for (int j = 0; j < MAX_LOGS_PER_RIVER; j++)
    {
        if (!rl.logs[j].active) { freeSlot = j; break; }
    }
    if (freeSlot < 0) return;

    /* Random loại gỗ: 50% nhỏ, 50% lớn */
    uint8_t logType = (uint8_t)(nextRandom() % NUM_LOG_TYPES);
    int16_t oW = logTypeDefs[logType].width;

    int16_t spawnX = (rl.dir > 0) ? -oW : SCREEN_W;

    for (int j = 0; j < MAX_LOGS_PER_RIVER; j++)
    {
        if (!rl.logs[j].active) continue;
        int16_t eW = logTypeDefs[rl.logs[j].typeIdx].width;
        int16_t eX = rl.logs[j].x;
        int16_t gap = (rl.dir > 0)
            ? (eX - (spawnX + oW))
            : (spawnX - (eX + eW));
        if (gap < MIN_LOG_GAP) return;
    }

    rl.logs[freeSlot].active  = true;
    rl.logs[freeSlot].x       = spawnX;
    rl.logs[freeSlot].typeIdx = logType;

    logWidgets[idx][freeSlot].setBitmap(
        touchgfx::Bitmap(logTypeDefs[logType].bmpId));
    logWidgets[idx][freeSlot].setWidth(oW);
    logWidgets[idx][freeSlot].setHeight(OBJ_H);
    logWidgets[idx][freeSlot].setXY(spawnX, rl.y);
    logWidgets[idx][freeSlot].setVisible(true);
    logWidgets[idx][freeSlot].invalidate();
}

void Screen2View::renderLogs()
{
    for (int i = 0; i < NUM_RIVER_LANES; i++)
    {
        for (int j = 0; j < MAX_LOGS_PER_RIVER; j++)
        {
            if (!riverLanes[i].logs[j].active) continue;
            touchgfx::Image& w = logWidgets[i][j];
            w.invalidate();
            w.setXY(riverLanes[i].logs[j].x, riverLanes[i].y);
            w.invalidate();
        }
    }
}

/* ================================================================
 *             P H Ầ N   Đ Í C H   (FINISH ZONE)
 *
 *  Vùng đích ở Y=0 (grass3) — VẬT THỂ TĨNH, không di chuyển.
 *  ─────────────────────────────────────────────────────────────
 *  Giống Happy Hopper / Frogger cổ điển:
 *  • Lá sen (lotus_pad, 32×32): AN TOÀN — đích đến
 *  • Cá sấu (croccodie, 64×32): NGUY HIỂM — giẫm vào = chết
 *  • Người chơi phải nhắm vào lá sen để hoàn thành
 *
 *  Bố trí 4 vật thể ở đích:
 *  ─────────────────────────────────────────────────────────────
 *  Vị trí 0 (X=0):   LÁ SEN   — luôn an toàn (đảm bảo chơi được)
 *  Vị trí 1 (X=48):  RANDOM   — lá sen hoặc cá sấu
 *  Vị trí 2 (X=128): RANDOM   — lá sen hoặc cá sấu
 *  Vị trí 3 (X=208): LÁ SEN   — luôn an toàn (đảm bảo chơi được)
 *
 *  → Luôn có ít nhất 2 lá sen → game luôn có lời giải!
 * ================================================================ */
void Screen2View::initFinishZone()
{
    /* Ẩn hết tất cả vật thể ở vùng đích (bỏ lá sen, cá sấu) */
    for (int i = 0; i < NUM_FINISH_OBJS; i++)
    {
        finishSlots[i].active = false;
        finishWidgets[i].setVisible(false);
        finishWidgets[i].invalidate();
    }
}

/* ================================================================
 *  BỘ SINH SỐ NGẪU NHIÊN (xorshift32)
 * ================================================================ */
uint32_t Screen2View::nextRandom()
{
    rngState ^= rngState << 13;
    rngState ^= rngState >> 17;
    rngState ^= rngState << 5;
    return rngState;
}

int16_t Screen2View::randRange(int16_t lo, int16_t hi)
{
    if (lo >= hi) return lo;
    uint32_t range = (uint32_t)(hi - lo + 1);
    return lo + (int16_t)(nextRandom() % range);
}

/* ================================================================
 *  DI CHUYỂN CAT THEO SỰ KIỆN NÚT NHẤN (MỖI BƯỚC = 1 LÀN ĐƯỜNG)
 * ================================================================ */
void Screen2View::moveCat(uint16_t cmd)
{
    int16_t step = OBJ_H; /* 32 pixel — bằng chính xác độ rộng 1 làn đường */
    int16_t newX = cat.getX();
    int16_t newY = cat.getY();

    switch (cmd)
    {
    case CMD_CAT_LEFT:
        newX -= step;
        if (newX < 0)
        {
            newX = 0;
            if (newX >= cat.getX()) return; /* Không ra ngoài màn hình bên trái */
        }
        break;

    case CMD_CAT_RIGHT:
        newX += step;
        if (newX + cat.getWidth() > SCREEN_W)
        {
            newX = SCREEN_W - cat.getWidth();
            if (newX <= cat.getX()) return; /* Không ra ngoài màn hình bên phải */
        }
        break;

    case CMD_CAT_UP:
        newY -= step;
        if (newY < 0)
        {
            newY = 0;
            if (newY >= cat.getY()) return; /* Không ra ngoài màn hình phía trên */
        }
        break;

    case CMD_CAT_DOWN:
        newY += step;
        if (newY + cat.getHeight() > 320)
        {
            newY = 320 - cat.getHeight();
            if (newY <= cat.getY()) return; /* Không ra ngoài màn hình phía dưới */
        }
        break;

    default:
        return;
    }

    if (newX != cat.getX() || newY != cat.getY())
    {
        cat.invalidate();
        cat.setXY(newX, newY);
        cat.invalidate();

        /* Kiểm tra va chạm ngay sau bước nhảy */
        checkCollisions();
    }
}

/* ================================================================
 *  XỬ LÝ VA CHẠM & TIM
 * ================================================================ */
void Screen2View::resetCatPositionAndLoseLife()
{
    /* Đưa cat về vị trí xuất phát (104, 288) */
    cat.invalidate();
    cat.setXY(104, 288);
    cat.invalidate();

    /* Trừ đi 1 tim */
    lives--;
    if (lives == 2)
    {
        heart3.setVisible(false);
        heart3.invalidate();
    }
    else if (lives == 1)
    {
        heart2.setVisible(false);
        heart2.invalidate();
    }
    else if (lives <= 0)
    {
        heart.setVisible(false);
        heart.invalidate();

        /* Khi hết tim, tạm thời hồi lại 3 tim và cho phép chơi tiếp (hoặc reset game) */
        lives = 3;
        heart.setVisible(true);
        heart.invalidate();
        heart2.setVisible(true);
        heart2.invalidate();
        heart3.setVisible(true);
        heart3.invalidate();
    }
}

void Screen2View::checkCollisions()
{
    int16_t catX = cat.getX();
    int16_t catY = cat.getY();
    int16_t catW = cat.getWidth();
    int16_t margin = 4; /* dung sai 4px để va chạm chính xác, không gây ức chế */

    /* 0. Kiểm tra nếu mèo đã sang được bờ bên kia (Y = 0 / FINISH_Y) */
    if (catY == FINISH_Y)
    {
        /* Sang bờ bên kia thành công! Reset về vị trí ban đầu (104, 288) và KHÔNG mất tim */
        cat.invalidate();
        cat.setXY(104, 288);
        cat.invalidate();
        /* TODO: Cộng 1 điểm cho người chơi (hệ thống điểm sẽ được thiết kế sau) */
        return;
    }

    /* 1. Kiểm tra va chạm với xe trên ĐƯỜNG (khi cat đứng trên các làn Y = 160, 192, 224, 256) */
    for (int i = 0; i < NUM_ROAD_LANES; i++)
    {
        if (catY == roadLanes[i].y)
        {
            for (int j = 0; j < MAX_CARS_PER_LANE; j++)
            {
                if (!roadLanes[i].cars[j].active) continue;

                int16_t carX = roadLanes[i].cars[j].x;
                int16_t carW = carTypeDefs[roadLanes[i].cars[j].typeIdx].width;

                /* Nếu cat và xe giao nhau theo trục X */
                if (catX + margin < carX + carW && catX + catW - margin > carX)
                {
                    resetCatPositionAndLoseLife();
                    return;
                }
            }
            return; /* Đang ở trên đường bộ (và không đụng xe), an toàn */
        }
    }

    /* 2. Kiểm tra khi đứng trên SÔNG (khi cat đứng trên các làn Y = 32, 64, 96) */
    for (int i = 0; i < NUM_RIVER_LANES; i++)
    {
        if (catY == riverLanes[i].y)
        {
            bool onLog = false;
            for (int j = 0; j < MAX_LOGS_PER_RIVER; j++)
            {
                if (!riverLanes[i].logs[j].active) continue;

                int16_t logX = riverLanes[i].logs[j].x;
                int16_t logW = logTypeDefs[riverLanes[i].logs[j].typeIdx].width;

                /* Nếu cat có chân đứng trên khúc gỗ này (dung sai 6px từ 2 bên mép) */
                if (catX + catW - 6 > logX && catX + 6 < logX + logW)
                {
                    onLog = true;
                    break;
                }
            }

            /* Nếu dẫm lên sông mà KHÔNG có khúc gỗ ở dưới -> rớt xuống nước chết (như đâm xe) */
            if (!onLog)
            {
                resetCatPositionAndLoseLife();
                return;
            }
            return; /* Đang đứng an toàn trên gỗ */
        }
    }
}
