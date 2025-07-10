package com.firefighter.commander.service;

import com.fasterxml.jackson.core.type.TypeReference;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.firefighter.commander.model.FirefighterData;

import java.io.IOException;
import java.net.URI;
import java.net.http.HttpClient;
import java.net.http.HttpRequest;
import java.net.http.HttpResponse;
import java.time.Duration;
import java.util.List;
import java.util.Map;

public class ApiService {
    
    private static final String BASE_URL = "http://localhost:8080/api/firefighter";
    private final HttpClient httpClient;
    private final ObjectMapper objectMapper;
    
    public ApiService() {
        this.httpClient = HttpClient.newBuilder()
                .connectTimeout(Duration.ofSeconds(10))
                .build();
        this.objectMapper = new ObjectMapper();
    }
    
    /**
     * 获取所有设备的最新数据
     */
    public List<FirefighterData> getLatestData() throws IOException, InterruptedException {
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(BASE_URL + "/data/latest"))
                .timeout(Duration.ofSeconds(30))
                .header("Content-Type", "application/json")
                .GET()
                .build();
        
        HttpResponse<String> response = httpClient.send(request, 
                HttpResponse.BodyHandlers.ofString());
        
        if (response.statusCode() == 200) {
            return objectMapper.readValue(response.body(), 
                    new TypeReference<List<FirefighterData>>() {});
        } else {
            throw new IOException("API请求失败，状态码: " + response.statusCode());
        }
    }
    
    /**
     * 获取指定设备的最新数据
     */
    public FirefighterData getLatestDataByDevice(String deviceId) throws IOException, InterruptedException {
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(BASE_URL + "/data/latest/" + deviceId))
                .timeout(Duration.ofSeconds(30))
                .header("Content-Type", "application/json")
                .GET()
                .build();
        
        HttpResponse<String> response = httpClient.send(request, 
                HttpResponse.BodyHandlers.ofString());
        
        if (response.statusCode() == 200) {
            return objectMapper.readValue(response.body(), FirefighterData.class);
        } else if (response.statusCode() == 404) {
            return null;
        } else {
            throw new IOException("API请求失败，状态码: " + response.statusCode());
        }
    }
    
    /**
     * 获取设备历史数据
     */
    public List<FirefighterData> getHistoryData(String deviceId, int hours) throws IOException, InterruptedException {
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(BASE_URL + "/data/history/" + deviceId + "?hours=" + hours))
                .timeout(Duration.ofSeconds(30))
                .header("Content-Type", "application/json")
                .GET()
                .build();
        
        HttpResponse<String> response = httpClient.send(request, 
                HttpResponse.BodyHandlers.ofString());
        
        if (response.statusCode() == 200) {
            return objectMapper.readValue(response.body(), 
                    new TypeReference<List<FirefighterData>>() {});
        } else {
            throw new IOException("API请求失败，状态码: " + response.statusCode());
        }
    }
    
    /**
     * 发送撤退指令
     */
    public void sendEvacuateCommand(String deviceId) throws IOException, InterruptedException {
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(BASE_URL + "/command/evacuate/" + deviceId))
                .timeout(Duration.ofSeconds(30))
                .header("Content-Type", "application/json")
                .POST(HttpRequest.BodyPublishers.noBody())
                .build();
        
        HttpResponse<String> response = httpClient.send(request, 
                HttpResponse.BodyHandlers.ofString());
        
        if (response.statusCode() != 200) {
            throw new IOException("发送撤退指令失败，状态码: " + response.statusCode());
        }
    }
    
    /**
     * 发送状态查询指令
     */
    public void sendStatusRequest(String deviceId) throws IOException, InterruptedException {
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(BASE_URL + "/command/status/" + deviceId))
                .timeout(Duration.ofSeconds(30))
                .header("Content-Type", "application/json")
                .POST(HttpRequest.BodyPublishers.noBody())
                .build();
        
        HttpResponse<String> response = httpClient.send(request, 
                HttpResponse.BodyHandlers.ofString());
        
        if (response.statusCode() != 200) {
            throw new IOException("发送状态查询指令失败，状态码: " + response.statusCode());
        }
    }
    
    /**
     * 发送自定义指令
     */
    public void sendCustomCommand(String deviceId, String command) throws IOException, InterruptedException {
        Map<String, Object> commandData = Map.of(
                "command", "CUSTOM",
                "data", command,
                "timestamp", System.currentTimeMillis()
        );
        
        String jsonBody = objectMapper.writeValueAsString(commandData);
        
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(BASE_URL + "/command/custom/" + deviceId))
                .timeout(Duration.ofSeconds(30))
                .header("Content-Type", "application/json")
                .POST(HttpRequest.BodyPublishers.ofString(jsonBody))
                .build();
        
        HttpResponse<String> response = httpClient.send(request, 
                HttpResponse.BodyHandlers.ofString());
        
        if (response.statusCode() != 200) {
            throw new IOException("发送自定义指令失败，状态码: " + response.statusCode());
        }
    }
    
    /**
     * 获取在线设备列表
     */
    public List<String> getOnlineDevices() throws IOException, InterruptedException {
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(BASE_URL + "/devices/online"))
                .timeout(Duration.ofSeconds(30))
                .header("Content-Type", "application/json")
                .GET()
                .build();
        
        HttpResponse<String> response = httpClient.send(request, 
                HttpResponse.BodyHandlers.ofString());
        
        if (response.statusCode() == 200) {
            return objectMapper.readValue(response.body(), 
                    new TypeReference<List<String>>() {});
        } else {
            throw new IOException("API请求失败，状态码: " + response.statusCode());
        }
    }
    
    /**
     * 获取系统统计信息
     */
    public Map<String, Object> getSystemStats() throws IOException, InterruptedException {
        HttpRequest request = HttpRequest.newBuilder()
                .uri(URI.create(BASE_URL + "/stats"))
                .timeout(Duration.ofSeconds(30))
                .header("Content-Type", "application/json")
                .GET()
                .build();
        
        HttpResponse<String> response = httpClient.send(request, 
                HttpResponse.BodyHandlers.ofString());
        
        if (response.statusCode() == 200) {
            return objectMapper.readValue(response.body(), 
                    new TypeReference<Map<String, Object>>() {});
        } else {
            throw new IOException("API请求失败，状态码: " + response.statusCode());
        }
    }
} 