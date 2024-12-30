#!/usr/bin/python3
import os
import re
import matplotlib.pyplot as plt
from matplotlib.colors import ListedColormap
import seaborn as sns
from pprint import pprint
import numpy as np
import pandas as pd

# Set the input folder containing the files
input_folder = "/home/joachim/Documents/GitHub/rocksdb-VE/dataparser/smalltrace/"  # Replace with your folder path
output_folder = "/home/joachim/Documents/GitHub/rocksdb-VE/dataparser/stackplots/"  # Folder to save the plots
os.makedirs(output_folder, exist_ok=True)

label_to_exp = {'back_in_uspace': 'from kspace to uspace',
    'xfs_write_end': 'xfs write time',
    'ext4_write_end': 'ext4 write time',
    'xfs_write_starts': 'write preparation',
    'ext4_write_starts': 'write preparation',
    'sys_enter': 'from uspace to kspace',
    'cleanup_done': 'sqe cleanup',
    'finished_wait': 'app thread synchronisation',
    'io_sqe_processing': 'sqe processing',
    'woke_up': 'io worker synchronisation',
}
x_names = {
    'smalltrace-bpf-io_uring_nv.txt' : 'io_uring',
    'smalltrace-bpf-posix.txt' : 'posix'
}


# Regex pattern to match stats lines
stats_pattern = re.compile(r"@(\w+)_Stats:.*average\s+(\d+),")

# Function to extract averages and names
def extract_averages(file_path):
    averages = []
    with open(file_path, 'r') as file:
        for line in file:
            match = stats_pattern.search(line)
            if match:
                name, average = match.groups()
                averages.append((name, int(average)))
    return averages


# Generate stackplot for each file

allNames = []
stackPlots = {}

for file_name in os.listdir(input_folder):
    file_path = os.path.join(input_folder, file_name)
    if not os.path.isfile(file_path):
        continue

    # Extract averages and names
    data = extract_averages(file_path)
    if not data:
        continue

    names, averages = zip(*data)

    allNames.extend( list(names))
    
    nameAvgMapping = dict(zip(names, averages))
    stackPlots[x_names[file_name]] = nameAvgMapping

allNames = np.unique(allNames)

colors = sns.color_palette("viridis", len(allNames))
colorMapping = dict(zip(allNames, colors))

df = pd.DataFrame(stackPlots)
# Plot stackplot
for file_name in stackPlots:    
    # plt.bar(0, averages, alpha=0.8, color=sns.color_palette("husl", len(averages)), bottom=)
    nameAvgMapping = stackPlots[file_name]
    bottom = 0
    x = file_name
    
    idx = 0
sns.set_style("whitegrid")
my_cmap = ListedColormap(sns.color_palette("mako_r",10).as_hex())
pprint(df)
df.transpose().plot(kind="bar", stacked=True, figsize=(6,2.5), colormap=my_cmap)
    # for name in nameAvgMapping:
    #     avg = nameAvgMapping[name]
    #     plt.bar(x, avg, bottom=bottom, color=colorMapping[name], label= name)
    #     bottom += avg
    #     idx += 1
handles, labels = plt.gca().get_legend_handles_labels()

pprint(labels)
plt.legend(handles[::-1], list(map(lambda x : label_to_exp[x], labels[::-1])), bbox_to_anchor=(1.05, 1.0), loc='upper left')
plt.tight_layout(rect=[.02, -0.1, 1, 0.95])
# plt.legend(labels, loc="upper left", fontsize=10)

# Customize plot
plt.title(f"File write latency posix vs io_uring")
plt.ylabel("time ns")
plt.xticks(rotation=0)
# tick_labels = plt.axes().get_xticklabels()
# pprint(list(map(lambda x : x.get_text(), tick_labels)))
# plt.axes().set_xticklabels(list(map(lambda x : x_names[x.get_text()],tick_labels)))

# Save plot
plt.savefig("latency_analysis.pdf")
plt.show()
plt.close()

