FROM ubuntu:latest
EXPOSE 8080
ENV DEBIAN_FRONTEND=noninteractive \
    MYSQL_HOST=127.0.0.1 \
    MYSQL_PORT=3306 \
    MYSQL_NAME=webserver \
    MYSQL_LOGIN=root \
    MYSQL_PASSWORD=admin

RUN apt-get update -y && apt-get install -y \
    cmake \
    g++ \
    libpoco-dev

COPY web_server /web_server

WORKDIR /web_server/build

RUN cmake .. && cmake --build . --target main -- -j 9

CMD /web_server/build/main\
 --host=$MYSQL_HOST\
 --port=$MYSQL_PORT\
 --database=$MYSQL_NAME\
 --login=$MYSQL_LOGIN\
 --password=$MYSQL_PASSWORD
