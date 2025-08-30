# Hi 👋, I'm Sergey Makarov

![Profile views](https://komarev.com/ghpvc/?username=sergey030520&label=Profile%20views&color=0e75b6&style=flat)

- 🔭 I’m currently working on [uLoggerRTC](https://github.com/Sergey030520/uLoggerRTC.git)
- 👨‍💻 All of my projects are available at [https://github.com/Sergey030520](https://github.com/Sergey030520)

## Languages and Tools

![C](https://github.com/tandpfun/skill-icons/blob/main/icons/C.svg) 
![CMake](https://github.com/tandpfun/skill-icons/blob/main/icons/CMake-Dark.svg) 
![VSCode](https://github.com/tandpfun/skill-icons/blob/main/icons/VSCode-Dark.svg) 
![Git](https://github.com/tandpfun/skill-icons/blob/main/icons/Git.svg) 

# uLoggerRTC: RTC логгер для STM32F407VET6

1. [Описание](#description_project)
2. [Основные возможности](#features_project)
3. [Работа с кнопками](#button_events)
4. [Сборка проекта](#build_project)
5. [Загрузка на плату](#flash_to_board)
6. [Пример работы](#example_work_project)

## Описание <a name="description_project"></a>

Проект uLoggerRTC — **логгер времени и даты** на базе STM32F407VET6.  

Особенности проекта:  

- Реализован **RTC логгер**, который выводит текущее время через UART.
- Кнопки позволяют изменять дату и время вручную.
- Встроена система логирования всех действий.
- Пользовательские прерывания для кнопок.
- LED-индикация успешных и неудачных действий.

## Основные возможности

- Отслеживание времени и даты с помощью встроенного RTC.
- Логирование событий через UART (`LOG_INFO`, `LOG_ERROR`, `LOG_DEBUG`, `LOG_WARNING`).
- Управление временем и датой **пользовательскими кнопками**.
- LED-индикация успешных и неудачных операций.
- Таймеры и прерывания для обработки кнопок.

## Работа с кнопками <a name="button_events"></a>

Проект использует **три кнопки**, каждая из которых привязана к отдельному действию:  

- **Кнопка 1:** установка полной даты и времени.  
- **Кнопка 2:** установка только времени.  
- **Кнопка 3:** установка только даты.  

При нажатии на любую кнопку вызывается соответствующая функция, которая обрабатывает событие и обновляет RTC.  

Каждое действие сопровождается:  

- Логированием через UART.  
- Мигающим светодиодом, сигнализирующим об успешной или неудачной установке времени/даты.  

Пример обработки событий реализован в функции `process_button_events()`, где проверяется, какая кнопка была нажата, и вызывается соответствующая установка даты или времени.

## Сборка проекта <a name="build_project"></a>

Проект поддерживает сборку с помощью **CMake**.

Пример сборки через CMake (Linux):

```bash
mkdir build
cd build
cmake ..
make
```
Проект был протестирован на STM32F407VET6 с SD-картой 8GB. Все операции чтения/записи корректны.

## Загрузка на плату <a name="flash_to_board"></a>

На Linux перед загрузкой прошивки необходимо установить утилиту `st-flash`:

```
sudo apt install stlink-tools
```

Для загрузки прошивки на STM используется скрипт load_stm_data.sh с утилитой st-flash:
```
./load_stm_data.sh
```

## Пример работы

```
    int status = init_board();
    RTC_DateTime_Type cur_datetime;
 
    if (status != 0)
    {
        ledOn(13, 1);
        goto error;
    }
    print_clock_frequencies();
    LOG_INFO("Драйверы запущены\r\n");

    init_buttons();
    init_buttons_callbacks();

    while (1)
    {
         if (get_datetime_rtc(&cur_datetime)) {
            LOG_INFO("RTC read error\r\n");
            goto error;
        }

        LOG_INFO("{{year:%d, month:%d, week:%d, day:%d}; {hour:%02d, minute:%02d, sec:%02d}}",
                 cur_datetime.date.year, cur_datetime.date.month, cur_datetime.date.week, cur_datetime.date.day,
                 cur_datetime.time.hour, cur_datetime.time.minute, cur_datetime.time.seconds);

        process_button_events();

        delay_timer(3000);
    }

error:
    while (1)
        ;
    return 0;
```
