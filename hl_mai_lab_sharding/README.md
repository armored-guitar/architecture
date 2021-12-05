# SQL webservice
Сборка: в корне запускаем
```sh
docker build . -t webservice_3
```
Дефолтный порт нашего вебсервиса это 8080 port хоста
Для запуска необходимо поднять два контейнера
```sh
docker-compose --env-file project_env  up
```
Реализованы все 3 метода
