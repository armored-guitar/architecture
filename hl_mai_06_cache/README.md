# SQL webservice
Сборка: в корне запускаем
```sh
docker build . -t webservice
```
Дефолтный порт нашего вебсервиса это 8080 port хоста
Для запуска необходимо поднять два контейнера
```sh
docker-compose --env-file project_env  up
```
Реализованы все методы и проведено сравнение метода get по логину с кешем и без
## Cash
```sh
wrk -c 8 -d 30 -t 8 "http://localhost:8080/person?login=rcoughlan0"
```
8 threads and 8 connections

| Thread Stats | Avg    | Stdev  | Max     | +/- Stdev |
|--------------|--------|--------|---------|-----------|
| Latency      | 3.07ms | 1.51ms | 41.26ms | 76.84%    |
| Req/Sec      | 332.61 | 32.60  | 444.00  | 69.92%    |

79524 requests in 30.02s, 16.99MB read
Requests/sec:   2649.15
Transfer/sec:    579.52KB

## Without cash
```sh
wrk -c 8 -d 30 -t 8 "http://localhost:8080/person?login=rcoughlan0&no_cache=1"
```
8 threads and 8 connections

| Thread Stats | Avg    | Stdev  | Max     | +/- Stdev |
|--------------|--------|--------|---------|-----------|
| Latency      | 9.45ms | 2.11ms | 26.50ms | 69.21%    |
| Req/Sec      | 106.14 | 12.24  | 160.00  | 66.75%    |

  25441 requests in 30.10s, 5.44MB read
Requests/sec:    845.21
Transfer/sec:    184.92KB

## Итог
Кэш позволил ускорить обработку примерно в три раза
