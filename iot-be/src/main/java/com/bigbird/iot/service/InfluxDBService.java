package com.bigbird.iot.service;

import java.util.concurrent.TimeUnit;

import org.influxdb.InfluxDB;
import org.influxdb.dto.Point;
import org.influxdb.dto.Query;
import org.influxdb.dto.QueryResult;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.beans.factory.annotation.Value;
import org.springframework.stereotype.Service;

import com.bigbird.iot.model.DataPoint;

@Service
public class InfluxDBService {
	@Autowired
	private InfluxDB influxDB;

	@Value("${influxdb.database}")
	private String database;

	public QueryResult queryData(String query) {
		QueryResult queryResult = influxDB.query(new Query(query, database));
		return queryResult;
	}

	public void sendData(DataPoint dataPoint) {
		Point point = Point.measurement(dataPoint.getMeasurement())
				.tag("tag", dataPoint.getTag())
				.addField("value", dataPoint.getValue())
				.time(System.currentTimeMillis(), TimeUnit.MILLISECONDS)
				.build();
		influxDB.write(database, "autogen", point);
	}
}
