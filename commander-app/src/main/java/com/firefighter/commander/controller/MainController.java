package com.firefighter.commander.controller;

import com.firefighter.commander.model.FirefighterData;
import com.firefighter.commander.service.ApiService;
import javafx.application.Platform;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.concurrent.ScheduledService;
import javafx.concurrent.Task;
import javafx.fxml.FXML;
import javafx.fxml.Initializable;
import javafx.scene.control.*;
import javafx.scene.control.cell.PropertyValueFactory;
import javafx.util.Duration;

import java.net.URL;
import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;
import java.util.List;
import java.util.ResourceBundle;

/**
 * 主界面控制器
 * 负责处理UI交互逻辑、数据展示和指令发送
 */
public class MainController implements Initializable {

    @FXML private TableView<FirefighterData> firefighterTable;
    @FXML private TableColumn<FirefighterData, String> deviceIdColumn;
    @FXML private TableColumn<FirefighterData, Double> heartRateColumn;
    @FXML private TableColumn<FirefighterData, Double> bloodOxygenColumn;
    @FXML private TableColumn<FirefighterData, Double> temperatureColumn;
    @FXML private TableColumn<FirefighterData, Integer> batteryColumn;
    @FXML private TableColumn<FirefighterData, String> locationColumn;

    @FXML private Button evacuateButton;
    @FXML private Button statusButton;
    @FXML private Button sendCommandButton;
    @FXML private TextField commandField;
    @FXML private TextArea logArea;

    @FXML private Label totalDevicesLabel;
    @FXML private Label onlineDevicesLabel;
    @FXML private Label alertCountLabel;

    private ApiService apiService;
    private ObservableList<FirefighterData> firefighterDataList;
    private ScheduledService<List<FirefighterData>> dataRefreshService;

    @Override
    public void initialize(URL location, ResourceBundle resources) {
        // 初始化API服务
        apiService = new ApiService();
        
        // 初始化数据列表
        firefighterDataList = FXCollections.observableArrayList();
        
        // 设置表格列
        setupTableColumns();
        
        // 设置按钮事件
        setupButtonActions();
        
        // 启动数据刷新服务
        startDataRefreshService();
        
        // 初始日志
        addLog("系统启动成功", "INFO");
        addLog("开始监控消防员设备数据...", "INFO");
    }

    /**
     * 设置表格列
     */
    private void setupTableColumns() {
        deviceIdColumn.setCellValueFactory(new PropertyValueFactory<>("deviceId"));
        heartRateColumn.setCellValueFactory(new PropertyValueFactory<>("heartRate"));
        bloodOxygenColumn.setCellValueFactory(new PropertyValueFactory<>("bloodOxygen"));
        temperatureColumn.setCellValueFactory(new PropertyValueFactory<>("bodyTemperature"));
        batteryColumn.setCellValueFactory(new PropertyValueFactory<>("batteryLevel"));
        
        // 位置列需要特殊处理
        locationColumn.setCellValueFactory(cellData -> {
            FirefighterData data = cellData.getValue();
            String location = String.format("%.6f, %.6f", data.getLatitude(), data.getLongitude());
            return new javafx.beans.property.SimpleStringProperty(location);
        });

        // 设置表格数据
        firefighterTable.setItems(firefighterDataList);
        
        // 设置健康状态行样式
        firefighterTable.setRowFactory(tv -> new TableRow<FirefighterData>() {
            @Override
            protected void updateItem(FirefighterData item, boolean empty) {
                super.updateItem(item, empty);
                
                if (item == null || empty) {
                    setStyle("");
                } else {
                    // 根据健康状态设置行颜色
                    if (item.getHeartRate() > 160 || item.getBloodOxygen() < 90 || 
                        item.getBodyTemperature() > 39) {
                        setStyle("-fx-background-color: #ffebee;"); // 红色背景
                    } else if (item.getBatteryLevel() < 20) {
                        setStyle("-fx-background-color: #fff3e0;"); // 橙色背景
                    } else {
                        setStyle("-fx-background-color: #e8f5e8;"); // 绿色背景
                    }
                }
            }
        });
    }

    /**
     * 设置按钮事件
     */
    private void setupButtonActions() {
        // 紧急撤退按钮
        evacuateButton.setOnAction(event -> {
            FirefighterData selectedData = firefighterTable.getSelectionModel().getSelectedItem();
            if (selectedData != null) {
                sendEvacuateCommand(selectedData.getDeviceId());
            } else {
                // 如果没有选中设备，发送给所有设备
                sendEvacuateCommandToAll();
            }
        });

        // 状态查询按钮
        statusButton.setOnAction(event -> {
            FirefighterData selectedData = firefighterTable.getSelectionModel().getSelectedItem();
            if (selectedData != null) {
                sendStatusRequest(selectedData.getDeviceId());
            } else {
                addLog("请选择一个设备进行状态查询", "WARN");
            }
        });

        // 发送自定义指令按钮
        sendCommandButton.setOnAction(event -> {
            String command = commandField.getText().trim();
            if (!command.isEmpty()) {
                FirefighterData selectedData = firefighterTable.getSelectionModel().getSelectedItem();
                if (selectedData != null) {
                    sendCustomCommand(selectedData.getDeviceId(), command);
                    commandField.clear();
                } else {
                    addLog("请选择一个设备发送指令", "WARN");
                }
            } else {
                addLog("请输入指令内容", "WARN");
            }
        });
    }

