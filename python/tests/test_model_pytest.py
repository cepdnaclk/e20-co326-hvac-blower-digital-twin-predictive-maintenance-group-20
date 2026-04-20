import joblib
import pandas as pd


def test_model_predictions():
    model = joblib.load("fan_anomaly_model.pkl")
    normal = pd.DataFrame([{"current": 3.0, "moving_avg": 3.0}])
    anomaly = pd.DataFrame([{"current": 6.0, "moving_avg": 3.0}])

    pred_n = model.predict(normal)[0]
    pred_a = model.predict(anomaly)[0]

    assert pred_n == 1
    assert pred_a == -1
