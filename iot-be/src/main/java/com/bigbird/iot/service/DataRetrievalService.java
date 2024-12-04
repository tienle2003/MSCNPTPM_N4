package com.bigbird.iot.service;

import java.util.HashMap;
import java.util.Map;

import org.influxdb.dto.QueryResult;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.scheduling.annotation.EnableScheduling;
import org.springframework.scheduling.annotation.Scheduled;
import org.springframework.stereotype.Service;

import com.bigbird.iot.controller.WebSocketController;
import com.fasterxml.jackson.core.JsonProcessingException;
import com.fasterxml.jackson.databind.ObjectMapper;

@Service
@EnableScheduling
public class DataRetrievalService {
	@Autowired
	private InfluxDBService influxDBService;

	@Autowired
	private WebSocketController webSocketController;

	private ObjectMapper objectMapper = new ObjectMapper();

	@Scheduled(fixedRate = 2000)
	public void sendRealTimeData() {
		try {
			String jsonData = getDataFromInfluxDB();
			if (jsonData != null) {
				webSocketController.sendRealtimeUpdates(jsonData);
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	// Tạo phương thức lấy dữ liệu từ InfluxDB
	public String getDataFromInfluxDB() {
		try {
			QueryResult queryRSR0 = influxDBService.queryData("SELECT * FROM RS_R0");
			QueryResult queryHumidity = influxDBService.queryData("SELECT * FROM Humidity");
			QueryResult queryTemperature = influxDBService.queryData("SELECT * FROM Temperature");

			Map<String, Object> dataMap = new HashMap<>();
			dataMap.put("RS_R0", queryRSR0);
			dataMap.put("HUMIDITY", queryHumidity);
			dataMap.put("TEMPERATURE", queryTemperature);

			return objectMapper.writeValueAsString(dataMap);
		} catch (JsonProcessingException e) {
			e.printStackTrace();
			return "Error, can not get data!!! Wait some minutes";
		}
	}
}