    /**
     * 启动数据刷新服务
     */
    private void startDataRefreshService() {
        dataRefreshService = new ScheduledService<List<FirefighterData>>() {
            @Override
            protected Task<List<FirefighterData>> createTask() {
                return new Task<List<FirefighterData>>() {
                    @Override
                    protected List<FirefighterData> call() throws Exception {
                        return apiService.getAllFirefighterData();
                    }
                };
            }
        };

        dataRefreshService.setPeriod(Duration.seconds(5)); // 每5秒刷新一次

        dataRefreshService.setOnSucceeded(event -> {
            List<FirefighterData> newData = dataRefreshService.getValue();
            Platform.runLater(() -> {
                firefighterDataList.clear();
                if (newData != null) {
                    firefighterDataList.addAll(newData);
                    updateStatistics();
                }
            });
        });

        dataRefreshService.setOnFailed(event -> {
            Platform.runLater(() -> {
                addLog("数据刷新失败: " + dataRefreshService.getException().getMessage(), "ERROR");
            });
        });

        dataRefreshService.start();
    }

    /**
     * 更新统计信息
     */
    private void updateStatistics() {
        int totalDevices = firefighterDataList.size();
        int onlineDevices = (int) firefighterDataList.stream()
                .filter(data -> data.getBatteryLevel() > 0)
                .count();
        
        int alertCount = (int) firefighterDataList.stream()
                .filter(data -> data.getHeartRate() > 160 || 
                               data.getBloodOxygen() < 90 || 
                               data.getBodyTemperature() > 39 ||
                               data.getBatteryLevel() < 20)
                .count();

        totalDevicesLabel.setText("总设备数: " + totalDevices);
        onlineDevicesLabel.setText("在线设备: " + onlineDevices);
        alertCountLabel.setText("警报数量: " + alertCount);
    }

    /**
     * 发送撤退指令
     */
    private void sendEvacuateCommand(String deviceId) {
        try {
            apiService.sendCommand(deviceId, "EVACUATE", "立即撤退到安全区域");
            addLog("已向设备 " + deviceId + " 发送紧急撤退指令", "INFO");
        } catch (Exception e) {
            addLog("发送撤退指令失败: " + e.getMessage(), "ERROR");
        }
    }

    /**
     * 向所有设备发送撤退指令
     */
    private void sendEvacuateCommandToAll() {
        int successCount = 0;
        for (FirefighterData data : firefighterDataList) {
            try {
                apiService.sendCommand(data.getDeviceId(), "EVACUATE", "立即撤退到安全区域");
                successCount++;
            } catch (Exception e) {
                addLog("向设备 " + data.getDeviceId() + " 发送指令失败: " + e.getMessage(), "ERROR");
            }
        }
        addLog("已向 " + successCount + " 个设备发送紧急撤退指令", "INFO");
    }

    /**
     * 发送状态查询指令
     */
    private void sendStatusRequest(String deviceId) {
        try {
            apiService.sendCommand(deviceId, "STATUS_REQUEST", "请求状态更新");
            addLog("已向设备 " + deviceId + " 发送状态查询指令", "INFO");
        } catch (Exception e) {
            addLog("发送状态查询失败: " + e.getMessage(), "ERROR");
        }
    }

    /**
     * 发送自定义指令
     */
    private void sendCustomCommand(String deviceId, String command) {
        try {
            apiService.sendCommand(deviceId, "CUSTOM", command);
            addLog("已向设备 " + deviceId + " 发送自定义指令: " + command, "INFO");
        } catch (Exception e) {
            addLog("发送自定义指令失败: " + e.getMessage(), "ERROR");
        }
    }

    /**
     * 添加日志
     */
    private void addLog(String message, String level) {
        Platform.runLater(() -> {
            String timestamp = LocalDateTime.now().format(DateTimeFormatter.ofPattern("yyyy-MM-dd HH:mm:ss"));
            String logEntry = String.format("[%s] %s: %s%n", timestamp, level, message);
            logArea.appendText(logEntry);
            
            // 自动滚动到底部
            logArea.setScrollTop(Double.MAX_VALUE);
        });
    }

    /**
     * 停止数据刷新服务
     */
    public void shutdown() {
        if (dataRefreshService != null) {
            dataRefreshService.cancel();
        }
    }
} 