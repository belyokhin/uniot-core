// uLisp
// (def odd (n) (= 1 (% n 2))) (def color (n) (% n 3)) (task 30 200 '(led (color (t-get env 't_pass)) (odd (t-get env 't_pass))))
// (def color (n) (% n 3))(task 0 500 '( (print (ldr)) (led 0 0) (led 1 0) (led 2 0) (led ( - (/ (ldr) 100) 1) 1)))

// unLisp
// (defun odd (n) (= 1 (% n 2))) (defun color (n) (% n 3)) (task 30 200 '(led (color #t_pass) (odd #t_pass)))
// (defun color (n) (% n 3))(task 0 500 '(list (print (ldr)) (led 0 0) (led 1 0) (led 2 0) (led ( - (/ (ldr) 100) 1) 1)))

#include <Uniot.h>
#include <Board-WittyCloud.h>
#include <CBOR.h>
#include <AppKit.h>

using namespace uniot;

AppKit MainAppKit(MyCredentials, PIN_BUTTON, LOW, RED);

String DeviceId = String(ESP.getChipId(), HEX); // TODO: CBOR: implement storage for dynamic values 

MQTTDevice mqttDevice([](MQTTDevice *device, const String &topic, const Bytes &pa) {
  Serial.println(topic);
  Serial.write(pa.raw(), pa.size());
  Serial.println();

  if (topic.endsWith("script")) {
    unLisp::getInstance().runCode(pa);
  }

  if (topic.endsWith("online/request")) {
    // CBOR packet;
    // packet.put("id", DeviceId.c_str()); // TODO: CBOR: implement storage for dynamic values
    // packet.put("type", "rgb");

    mqttDevice.publish("bits/TEST/online/response", CBOR().put("id", DeviceId.c_str()).put("type", "relay").build());
  }
});

auto taskPrintHeap = TaskScheduler::make([&](short t) {
  Serial.println(ESP.getFreeHeap());
  // Serial.println(WiFi.status());
  // Serial.println(analogRead(LDR));
});

auto taskPrintOwner = TaskScheduler::make([&](short t) {
  Serial.println("Owner: " + MyCredentials.getOwnerId());
  // Serial.println(WiFi.status());
  // Serial.println(analogRead(LDR));
});

struct Obj *user_prim_led(void *root, struct Obj **env, struct Obj **list)
{
  auto args = eval_list(root, env, list);
  if (length(args) != 2)
    error("Malformed task");

  auto objColor = args->car;
  auto objState = args->cdr->car;

  // TODO: check types
  // TODO: allow bool too

  auto color = objColor->value;
  auto state = objState != Nil;
  if (objState->type == TINT)
  {
    state = objState->value != 0;
  }

  uint8_t pin = 0;
  switch (color)
  {
  case 0:
    pin = RED;
    break;
  case 1:
    pin = GREEN;
    break;
  case 2:
    pin = BLUE;
    break;
  default:
    error("Out of range: color: [0, 1, 2]");
  }

  digitalWrite(pin, state);
  return objState;
}

struct Obj *user_prim_ldr(void *root, struct Obj **env, struct Obj **list)
{
  DEFINE1(objLdr);
  *objLdr = make_int(root, analogRead(LDR));
  return *objLdr;
}

void inject()
{
  Serial.begin(9600);

  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(LDR, INPUT);

  MainAppKit.getMQTT().setServer("broker.hivemq.com", 1883);
  MainAppKit.getMQTT().addDevice(&mqttDevice);
  mqttDevice.subscribe("bits/TEST/online/request");
  mqttDevice.subscribe("bits/TEST/script");

  MainBroker.connect(&MainAppKit);
  MainScheduler.push(&MainAppKit)
      ->push(taskPrintHeap)
      ->push(taskPrintOwner);

  taskPrintHeap->attach(500);
  taskPrintOwner->attach(500);

  MainAppKit.attach();
  MainAppKit.begin();

  unLisp::getInstance().pushPrimitive("led", user_prim_led);
  unLisp::getInstance().pushPrimitive("ldr", user_prim_ldr);

  Serial.print("CHIP_ID: ");
  Serial.println(String(ESP.getChipId(), HEX));
  Serial.print("DEVICE_ID: ");
  Serial.println(MyCredentials.getDeviceId());
}
