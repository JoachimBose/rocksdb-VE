#!/usr/bin/python3

import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd

# Data for the table
data = {
    "enginetype": ["io_uring", "libaio", "posix", "io_uring_sqp"],
    "get": [36.80, 32.57, 33.86, 72.78],
    "put": [74.21, 13.42, 12.38, 75.78],
}

# Convert data to DataFrame
df = pd.DataFrame(data)

# Melt the DataFrame to a long format suitable for seaborn
df_melted = pd.melt(df, id_vars="enginetype", var_name="operation", value_name="value")

# Create a bar chart using seaborn
plt.figure(figsize=(4, 3))
sns.barplot(data=df_melted, x="enginetype", y="value", hue="operation", palette="mako")

# Add titles and labels
plt.title("get & put P99 latency for different engines")
plt.xlabel("Engine Type")
plt.ylabel("Latency (micro seconds)")
plt.legend(title="Operation")

# Show the plot
plt.tight_layout()
plt.savefig("latency-p99.pdf")
plt.show()
