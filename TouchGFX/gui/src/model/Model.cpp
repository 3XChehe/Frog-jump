#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>
#include "main.h"
#include "cmsis_os2.h"

extern "C" osMessageQueueId_t myCommandQueueHandle;

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
    if (myCommandQueueHandle != 0 && modelListener != 0)
    {
        uint16_t cmd = 0;
        if (osMessageQueueGet(myCommandQueueHandle, &cmd, 0, 0) == osOK)
        {
            modelListener->onCommandReceived(cmd);
        }
    }
}
