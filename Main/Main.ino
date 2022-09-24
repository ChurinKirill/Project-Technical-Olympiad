#include <GyverBME280.h>
#include <LiquidCrystal.h>

#define button_toggle_pin 6
#define temp_cond 0
#define pres_cond 1

LiquidCrystal lcd(9, 8, 7, 10, 11, 12);
GyverBME280 bme;
int stat = temp_cond;
bool wasUp = true;
const float pressure_delta = 101200, temperature_delta = 27.5; //временно

void setup()
{
  lcd.begin(16, 2);
  bme.begin(0x76);
  pinMode(button_toggle_pin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop()
{
  bool isUp = digitalRead(button_toggle_pin);
  float pressure = bme.readPressure();
  float temperature = bme.readTemperature();
  if (pressure < pressure_delta || temperature > temperature_delta)
    digitalWrite(LED_BUILTIN, HIGH);
  else
    digitalWrite(LED_BUILTIN, LOW);
  if (wasUp && !isUp)
  {
    delay(50);
    isUp = digitalRead(button_toggle_pin);
    if (!isUp)
    {
      stat = 1 - stat;
    }
  }
  wasUp = isUp;
  
  if (stat == temp_cond)
  {
    lcd.setCursor(0, 0);
    lcd.print(pressure);
    lcd.print(" Pa      ");
  }
  else if (stat == pres_cond)
  {
    lcd.setCursor(0, 0);
    lcd.print(temperature);
    lcd.print(" " + String(char(223)) + "C      ");
  } 
  delay(100);
}
