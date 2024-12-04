package com.bigbird.iot.controller;

import org.influxdb.dto.QueryResult;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RequestMapping;
import org.springframework.web.bind.annotation.RequestParam;
import org.springframework.web.bind.annotation.RestController;

import com.bigbird.iot.model.DataPoint;
import com.bigbird.iot.service.InfluxDBService;

@RestController
@RequestMapping("/api/v1/influxdb")
public class QueryController {
	@Autowired
	private InfluxDBService influxDBService;

	@GetMapping("test")
	public String queryData() {
		return "Hello from /api/v1/test endpoint!";
	}

	@GetMapping("")
	public QueryResult queryData(@RequestParam("query") String query) {
		QueryResult rs = influxDBService.queryData(query);
		System.out.println(rs);
		return rs;
	}

	@PostMapping("")
	public void sendData(@RequestBody DataPoint dataPoint) {
		influxDBService.sendData(dataPoint);
	}
}
