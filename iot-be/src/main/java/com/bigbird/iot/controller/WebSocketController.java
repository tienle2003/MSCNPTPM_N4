package com.bigbird.iot.controller;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.messaging.simp.SimpMessagingTemplate;
import org.springframework.stereotype.Controller;

@Controller
public class WebSocketController {
	@Autowired
	private SimpMessagingTemplate simpMessagingTemplate;

	public void sendRealtimeUpdates(String message) {
		simpMessagingTemplate.convertAndSend("/topic/messages", message);
	}
}
