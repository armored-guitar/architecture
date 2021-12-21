CONTAINER_NAME=mysql
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

while IFS=, read -r login first_name last_name age; do
   curl -X POST -d "login=$login&first_name=$first_name&last_name=$last_name&age=$age" http://localhost:8080/person
done < mock_data.csv