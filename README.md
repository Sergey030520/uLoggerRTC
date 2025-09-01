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
4. [UART Middleware](#uart_middleware)
5. [Сборка проекта](#build_project)
6. [Загрузка на плату](#flash_to_board)
7. [Пример работы](#example_work_project)
8. [Пример вывода (Log Output)](#log_output)

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

## UART Middleware <a name="uart_middleware"></a>

Файл `middleware/usart_board.c` содержит функцию:

```
void init_uart();
```
Эта функция выполняет конфигурацию USART1 и всех основных параметров UART:

Настройка TX/RX пинов.

Включение DMA для передачи данных.

Установка скорости передачи по умолчанию 115200 бод.

Используется для логирования всех событий через LOG_INFO, LOG_ERROR, LOG_DEBUG, LOG_WARNING.

Таким образом, UART инициализируется при старте системы через вызов init_board().

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
## Пример вывода (Log Output) <a name="log_output"></a>

Ниже приведён пример вывода логов из UART.  

- Строки вида `[main.c:120] {{...}}` — это текущее состояние RTC (год, месяц, неделя, день, часы, минуты, секунды).  
- Строки с префиксом `[BTN]` означают **нажатие кнопки**, после которого обновляется дата или время.  

### Соответствие кнопок:
- **Кнопка 1** → установка полной даты и времени.  
- **Кнопка 2** → установка только времени.  
- **Кнопка 3** → установка только даты.  

### Пример логов:
```
[main.c:120] {{year:1980, month:1, week:1, day:1}; {hour:00, minute:00, sec:03}}
[main.c:120] {{year:1980, month:1, week:1, day:1}; {hour:00, minute:00, sec:04}}
[main.c:120] {{year:1980, month:1, week:1, day:1}; {hour:00, minute:00, sec:04}}
[main.c:120] {{year:1980, month:1, week:1, day:1}; {hour:00, minute:00, sec:05}}
[main.c:120] {{year:1980, month:1, week:1, day:1}; {hour:00, minute:00, sec:06}}
[main.c:86] [BTN] Set date -> 2025-08-15 (w=1)
[main.c:120] {{year:2085, month:8, week:41, day:15}; {hour:00, minute:00, sec:06}}
[main.c:120] {{year:2085, month:8, week:41, day:15}; {hour:00, minute:00, sec:07}}
[main.c:69] [BTN] Set time -> 07:20:00[main.c:120] {{year:2085, month:8, week:41, day:15}; {hour:07, minute:20, sec:00}}
[main.c:120] {{year:2085, month:8, week:41, day:15}; {hour:07, minute:20, sec:01}}
[main.c:50] [BTN] Set full datetime -> 2025-02-24 (w=1) 10:50:00
[main.c:120] {{year:2025, month:2, week:41, day:24}; {hour:10, minute:50, sec:00}}
```
