import joblib
try:
    import m2cgen as m2c
except ImportError:
    pass
model = joblib.load('fan_anomaly_model.pkl')
try:
    print('Testing m2cgen...')
    code = m2c.export_to_c(model)
    print("m2cgen success!", code[:100])
except Exception as e:
    print("m2cgen error:", e)

try:
    print('\nTesting micromlgen...')
    from micromlgen import port
    code = port(model)
    print("micromlgen success!", code[:100])
except Exception as e:
    print("micromlgen error:", e)
