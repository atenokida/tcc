columns_config = {
    "age": {
        "type": "numeric",
        "params": {
            "min": 18,
            "max": 65,
            "distribution": "zipf",
            "mode": "discrete",
            "skewness": 1.1,
        },
    },
    "salary": {
        "type": "numeric",
        "params": {
            "min": 30000,
            "max": 120000,
            "distribution": "normal",
            "mean": 75000,
            "std": 15000,
            "mode": "continuous",
        },
    },
    "department": {
        "type": "categorical",
        "params": {
            "num_categories": 5,
            # If the distribution is normal, the frequencies will be generated based
            # on the positions of the categories array.
            # e.g., ['val1', 'val2', 'val3'] where val2 is the peak of the curve
            "categories": ["HR", "Engineering", "Marketing", "Sales", "Finance"],
            "distribution": "normal",
            "std": 1,
        },
    },
}
