#include <GyverBME280.h>
#include <LiquidCrystal.h>

#define button_toggle_display_pin 6
#define button_toggle_cond_pin 5
#define buzzer_pin 4
#define led_alert_pin 3
#define temp_cond 0
#define pres_cond 1

LiquidCrystal lcd(9, 8, 7, 10, 11, 12);
GyverBME280 bme;
int stat = temp_cond;
bool wasUp = true;
float pressure_delta = 101200, temperature_delta = 27.5; //нормальные показтель температуры и давления по умолчанию: 101200 Па и 27.5 градусов Цельсия
float sum_pressure = 0, sum_temperature = 0;

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
  if (!digitalRead(button_toggle_cond_pin)) // Высчитываем нормалиные значения
  {
    calcNormal();
  }
  else
  {
    bool isUp = digitalRead(button_toggle_display_pin);
    float pressure = bme.readPressure();
    float temperature = bme.readTemperature();
    // Проверка на несоответствие нормальным показателям
    if (pressure < pressure_delta - 100 || pressure > pressure_delta + 500 || temperature > temperature_delta + 0.5 || temperature < temperature_delta - 2.5) // Учитываем погрешность вычислений
    {
      digitalWrite(led_alert_pin, HIGH);
      tone(buzzer_pin, 4500, 40);
      delay(100);
      digitalWrite(led_alert_pin, LOW);
    }
    else
      digitalWrite(led_alert_pin, LOW);
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
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(Pa_To_kPa(pressure));
      lcd.print(" kPa");
      lcd.setCursor(0, 1);
      lcd.print(Pa_To_kPa(pressure_delta));
      lcd.print(" kPa");
    }
    else if (stat == pres_cond)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(temperature);
      lcd.print(" " + String(char(223)) + "C");
      lcd.setCursor(0, 1);
      lcd.print(temperature_delta);
      lcd.print(" " + String(char(223)) + "C");
    }
    delay(250);
  }
}


void calcNormal()
{
    lcd.clear();
    lcd.print("...");
    for (int i = 0; i < 5; i++)
    {
      sum_pressure += bme.readPressure();
      sum_temperature += bme.readTemperature();
      delay(200);
    }
    // 5 - кол-во вычислений
    pressure_delta = sum_pressure / 5;
    temperature_delta = sum_temperature / 5;
    sum_pressure = 0; sum_temperature = 0;
    lcd.clear();
}


float Pa_To_kPa(float numb){
  float result = numb / 1000;
  return result;
}
