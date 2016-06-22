# WebCrawler PasBot
Простой веб краулер, для обхода сайтов.
Для сборки требуется поддержка С++11

(-std=c++11 or -std=gnu++11 for gcc).

На данный момент, никакую информацию с сайтов не собирает, просто обходит весь сайт.


Требуемые для сборки библиотеки:

boost::regex (1_59v) и ws2_32.lib при компиляции на Windows





Для Windows требуется подключать ws2_32.lib:

\#pragma comment(lib, "Ws2_32.lib") в Visual Studio

-lws2_32 для MinGW


Состоит из трех частей:

1) Библиотека для работы по сети, поддерживаемая только HTTP(1.0/1.1).

2) Парсер APX - Api for parsing XML.

3) Сам краулер(алгоритмы сбора ссылок, определения уникальных ссылок, сбор внешних ссылок).
