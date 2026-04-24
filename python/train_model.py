import pandas as pd
from sklearn.svm import OneClassSVM
import joblib
from micromlgen import port
import os

df = pd.read_csv("normal_fan_data.csv")

X = df[["current", "moving_avg"]].values

model = OneClassSVM(nu=0.05, gamma=0.001)
model.fit(X)

joblib.dump(model, "fan_anomaly_model.pkl")
print("Saved fan_anomaly_model.pkl")

code = port(model)

output_path = os.path.join("..", "platformio", "include", "fan_anomaly_model.h")
os.makedirs(os.path.dirname(output_path), exist_ok=True)
with open(output_path, "w") as f:
    f.write(code)
print(f"Saved {output_path}")