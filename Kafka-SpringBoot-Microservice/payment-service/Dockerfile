FROM maven:3.8.4-openjdk-8 AS build
WORKDIR /home
COPY . /home/
RUN mvn clean package -DskipTests

FROM openjdk:8-jdk-alpine
EXPOSE 8084
COPY --from=build /home/target/*.jar app.jar
ENTRYPOINT ["java", "-jar", "/app.jar"]
