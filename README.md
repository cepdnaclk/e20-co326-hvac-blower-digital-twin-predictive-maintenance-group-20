# HVAC Blower Digital Twin Predictive Maintenance

This project implements an **Industrial Internet of Things (IIoT) based Digital Twin system** designed to monitor and predict potential failures in an HVAC blower fan. HVAC (Heating, Ventilation, and Air Conditioning) systems rely heavily on blower fans to maintain proper airflow in buildings and industrial environments. Over time, factors such as airflow blockage, bearing wear, or mechanical stress can increase motor load and eventually lead to system failures. Early detection of such conditions is essential to reduce downtime, prevent equipment damage, and improve energy efficiency.

The objective of this project is to develop a **predictive maintenance framework** that continuously monitors the operational health of a blower fan using real-time sensor data. A **current sensor (ACS712)** is used to measure the motor current of a 12V brushless DC fan, which acts as a scaled representation of an HVAC blower. Variations in motor current are analyzed to detect abnormal operating conditions such as airflow obstruction or increased mechanical load.

An **ESP32-S3 microcontroller** serves as the edge device responsible for data acquisition and local processing. The system performs **edge-level anomaly detection using lightweight machine learning techniques (TinyML)** to identify unusual patterns in motor current. These anomaly scores and sensor readings are transmitted through an **MQTT-based communication pipeline using the Sparkplug B topic structure**, enabling structured industrial messaging and a Unified Namespace (UNS).

The project follows a **four-layer Industrial IoT architecture**:

1. **Perception Layer** – Sensor data acquisition and feature extraction using the ESP32-S3.
2. **Transport Layer** – Secure data transmission via MQTT following a structured topic hierarchy.
3. **Edge Logic Layer** – Data processing, rule-based logic, and orchestration using Node-RED.
4. **Application Layer** – Time-series data storage using InfluxDB and visualization through Grafana dashboards.

The collected data is stored in **InfluxDB**, a time-series database optimized for industrial telemetry. Visualization and monitoring are performed using **Grafana**, which provides a SCADA-style interface to represent the system’s **Digital Twin**. The Digital Twin maintains real-time synchronization with the physical system, allowing operators to observe system behavior, detect anomalies, and remotely control the blower fan through a relay module.

Additionally, the system performs **trend analysis and Remaining Useful Life (RUL) estimation** in the cloud layer using Node-RED function nodes. This allows the system to provide predictive insights into potential motor degradation over time.

To ensure industrial-grade reliability, the system incorporates several **cybersecurity and reliability features**, including MQTT authentication, controlled topic access, Last Will and Testament (LWT) messages for device status monitoring, automatic reconnection mechanisms, and timestamped data logging.

Overall, this project demonstrates a complete **cyber-physical monitoring system integrating embedded hardware, industrial communication protocols, edge intelligence, and cloud analytics**. It highlights how modern industrial environments can transition from traditional reactive maintenance strategies to **predictive maintenance using Digital Twin technologies and Industrial IoT architectures**.

---

## How This Works (current implementation)

- Python edge simulator (in `python/`) generates realistic blower fan current readings, computes a short moving average, and performs anomaly detection using a pre-trained IsolationForest model (`fan_anomaly_model.pkl`).
- The Python edge publishes two MQTT topics:
  - `sensors/group20/hvac-blower/data` — periodic sensor payloads (timestamp, device, current, moving_avg, unit).
  - `alerts/group20/hvac-blower/status` — status messages (NORMAL / ANOMALY) with a human-readable message.
- An MQTT broker (Eclipse Mosquitto) runs as a Docker service (`mqtt` in `docker-compose.yml`) and relays messages to subscribers such as Node-RED for visualization or downstream processing.
- A `python-edge` Docker service builds and runs the Python publisher inside a container so the system can be launched with `docker-compose` for end-to-end testing.

## Repository structure (relevant files)

- `docker-compose.yml` — brings up `mqtt`, `node-red`, and `python-edge` services.
- `python/` — Python edge simulator and model artifacts:
  - `mqtt_publisher.py` — main publisher with CLI, logging and reconnect/backoff logic.
  - `fan_anomaly_model.pkl` — trained IsolationForest model (saved with scikit-learn 1.5.0).
  - `requirements.txt` — Python dependencies (pinned `scikit-learn==1.5.0`).
  - `Dockerfile` — image used by the `python-edge` service.
  - `test_model.py` and `tests/test_model_pytest.py` — smoke test and pytest unit test for the model.

## Quick start (Docker Compose)

1. Build and start the services (broker + publisher):

```powershell
docker-compose up -d --build mqtt python-edge
```

2. Check the publisher logs to observe sensor and alert messages:

```powershell
docker-compose logs --tail=200 --follow python-edge
```

3. Run the smoke test inside the running `python-edge` container:

```powershell
docker-compose exec python-edge python test_model.py
```

## Run the Python publisher locally (without Docker)

1. From the repository root, install dependencies:

```bash
pip install -r python/requirements.txt
```

2. Run the publisher for a limited number of iterations (useful for local development):

```bash
cd python
python mqtt_publisher.py --iterations 10 --log-level INFO
```

CLI options for `mqtt_publisher.py`:

- `--broker` (default `mqtt`) — MQTT host (use `localhost` when running outside compose).
- `--port` (default `1883`) — MQTT port.
- `--model-path` (default `fan_anomaly_model.pkl`) — path to the joblib model file.
- `--interval` (default `2.0`) — seconds between publishes.
- `--iterations` — integer limit for number of loop iterations (useful for tests).
- `--log-level` — `DEBUG|INFO|WARNING|ERROR`.

## Tests and CI

- Unit test: `python/tests/test_model_pytest.py` — run with pytest:

```bash
cd python
pytest -q tests
```

- A GitHub Actions workflow is provided at `.github/workflows/ci.yml` to run tests on push/PR.

## Notes & troubleshooting

- The saved model was trained and serialized with scikit-learn 1.5.0. We pin `scikit-learn==1.5.0` in `python/requirements.txt` to avoid unpickle compatibility warnings.
- If the publisher cannot connect to the MQTT host named `mqtt`, either ensure `docker-compose` is running (service name `mqtt`) or run locally and pass `--broker localhost`.
- The publisher implements an exponential backoff on connect attempts and graceful shutdown via SIGINT/SIGTERM.

## Next steps (suggestions)

- Add Node-RED flows (if not already present) to subscribe to the MQTT topics and visualize data in Grafana or a lightweight dashboard.
- Add automated tests for the publisher CLI and mock MQTT using a test broker (e.g., `paho-mqtt` test client or `hbmqtt` in tests).
- Extend the model to produce an anomaly score and threshold tuning / evaluation reports.
