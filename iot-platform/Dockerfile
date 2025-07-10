FROM openjdk:11-jre-slim

LABEL maintainer="firefighter-team@example.com"
LABEL description="消防员智能安全手表物联网平台"

WORKDIR /app

# 安装必要的系统包
RUN apt-get update && apt-get install -y \
    curl \
    netcat \
    && rm -rf /var/lib/apt/lists/*

# 复制JAR文件
COPY target/firefighter-iot-platform-*.jar app.jar

# 创建应用用户
RUN addgroup --system spring && adduser --system spring --ingroup spring
RUN chown -R spring:spring /app
USER spring:spring

# 健康检查
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
    CMD curl -f http://localhost:8080/actuator/health || exit 1

# 暴露端口
EXPOSE 8080

# 启动命令
ENTRYPOINT ["java", "-jar", "/app/app.jar"]

# JVM参数优化
ENV JAVA_OPTS="-Xms512m -Xmx1024m -XX:+UseG1GC -XX:G1HeapRegionSize=16m -XX:+UseStringDeduplication"

CMD ["sh", "-c", "java $JAVA_OPTS -jar app.jar"] 