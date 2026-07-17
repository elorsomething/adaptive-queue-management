import traceback

try:
    import joblib
    import pandas as pd
    import sys
    import os

    model_path = os.path.join(
        os.path.dirname(__file__),
        "models",
        "best_rf_model.pkl"
    )

    model = joblib.load(model_path)

    queue_size = float(sys.argv[1])
    queue_occupancy = float(sys.argv[2])
    drop_probability = float(sys.argv[3])
    arrival_rate = float(sys.argv[4])
    service_rate = float(sys.argv[5])

    sample = pd.DataFrame({
        "QueueSize": [queue_size],
        "QueueOccupancy": [queue_occupancy],
        "DropProbability": [drop_probability],
        "ArrivalRate": [arrival_rate],
        "ServiceRate": [service_rate]
    })

    prediction = model.predict(sample)

    print(int(prediction[0]))

except Exception:
    traceback.print_exc()