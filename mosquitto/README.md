# Mosquitto MQTT Broker

This service provides the central MQTT broker for the HVAC blower predictive
maintenance system. It receives messages from the Python edge application and
forwards them to subscribers such as the Node-RED dashboard.

## Configuration

The broker uses Eclipse Mosquitto and listens on TCP port `1883`.

Configuration file:

```text
mosquitto/mosquitto.conf
```

Current configuration:

```conf
listener 1883
allow_anonymous true
```

Anonymous access is enabled for local Docker-based testing. In a production
industrial deployment, authentication or TLS should be added.

## Docker Compose Service

The broker is defined as the `mqtt` service in `docker-compose.yml`.

```yaml
mqtt:
  image: eclipse-mosquitto:2.0
  container_name: mqtt
  restart: unless-stopped
  ports:
    - "1883:1883"
  volumes:
    - ./mosquitto/mosquitto.conf:/mosquitto/config/mosquitto.conf
```

Other Docker services can reach the broker using:

```text
Host: mqtt
Port: 1883
```

## MQTT Topics

Sensor data topic:

```text
sensors/group20/hvac-blower/data
```

This topic carries processed sensor readings such as motor current and moving
average.

Alert topic:

```text
alerts/group20/hvac-blower/status
```

This topic carries anomaly detection results from the edge AI application.

## Testing

Start the MQTT broker:

```powershell
docker-compose up -d mqtt
```

Check that the broker is running:

```powershell
docker-compose ps mqtt
```

Check broker logs:

```powershell
docker-compose logs --tail=20 mqtt
```

Expected log lines:

```text
Config loaded from /mosquitto/config/mosquitto.conf.
Opening ipv4 listen socket on port 1883.
mosquitto version 2.0.22 running
```

### Manual Publish/Subscribe Test

Open one terminal and subscribe to the sensor topic:

```powershell
docker exec -it mqtt mosquitto_sub -h localhost -p 1883 -t sensors/group20/hvac-blower/data -v
```

Open another terminal and publish a test message:

```powershell
docker exec -it mqtt mosquitto_pub -h localhost -p 1883 -t sensors/group20/hvac-blower/data -m "test-message"
```

Expected subscriber output:

```text
sensors/group20/hvac-blower/data test-message
```

### Python Edge Publisher Test

Start the Python edge publisher:

```powershell
docker-compose up -d --build python-edge
```

Subscribe to live sensor data:

```powershell
docker exec -it mqtt mosquitto_sub -h localhost -p 1883 -t sensors/group20/hvac-blower/data -v
```

Subscribe to live alert messages:

```powershell
docker exec -it mqtt mosquitto_sub -h localhost -p 1883 -t alerts/group20/hvac-blower/status -v
```

If JSON messages are received on both topics, the MQTT broker is correctly
routing data between the edge application and subscribers.
