#include <FastBot.h>
#define WIFI_SSID "Имя вашей wifi сети" //Имя Wi-Fi сети
#define WIFI_PASS "пароль от вашей wifi сети" //Пароль Wi-Fi сети
#define BOT_TOKEN "Ваш телеграм токен для бота" //токен для подключения к телеграм боту
#define LED_BUILTIN 33 //Пин сведодиода
#define THERM_PIN 35 // Пин термистора
#define R_FIXED 10000 // Сопротивление резистора в делителе (10 кОм)
#define B_COEFF 3950 // B коэффициент термистора
#define R_25 10000 // Сопротивление термистора при 25 °C
#define T_25 298.15 // Температура при 25 °C в Кельвинах
#define V_REF 5 // Опорное напряжение ESP32
FastBot bot(BOT_TOKEN);
void setup() {
  Serial.begin(9600);
  connectWiFi();
  bot.attach(newMsg);
  pinMode(LED_BUILTIN, OUTPUT);
}
float get_temp(){
  int adcValue = analogRead(THERM_PIN);
  float voltage = (adcValue / 4095.0) * V_REF; // Преобразование АЦП в напряжение
  float resistance = (R_FIXED * voltage) / (V_REF - voltage); // Расчет сопротивления термистора

  // Расчет температуры в Кельвинах
  float tempK = B_COEFF / (log(resistance / R_25) + (B_COEFF / T_25));
  // Преобразование в градусы Цельсия
  float tempC = tempK - 273.15;
  return tempC;
}
// Функция для управления светодиодом (включение/выключение)
void controlLed(bool state) {
  if (state) {
    digitalWrite(LED_BUILTIN, HIGH);  // Включение светодиода
    bot.sendMessage("Я включил светодиод", msg.chatID);
  } else {
    digitalWrite(LED_BUILTIN, LOW);  // Выключение светодиода
    bot.sendMessage("Я выключил светодиод", msg.chatID);
  }
}
// Функция для возвращения строки с меню
String getMenu() {
  return "Список команд:\n"
         "/led_on - включение светодиода\n"
         "/led_off - выключение светодиода\n"
         "/get_temp - Получить температуру\n";
}
// Основная функция обработки сообщений
void newMsg(FB_msg& msg) {
  if (msg.text == "Привет") {
    bot.sendMessage("Привет Телеграм бот на \"Piranha ESP32 V2\"", msg.chatID);
    bot.sendMessage(getMenu(), msg.chatID);  // Отправляем меню при приветствии
  } else if (msg.text == "/led_on") {
    controlLed(true);  // Включаем светодиод
  } else if (msg.text == "/led_off") {
    controlLed(false);  // Выключаем светодиод
  } else if (msg.text == "/get_temp") {
    float temp = get_temp();
    bot.sendMessage("Температура: " + String(temp, 2) + "°C", msg.chatID);
  } else {
    bot.sendMessage(getMenu(), msg.chatID);  // Отправляем меню по любой неизвестной команде
  }
}
void loop() {
  bot.tick();
}
void connectWiFi() {
  delay(2000);
  Serial.begin(115200);
  Serial.println();
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() > 15000) ESP.restart();
  }
  Serial.println("Connected");
}
