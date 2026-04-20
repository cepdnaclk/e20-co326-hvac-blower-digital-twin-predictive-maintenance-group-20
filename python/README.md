# Python edge publisher

Quick instructions to run the MQTT publisher and test the anomaly model.

Run services with docker-compose:

```powershell
docker-compose up -d --build mqtt python-edge
```

Check logs:

```powershell
docker-compose logs --tail=200 python-edge
```

Run the model smoke test inside the running container:

```powershell
docker-compose exec python-edge python test_model.py
```

Run the publisher locally (without Docker):

```powershell
cd python
pip install -r requirements.txt
python mqtt_publisher.py --iterations 10
```
