# IKS_L_2x20_clock

![alt tag](https://raw.githubusercontent.com/so1der/IKS_L_2x20_clock/refs/heads/main/pictures/clock.jpg "Main photo")

Годинник з індикатора клієнта ІКС-Л-2х20.

Для того щоб керувати індикатором, необхідно відпаяти мікросхему ST232C:

![alt tag](https://raw.githubusercontent.com/so1der/IKS_L_2x20_clock/refs/heads/main/pictures/st232c.jpg "ST232C")

І на місце де був контакт мікросхеми R1out, або RX мікроконтролеру, припаяти дріт:

![alt tag](https://raw.githubusercontent.com/so1der/IKS_L_2x20_clock/refs/heads/main/pictures/wire.jpg "Wire")

Тепер, об'єднавши землю ESP8266 (в даному проєкті це саме ESP, але можна використовувати будь який мікроконтролер) і дисплею, а також з'єднавши їх UART (TX з мікроконтролеру на припаяний дріт), дисплеєм можна керувати, відправляючи на нього дані через `Serial.print();` або `Serial.write()`

Щоб скомпілювати прошивку для годинника, потрібні Visual Studio Code, а також розширення PlatformIO. Потрібно скачати/клонувати цей репозиторій, і відкрити його як проєкт в PlatformIO. Після цього його можна буде скомпілювати та прошити. Необхідно підключити RTC модуль HW-111 DS1307 та датчик температури 18B20 за наступною схемою:

![alt tag](https://raw.githubusercontent.com/so1der/IKS_L_2x20_clock/refs/heads/main/pictures/schematic1.jpg "Schematic")
![alt tag](https://raw.githubusercontent.com/so1der/IKS_L_2x20_clock/refs/heads/main/pictures/schematic2.jpg "Schematic")

Після включення, годинник підіймає свою WiFi AP, "Clock_AP" з паролем vfdclock. Після підключення до даної AP, за IP адресою 192.168.4.2 доступні налаштування годинника, де можна калібрувати датчик температури, або синхронізувати годинник з телефоном. Також можна ввести дані від своєї WiFi мережі, щоб годинник під'єднався до неї, і брав дані про час з інтернету, з NTP серверу, який теж можна налаштувати. У випадку синхронізації за допомогою NTP серверу - необхідно вручну вказувати чсасовий пояс.

Також, тепер, після комміту [08982e7](https://github.com/so1der/IKS_L_2x20_clock/commit/08982e725cf3f8a68315823c6cec8a1fc82291ac) є можливість виводити на дисплей свій кастомний текст! Це можна зробити через веб сторінку налаштування годинника, або через HTTP API запит вигляду:
```
http://CLOCK_IP/custom_text?&text=TEXT
```
де CLOCK_IP - IP адреса годинника, а TEXT - відповідно текст який треба вивести. За цю функцію а також за неймовірну роботу щодо покращення читабельності коду, я дуже вдячний **grisha87**!

![alt tag](https://raw.githubusercontent.com/so1der/IKS_L_2x20_clock/refs/heads/main/pictures/custom_text1.jpg "Custom text on Web Page")
![alt tag](https://raw.githubusercontent.com/so1der/IKS_L_2x20_clock/refs/heads/main/pictures/custom_text2.jpg "Custom text")
