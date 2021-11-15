FROM ubuntu:latest
EXPOSE 8080

ENV DEBIAN_FRONTEND=noninteractive \
    MYSQL_HOST=127.0.0.1 \
    MYSQL_PORT=3306 \
    MYSQL_NAME=webserver \
    MYSQL_LOGIN=root \
    MYSQL_PASSWORD=admin \
    CACHE_SERVERS=" "

RUN apt-get update -y && apt-get install -y \
    wget \
    unzip \
    build-essential \
    openjdk-11-jdk \
    unixodbc-dev \
    libssl-dev \
    cmake \
    g++ \
    libpoco-dev \
    && wget -q https://dlcdn.apache.org//ignite/2.11.0/apache-ignite-2.11.0-bin.zip \
    && unzip apache-ignite-2.11.0-bin.zip \
    && mkdir apache-ignite-2.11.0-build && cd apache-ignite-2.11.0-build \
    && cmake -DCMAKE_BUILD_TYPE=Release -DWITH_THIN_CLIENT=ON /apache-ignite-2.11.0-bin/platforms/cpp \
    && make && make install

COPY web_server /web_server

WORKDIR /web_server/build

RUN cmake .. && cmake --build . --target main -- -j 9

CMD /web_server/build/main\
 --host=$MYSQL_HOST\
 --port=$MYSQL_PORT\
 --database=$MYSQL_NAME\
 --login=$MYSQL_LOGIN\
 --password=$MYSQL_PASSWORD\
 --cache_servers=$CACHE_SERVERS
