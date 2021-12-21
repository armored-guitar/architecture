# SQL webservice with kafka queue
Дефолтный порт нашего вебсервиса это 8080 port хоста Для запуска необходимо поднять два контейнера
Для запуска необходимо запустить все 5 контейнеров
```sh
cd docker
docker-compose --env-file project_env  up
```

Далее надо инициализировать базу
```sh
cd ../
chmod +x init.sh
./init.sh
```
Реализованы оба метода, при этом post - через запись в очередь