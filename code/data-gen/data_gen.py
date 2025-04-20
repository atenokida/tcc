import pandas as pd

from col_gen import (
    generate_group_column,
    generate_numeric_column,
    generate_categorical_column,
)
from faker import Faker
from configs import columns_config


def generate_dataframe(
    group_spec: dict, generator: callable, columns: dict
) -> pd.DataFrame:
    if callable(generator):
        sample_generator = generator
    else:
        raise ValueError("col_type must be a callable that returns values.")

    group_values = generate_group_column(
        group_spec=group_spec, sample_generator=sample_generator, shuffle=True
    )
    total_rows = len(group_values)
    
    numeric_columns = {
        col_name: generate_numeric_column(rows=total_rows, col_params=value["params"])
        for col_name, value in columns.items()
        if value["type"] == "numeric"
    }
    categorical_columns = {
        col_name: generate_categorical_column(
            rows=total_rows, col_params=value["params"]
        )
        for col_name, value in columns.items()
        if value["type"] == "categorical"
    }

    data = {
        "group_column": group_values,
        **numeric_columns,
        **categorical_columns,
    }

    return pd.DataFrame(data)


if __name__ == "__main__":
    fake = Faker()
    group_sizes_distribution = {1: 100, 2: 100, 3: 100, 4: 100, 5: 100, 6: 100, 7: 100}

    df = generate_dataframe(
        generator=lambda: fake.city(),
        group_spec=group_sizes_distribution,
        columns=columns_config,
    )

    df.to_csv("./tcc/data/data.csv", index=False)
