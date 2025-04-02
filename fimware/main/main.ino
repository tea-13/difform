// ================================================== Define ==================================================

#define LEFT_MOTOR_A 8     // Цифровой выход (левый мотор). Если 0 - едем вперед
#define LEFT_MOTOR_B 9     // Цифровой выход (левый мотор). Если 0 - едем назад
#define RIGHT_MOTOR_A 10    // Цифровой выход (правый мотор). Если 0 - едем вперед
#define RIGHT_MOTOR_B 11    // Цифровой выход (правый мотор). Если 0 - едем назад

#define LEFT_ENCODER_A 39    // Цифровой вход, канал A энкодера левого колеса
#define LEFT_ENCODER_B 34    // Цифровой вход, канал B энкодера левого колеса 
#define RIGHT_ENCODER_A 32   // Цифровой вход, канал A энкодера правого колеса
#define RIGHT_ENCODER_B 35   // Цифровой вход, канал B энкодера правого колеса


// ================================================== Libraries ==================================================

#include <Wire.h>

#include <iarduino_Position_BMX055.h>
iarduino_Position_BMX055 ACC(BMA);
iarduino_Position_BMX055 GYRO(BMG);
iarduino_Position_BMX055 MAG(BMM);


// ================================================== Global var ==================================================
volatile long left_encoder_value = 0; 
volatile long right_encoder_value = 0;
uint32_t timer1 = 0;

// ================================================== Maing program ==================================================
void setup()
{
  // -------------- Init --------------
  Serial.begin(115200);
  while(!Serial)
  {
    logger_print(0, "Serial", "Doesn't open");
  }
  
  init_encoders();
  init_IMU();
  calibrate_IMU();

  timer1 = millis();
}


void loop()
{
  read_IMU();

  if (millis() - timer1 > 100)
  {
    send_IMU_info();
    send_encoders_info();

    timer1 = millis();
  }
}


// ================================================== Loger ==================================================

// Вывод сообщение
void logger_print(int type, String prefix, String msg)
{
  switch (type)
  {
    case 0: break;
      Serial.print(F("E"));
      break;
    case 1: 
      Serial.print(F("W"));
      break;
    case 2: break;
      Serial.print(F("I"));
      break;
    default:
      return ;
  }

  Serial.print(F("["));
  Serial.print(prefix);
  Serial.print(F("]: "));
  Serial.print(msg);
  Serial.println();
}


// ================================================== Encoder ==================================================

// Инициализация энкодеров
void init_encoders()
{
  attachInterrupt(digitalPinToInterrupt(LEFT_ENCODER_A), left_interrupt, RISING);
  attachInterrupt(digitalPinToInterrupt(RIGHT_ENCODER_A), right_interrupt, RISING);

  pinMode(LEFT_ENCODER_A, INPUT);
  pinMode(LEFT_ENCODER_B, INPUT);
  pinMode(RIGHT_ENCODER_A, INPUT);
  pinMode(RIGHT_ENCODER_B, INPUT);
}

// Функции обработки прерываний
void left_interrupt() 
{
  digitalRead(LEFT_ENCODER_B) ? left_encoder_value++ : left_encoder_value--;
}
void right_interrupt()
{
  digitalRead(RIGHT_ENCODER_B) ? right_encoder_value++ : right_encoder_value--;
}

// Отправка сообщения энкодера
void send_encoders_info()
{
  Serial.printf("I[ENC]: L: %ld R: %ld", left_encoder_value, right_encoder_value);
  Serial.println();
}


// ================================================== IMU ==================================================

// Инициализация IMU сенсора
void init_IMU()
{
  ACC.begin(&Wire);
  GYRO.begin(&Wire);
  MAG.begin(&Wire);
}


// Обновление данных с IMU
void read_IMU()
{
  ACC.read();
  GYRO.read();
  MAG.read();
}

void calibrate_IMU()
{
  Serial.println("КАЛИБРОВКА:");
  Serial.println("Установите модуль осью Z вверх.");
  Serial.println("Модуль должен быть неподвижен. Калибровка начнётся через 3 сек.");
  delay(3000);
    
  Serial.println("Акселерометр...");
  ACC.setFastOffset();
  Serial.println("Гироскоп...");
  GYRO.setFastOffset();
    
  Serial.println("Начинаем калибровку магнитометра...");
  Serial.println("Вращайте модуль так чтоб все его оси побывали во всех направлениях");
  Serial.println("Калибровка закончится через 30 сек.");
  
  uint32_t i = millis();
  while( (millis()-i) < 30000 ){
    MAG.setFastOffset();
  }
  Serial.println("Калибровка магнитометра завершена.");
    
  float data[3];
  MAG.getFastOffset(data);
  for (int i=0; i<3; i++) Serial.printf("%.2f ", data[i]);
  Serial.println();
    
  // float data[3]={-12.5,63.7,-10.6};
  // MAG.setFastOffset(data);
    
  Serial.println("Готово."); 
}


// Вывод в сериал пор информации с IMU
void send_IMU_info()
{
  Serial.printf("I[IMU]: A: %.2f %.2f %.2f G: %.2f %.2f %.2f M: %.2f %.2f %.2f", ACC.axisX, ACC.axisY, ACC.axisZ, GYRO.axisX, GYRO.axisY, GYRO.axisZ, MAG.axisX, MAG.axisY, MAG.axisZ);
  Serial.println();
}

