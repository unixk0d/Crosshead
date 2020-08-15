const int sensor01 = 2;//                             Кнопка (на пин поступает постоянно 5 вольт, активируется, когда на пин подается GND)
const int sensor02 = 4;//                             Точка синхронизации координат (Sharp 1A22LC)(она же сенсор инициализации, сенсор полного периода времени цикла)
const int sensor03 = 5;//                             Щелевой тахометр (Sharp 1A22LC), устройства DEV1 (открытая = 0, перекрытая = 1)если ловит HIGH(перекрытая) то показания идут +1, если же ловит LOW(открытая) то показания идут -1.
const int dev01 = 3;//                                Двигатель (Toshiba DCM-1240-2AU) (24х щелевой) 
const int led13 = 13;//                               Встроенный светодиод (Индикация работы DEV1)



boolean initialization = false;//                     Метка статуса инициализации, открывает функцию подачи сигнала на двигатели, остальные функции идут в обход инициализации.
boolean *pInitialization = &initialization;
unsigned int initializationTemp = 1900;//             Временная метка выхода за диапазон времени выполнения цикла инициализации, принуждает провести цикл инициализации заново
unsigned int *pInitializationTemp = &initializationTemp;



unsigned int sensor01Temp = 0;//                       Результирующая импульса в милисекундах
unsigned int *pSensor01Temp = &sensor01Temp;
unsigned int sensor01TempNull = 500;//                 Сброс счетчика в null спустя заданное переменной количество милисекунд
unsigned int *pSensor01TempNull = &sensor01TempNull;
unsigned long sensor01Time1 = 0;//                     Точка времени, вычитаемая из большей
unsigned long *pSensor01Time1 = &sensor01Time1;
unsigned long sensor01Time2 = 0;//                     Точка времени для вычета меньшей точки
unsigned long *pSensor01Time2 = &sensor01Time2;



unsigned int sensor02Temp0 = 0;//                       Результирующая импульса в милисекундах  (ОТРИЦАТЕЛЬНЫЙ ПЕРИОД)
unsigned int *pSensor02Temp0 = &sensor02Temp0;
unsigned int sensor02Temp1 = 0;//                       Результирующая импульса в милисекундах (ПОЛОЖИТЕЛЬНЫЙ ПЕРИОД)
unsigned int *pSensor02Temp1 = &sensor02Temp1;
unsigned int sensor02TempNull = 5000;//                 Сброс счетчика в null спустя заданное переменной количество милисекунд
unsigned int *pSensor02TempNull = &sensor02TempNull;

unsigned long sensor02Time01 = 0;//                     Точка времени, вычитаемая из большей  (ОТРИЦАТЕЛЬНЫЙ ПЕРИОД)
unsigned long *pSensor02Time01 = &sensor02Time01;
unsigned long sensor02Time02 = 0;//                     Точка времени для вычета меньшей точки  (ОТРИЦАТЕЛЬНЫЙ ПЕРИОД)
unsigned long *pSensor02Time02 = &sensor02Time02;
unsigned long sensor02Time1 = 0;//                      Точка времени, вычитаемая из большей (ПОЛОЖИТЕЛЬНЫЙ ПЕРИОД)
unsigned long *pSensor02Time1 = &sensor02Time1;
unsigned long sensor02Time2 = 0;//                      Точка времени для вычета меньшей точки (ПОЛОЖИТЕЛЬНЫЙ ПЕРИОД)
unsigned long *pSensor02Time2 = &sensor02Time2;



unsigned int sensor03Temp = 0;//                       Результирующая длины импульса
unsigned int *pSensor03Temp = &sensor03Temp;
unsigned int sensor03Inc = 0;//                        Количество сработок сенсора
unsigned int *pSensor03Inc = &sensor03Inc;
unsigned int sensor03Cicl = 0;//                       Количество циклов оборота тахометра
unsigned int *pSensor03Cicl = &sensor03Cicl;


void setup() {
  Serial.begin(9600);
  pinMode(sensor01, INPUT);
  pinMode(sensor02, INPUT);
  pinMode(sensor03, INPUT);
  pinMode(dev01, OUTPUT);
  pinMode(led13, OUTPUT);
}



void loop() {
  fInitialization();
  fStatSensor();
  if (*pInitialization == true) fRunDev();
  Serial.print("OK\n");
  //Serial.print(*pSensor01Temp);
  //Serial.print("            ");
  //Serial.print(*pSensor02Temp0);
  //Serial.print("   ");
  //Serial.print(*pSensor02Temp1);
  //Serial.print("\n");

  Serial.print(*pSensor03Temp);
  Serial.print("   ");
  Serial.print(*pSensor03Inc);
  Serial.print("   ");
  Serial.print(*pSensor03Cicl);
  Serial.print("\n");
}



