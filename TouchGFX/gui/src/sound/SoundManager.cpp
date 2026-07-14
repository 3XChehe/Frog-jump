#include <gui/sound/SoundManager.hpp>
#include <gui/sound/cash_sound.h>
#include <gui/sound/crash_sound.h>
#include <gui/sound/game_over_sound.h>
#include <gui/sound/jump_sound.h>
#include <gui/sound/sink_water_sound.h>

extern "C" {
#include "stm32f4xx_hal.h"
extern I2S_HandleTypeDef hi2s3;
}

void SoundManager::stopSound()
{
    HAL_I2S_DMAStop(&hi2s3);
}

void SoundManager::playJumpSound()
{
    HAL_I2S_DMAStop(&hi2s3);
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)jump_sound, (uint16_t)jump_sound_length);
}

void SoundManager::playCashSound()
{
    HAL_I2S_DMAStop(&hi2s3);
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)cash_sound, (uint16_t)cash_sound_length);
}

void SoundManager::playCrashSound()
{
    HAL_I2S_DMAStop(&hi2s3);
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)crash_sound, (uint16_t)crash_sound_length);
}

void SoundManager::playSinkWaterSound()
{
    HAL_I2S_DMAStop(&hi2s3);
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)sink_water_sound, (uint16_t)sink_water_sound_length);
}

void SoundManager::playGameOverSound()
{
    HAL_I2S_DMAStop(&hi2s3);
    HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t*)game_over_sound, (uint16_t)game_over_sound_length);
}

extern "C" void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s)
{
    if (hi2s->Instance == SPI3)
    {
        HAL_I2S_DMAStop(hi2s);
    }
}
