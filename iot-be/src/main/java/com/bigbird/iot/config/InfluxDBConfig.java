package com.bigbird.iot.config;

import org.influxdb.InfluxDB;
import org.influxdb.InfluxDBFactory;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Bean;
import org.springframework.context.annotation.Configuration;

@Configuration
public class InfluxDBConfig {
	@Value("${influxdb.url}")
	private String influxDBUrl;

//	@Value("${influxdb.username}")
//	private String username;
//
//	@Value("${influxdb.password}")
//	private String password;

	@Bean
	public InfluxDB influxDB() {
		return InfluxDBFactory.connect(influxDBUrl);
	}
}