void fInitialization() {  // инициализация START

  //тестовый зброс инициализации
  //  if (digitalRead(sensor02) == 0)
  //      {
  //        *pInitialization = false;
  //      }
  //тестовый зброс инициализации

  if (*pInitialization == false)
  {

    if ((*pSensor02Temp1) <= 300)
    {
      digitalWrite(led13, HIGH);
      analogWrite(dev01, 180);
      //Serial.print("  DEV01 LOW(210)\n");
    }
    else
    {
      // !!!ВНИМАНИЕ!!! для корректной работы необходимо изменить частоту!
      analogWrite(dev01, 116); // пониженный сигнал шим для уменьшения инерции и точного позиционирования метки в точки начала координат (116 - рабочая частота, ? - тестовая(500мл)) зависит от веса, нагружаемого на ось
      if ((*pSensor02Temp1) >= 2000 && (*pSensor02Temp1) <= 2500) // увеличить нагрузку, если механизм заклинило, так как заклинивший механизм есть причина нарушения цикла времени не выполняется правило установки истинности
      {
        digitalWrite(led13, HIGH);
        analogWrite(dev01, 180);
        //Serial.print("  DEV01 LOW(210)\n");
      }
      if ((digitalRead(sensor02) == 1) && (*pSensor02Temp1) < (*pInitializationTemp)) // правило установки истинности, завершающее инициализацию
      {
        *pInitialization = true;
        analogWrite(dev01, 0); // Сигнал остановки на случай, если устройство синхронизировано вручную либо остановка цикла синхронизации
      }
    }

  }

}  // инициализация END



void fStatSensor() {  // sensor START

  // sensor 01 (BTN) START
  if ( digitalRead(sensor01) == HIGH)
  {
    *pSensor01Time1 = 0; // Сброс первоначальной точки отсчета временной 1
  }

  if ( digitalRead(sensor01) == LOW)
  {
    if (*pSensor01Time1 == 0) (*pSensor01Time1) = millis(); // Установка первоночальной временной 1
    *pSensor01Time2 = 0; // Сброс первоначальной точки отсчета временной 2
    if (*pSensor01Time2 == 0) (*pSensor01Time2) = millis(); // Установка первоначальной временной 2
    *pSensor01Temp = (*pSensor01Time2) - (*pSensor01Time1); // Результирующая временная сработки сенсора, переинициализация при каждой сработке сенсора
  }

  if ((*pSensor01Temp) >= (*pSensor01TempNull)) (*pSensor01Temp) = 0;
  // sensor 01 (BTN) END

  // sensor 02 (СИНХРОНИЗАТОР) START
  if ( digitalRead(sensor02) == HIGH)
  {
    *pSensor02Time1 = 0; // Сброс первоначальной точки отсчета временной 1
    if (*pSensor02Time01 == 0) (*pSensor02Time01) = millis(); // Установка первоначальной временной 1
    *pSensor02Time02 = 0; // Сброс первоначальной точки отсчета временной 2
    if (*pSensor02Time02 == 0) (*pSensor02Time02) = millis(); // Установка первоначальной временной 2
    *pSensor02Temp0 = (*pSensor02Time02) - (*pSensor02Time01); // Результирующая временная сработки сенсора, переинициализация при каждой сработке сенсора
  }

  if ( digitalRead(sensor02) == LOW)
  { // цикл крейцкопфа START
    *pSensor02Time01 = 0; // Сброс первоначальной точки отсчета временной 1
    if (*pSensor02Time1 == 0) (*pSensor02Time1) = millis(); // Установка первоначальной временной 1
    *pSensor02Time2 = 0; // Сброс первоначальной точки отсчета временной 2
    if (*pSensor02Time2 == 0) (*pSensor02Time2) = millis(); // Установка первоначальной временной 2
    *pSensor02Temp1 = (*pSensor02Time2) - (*pSensor02Time1); // Результирующая временная сработки сенсора, переинициализация при каждой сработке сенсора
  } // цикл крейцкопфа END

  if ((*pSensor02Temp0) >= (*pSensor02TempNull)) (*pSensor02Temp0) = 0; // сброс счетчика
  if ((*pSensor02Temp1) >= (*pSensor02TempNull)) (*pSensor02Temp1) = 0; // сброс счетчика
  // sensor 02 (СИНХРОНИЗАТОР) END

  // sensor 03 (DEV1) START
  if ( digitalRead(sensor03) == LOW) (*pSensor03Temp)++;
  if ((*pSensor03Temp) >= 200) (*pSensor03Temp) =0;

  if ( digitalRead(sensor03) == HIGH)
  {
    if ((*pSensor03Temp) > 0) (*pSensor03Inc) ++;
    (*pSensor03Temp) = 0;
  }

  if ((*pSensor03Inc) >= 15 && (*pInitialization) == true) // значение по идее должно быть 192, но ловим 15 (!!что есть нехорошо!!) на физический цикл устройства. Диск(24щели) делает 8 полных оборотов (что примерно составляет 192 показания)
  {
    (*pSensor03Cicl) ++;
    (*pSensor03Inc) = 0;
  }
  // sensor 03 (DEV1) END

}  // sensor END



void fRunDev() {  // DEV START

  // Run Dev 01
  if ((*pSensor01Temp) >= 0 && (*pSensor01Temp) <= 250)
  {
    digitalWrite(led13, LOW);
    analogWrite(dev01, 0);
    //Serial.print("  DEV01 STOP (0)\n");
  }

  if ((*pSensor01Temp) >= 251 && (*pSensor01Temp) <= 700)
  {
    if ((*pSensor02Temp1) <= 850)
    {
      digitalWrite(led13, HIGH);
      analogWrite(dev01, 190);
      //Serial.print("  DEV01 LOW(210)\n");
    }
    else
    {
      digitalWrite(led13, HIGH);
      analogWrite(dev01, 255);
      //Serial.print("  DEV01 HIG(255)\n");
    }
  }
  // Run Dev 01

}  // DEV END
