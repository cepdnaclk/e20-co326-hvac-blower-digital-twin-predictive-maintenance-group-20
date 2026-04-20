import joblib
import pandas as pd
import sys

model = joblib.load("fan_anomaly_model.pkl")

# Normal sample (should be predicted as inlier -> 1)
normal = pd.DataFrame([{"current": 3.0, "moving_avg": 3.0}])
# Anomalous sample (large current)
anomaly = pd.DataFrame([{"current": 6.0, "moving_avg": 3.0}])

pred_n = model.predict(normal)[0]
pred_a = model.predict(anomaly)[0]

print("normal prediction:", pred_n)
print("anomaly prediction:", pred_a)

if pred_n != 1 or pred_a != -1:
    print("Model behavior unexpected")
    sys.exit(2)
else:
    print("Model test passed")
    sys.exit(0)
