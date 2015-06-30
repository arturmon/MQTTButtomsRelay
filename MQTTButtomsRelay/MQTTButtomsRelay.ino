// sketch för a "light switch" where you can control light or something 
// else from both vera and a local physical button (connected between digital
// pin 4,5 and GND).
// This node also works as a repeader for other nodes

#include <MySensor.h>
#include <SPI.h>
#include <Bounce2.h>

#define RELAY_PIN  6  // Arduino Digital I/O pin number for relay 
#define RELAY_PIN_2  7  // Arduino Digital I/O pin number for relay 
#define BUTTON_PIN  4  // Arduino Digital I/O pin number for button 
#define BUTTON_PIN_2  5  // Arduino Digital I/O pin number for button 
#define CHILD_ID 1   // Id of the sensor child
#define CHILD_ID_2 2   // Id of the sensor child
#define RELAY_ON 1
#define RELAY_OFF 0

Bounce debouncer = Bounce(); 
Bounce debouncer_2 = Bounce(); 
int oldValue=0;
int oldValue_2=0;
bool state;
bool state_2;
MySensor gw;
MyMessage msg(CHILD_ID,V_LIGHT);
MyMessage msg_2(CHILD_ID_2,V_LIGHT);

void setup()  
{  
  gw.begin(incomingMessage, AUTO, true);

  // Send the sketch version information to the gateway and Controller
  gw.sendSketchInfo("Button & Relay", "0.1");

 // Setup the button
  pinMode(BUTTON_PIN,INPUT);
  pinMode(BUTTON_PIN_2,INPUT);
  // Activate internal pull-up
  digitalWrite(BUTTON_PIN,HIGH);
  digitalWrite(BUTTON_PIN_2,HIGH);
  
  // After setting up the button, setup debouncer
  debouncer.attach(BUTTON_PIN);
  debouncer_2.attach(BUTTON_PIN_2);
  debouncer.interval(5);
  debouncer_2.interval(5);

  // Register all sensors to gw (they will be created as child devices)
  gw.present(CHILD_ID, S_LIGHT);
  gw.present(CHILD_ID_2, S_LIGHT);

  // Make sure relays are off when starting up
  digitalWrite(RELAY_PIN, RELAY_OFF);
  digitalWrite(RELAY_PIN_2, RELAY_OFF);
  // Then set relay pins in output mode
  pinMode(RELAY_PIN, OUTPUT); 
  pinMode(RELAY_PIN_2, OUTPUT);  
      
  // Set relay to last known state (using eeprom storage) 
  state = gw.loadState(CHILD_ID);
  state_2 = gw.loadState(CHILD_ID_2);
  digitalWrite(RELAY_PIN, state?RELAY_ON:RELAY_OFF);
  digitalWrite(RELAY_PIN_2, state_2?RELAY_ON:RELAY_OFF);  
}


/*
*  Example on how to asynchronously check for new messages from gw
*/
void loop() 
{
  gw.process();
  debouncer.update();
  debouncer_2.update();
  // Get the update value
  int value = debouncer.read();
  int value_2 = debouncer_2.read();
  if (value != oldValue && value==0) {
      gw.send(msg.set(state?false:true), true); // Send new state and request ack back
  }
  oldValue = value;
  if (value_2 != oldValue_2 && value_2==0) {
      gw.send(msg_2.set(state_2?false:true), true); // Send new state and request ack back
  }
  oldValue_2 = value_2;  
} 
 
void incomingMessage(const MyMessage &message) {
  // We only expect one type of message from controller. But we better check anyway.
  if (message.isAck()) {
     Serial.println("This is an ack from gateway");
  }

  if (message.type == V_LIGHT) {
    if (message.sensor == CHILD_ID){
     // Change relay state
     state = message.getBool();
     digitalWrite(RELAY_PIN, state?RELAY_ON:RELAY_OFF);
     // Store state in eeprom
     gw.saveState(CHILD_ID, state);
    }
    if (message.sensor == CHILD_ID_2){
     // Change relay state
     state_2 = message.getBool();
     digitalWrite(RELAY_PIN_2, state_2?RELAY_ON:RELAY_OFF);
     // Store state in eeprom
     gw.saveState(CHILD_ID_2, state_2);
    }    
    
     // Write some debug info
     Serial.print("Incoming change for sensor:");
     Serial.print(message.sensor);
     Serial.print(", New status: ");
     Serial.println(message.getBool());

   } 

}

