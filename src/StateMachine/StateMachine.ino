#include <Arduino_FreeRTOS.h>
#include <queue.h>

/**
 * StateMachine class
 * 
 * Currently holds business logic associated with my test case (LED blinking)
 * Ideally we would refactor this so that StateMachine class only has state machine plumbing.
 * Business logic would be put in a subclass (UserActiveClass)
 * 
 * TaskClass -> StateMachine -> UserActiveClass
 * 
 * The user will write potentially many of thses UserActiveClasses to create a collaboration to
 * implement user requirements
 */
class StateMachine : public TaskClass {

  private:
    enum State {creation, led_off, led_on};  // needs to be moved subclass
    State currentState; // generic, common to all StateMachines (private member of StateMachine)

    /**
       State procedures
       This is where all the work is done
       No StateMachine plumbing here -- it's all "business logic"
       Shoule be refactored to the UserActiveClass
    */
    void sp_creation() {
    }
    void sp_led_off() {
      Serial.println("Led off");
      digitalWrite(id + 2, LOW);
    }
    void sp_led_on() {
      Serial.println("Led on");
      digitalWrite(id + 2, HIGH);
    }

    typedef void (StateMachine::*StateProcedurePtr)();  // magic to allow the instance methods to be called using pointers

    /**
       Valid signals that can be sent
    */

    // Signals
    enum Signal {ON, OFF};    // should be UserActiveClass -- should be visible to app code to facilitate signal generation

    // State transitions table

    // State stt[sizeof(State)][sizeof(Signal)] = {   // cannot get member count for enum
    const State stt[3][2] = {        // how do we make this a static member of the UserActiveClass??
      {NULL,  led_off},
      {led_on, NULL},
      {NULL, led_off}
    };

    // lookup table for the state procedures simplyfying(??) the signal processing loop
    // how do we make this table a static member of the UserActiveClass (don't need a table in every instance
     
    const StateProcedurePtr stateProc[3] = {&StateMachine::sp_creation, &StateMachine::sp_led_off, &StateMachine::sp_led_on};

  protected:
    int id;   // UserActiveClass stuff, not generic
  public:

    /**
     * Constructor
     */
    StateMachine(const char * name, int i) : TaskClass(name) {
      byte off = 0x01; // xQueueSend passes this as (void *) -- why?
      id = i;   // not required by generic state machine
      currentState = creation;    // generic
      eventQ = xQueueCreate(4, 1);// generic  -- Q_length, message_size -- override in UserActiveClass?
      xQueueSend(eventQ, (void *) &off,  portMAX_DELAY); // should be sent by 
      pinMode(id + 2, OUTPUT); // business logic -- UserActiveClass initialisation code
    }

    // the event queue -- generic
    QueueHandle_t eventQ;  

    // active class event processing loop
    // we should be able to make this a generic procedure
    // (i.e. a protected member method of class StateMachine
    
    void task() {
      byte sig;
      for (;;) {
        if (xQueueReceive(eventQ, &sig, portMAX_DELAY) == pdTRUE) { 
          // we have a signal

          if (sig >= ON && sig <= OFF) {
            // it's a valid signal
            // Serial.print("valid signal ");
            // Serial.print((byte) sig, HEX);
            // Serial.println(" received");

            // look up the next state
            State next_state = stt[currentState][sig];
            
            // Serial.print("current state is ");
            // Serial.println(currentState);
            // Serial.print("next state is ");
            // Serial.println(next_state);

            // check that a transition is defined
            if (next_state > creation) {
              // there is a valid transition defined
              currentState = next_state;
              // execute the state actions
              (this->*stateProc[currentState])();
            } else {
             // Serial.println("no state transition defined");
            }
          }
        }
      }
    }
};


StateMachine sm1 = StateMachine("MyStateMachineA", 1);
StateMachine sm2 = StateMachine("MyStateMachineB", 2);
StateMachine sm3 = StateMachine("MyStateMachineC", 3);

byte L_ON = 0x00;
byte L_OFF = 0x01;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  // vTaskDelay(1000);


  // send an event to the StateMachine

  xQueueSend(sm1.eventQ, (void *) &L_OFF, portMAX_DELAY);
  Serial.println("sent off");
  xQueueSend(sm1.eventQ, (void *) &L_ON, portMAX_DELAY);
  Serial.println("sent on");
  xQueueSend(sm1.eventQ, (void *) &L_OFF, portMAX_DELAY);
  Serial.println("sent off");

  vTaskStartScheduler();

}

void loop() {
  // Serial.println("In loop() -- should not be here");
  // delay(1000);
}
