#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <portmacro.h>

class TaskBase {
  public:
    TaskHandle_t handle;
    ~TaskBase() {
#if INCLUDE_vTaskDelete
      vTaskDelete(handle);
#endif
      return;
    }
};

/**
class Task : public TaskBase {
  public:
    Task(char const* name, void (*taskfunc)(void *), int priority, int stackDepth = configMINIMAL_STACK_SIZE) {

      xTaskCreate(taskfunc, (signed char*)name, stackDepth, this, priority, &handle);

    }
};

**/

class TaskClass : public TaskBase {
  public:
    TaskClass(const char *name, int priority = 1, int stackDepth = configMINIMAL_STACK_SIZE) {
      xTaskCreate(taskfunc, name, stackDepth, this, priority, &handle);
    }
    virtual void task() = 0;
    static void taskfunc(void* param) {
      static_cast<TaskClass*>(param)->task();
    }
/**
#if INCLUDE_vTaskDelete
      vTaskDelete(handle);
#else
      while (1)
        xTaskDelay(10000);
#endif
    }
    **/
};
