#include <GyverBME280.h>
#include <LiquidCrystal.h>

#define button_toggle_display_pin 6
#define button_toggle_cond_pin 5
#define temp_cond 0
#define pres_cond 1

LiquidCrystal lcd(9, 8, 7, 10, 11, 12);
GyverBME280 bme;
int stat = temp_cond;
bool wasUp = true;
int cond = 1; // 1 - вычисление нормальных показателей, 2 - основная работа
float pressure_delta = 101200, temperature_delta = 27.5; //нормальные показтель температуры и давления по умолчанию: 101200 Па и 27.5 градусовC

float calculateAverage(float arr[], int len);

void setup()
{
  lcd.begin(16, 2);
  bme.begin(0x76);
  pinMode(button_toggle_display_pin, INPUT_PULLUP);
  pinMode(button_toggle_cond_pin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}
void loop()
{
  if (digitalRead(button_toggle_display_pin))
    cond = 1 - cond;
  if (cond == 1)
  {
    float pressure_data[5], temperature_data[5];
    for (int i = 0; i < 5; i++)
    {
      pressure_data[i] = bme.readPressure();
      temperature_data[i] = bme.readTemperature();
      delay(200);
    }
    pressure_delta = calculateAverage(pressure_data, 5);
    temperature_delta = calculateAverage(temperature_data, 5);
  }
  else
  {
    bool isUp = digitalRead(button_toggle_display_pin);
    float pressure = bme.readPressure();
    float temperature = bme.readTemperature();
    if (pressure < pressure_delta || temperature > temperature_delta)
      digitalWrite(LED_BUILTIN, HIGH);
    else
      digitalWrite(LED_BUILTIN, LOW);
    if (wasUp && !isUp)
    {
      delay(50);
      isUp = digitalRead(button_toggle_display_pin);
      if (!isUp)
        stat = 1 - stat;
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
}

float calculateAverage(float arr[], float len)
{
  float sum = 0;
  for (int i = 0; i < len; i++)
    sum += arr[i];
  return sum / len;
}
