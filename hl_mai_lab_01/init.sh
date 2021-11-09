CONTAINER_NAME=hl_mai_lab_01_mysql_1
export $(cat project_env | sed 's/#.*//g' | xargs)

docker exec -i ${CONTAINER_NAME} mysql --user=root --password="${MYSQL_ROOT_PASSWORD/$'\r'/}" << END

drop database if exists webserver;
CREATE DATABASE webserver;
GRANT ALL PRIVILEGES ON webserver.* TO 'webserver_admin'@'%';
FLUSH PRIVILEGES;
USE webserver;
CREATE TABLE  Person (
    login VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    first_name VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    last_name VARCHAR(256) CHARACTER SET utf8 COLLATE utf8_unicode_ci NOT NULL,
    age INT NOT NULL,
    PRIMARY KEY (login),
    INDEX name (last_name,first_name)
);
END

docker cp mock_data.csv ${CONTAINER_NAME}:/mock_data.csv

docker exec -i ${CONTAINER_NAME} mysql --user=root --password="${MYSQL_ROOT_PASSWORD/$'\r'/}" << END

use webserver;
LOAD DATA INFILE '/mock_data.csv'
INTO TABLE Person
FIELDS TERMINATED BY ','
ENCLOSED BY '"'
LINES TERMINATED BY '\n'
IGNORE 1 ROWS;

END

