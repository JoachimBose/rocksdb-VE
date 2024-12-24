#!/usr/bin/python3
import re
import pprint
import seaborn as sns
import matplotlib.pyplot as plt
import pandas as pd

PERCENTILE = 0.999

def normalise(counts):
    total = sum(counts)
    if total == 0:
        return counts
    return list(map(lambda x : x / total, counts))

def findRange(counts):
    total = sum(counts)
    if total == 0:
        return 0,0

    i = 0
    while(sum(counts[:i]) < (total * (1-PERCENTILE))):
        print(f"{sum(counts[:i])} > {(total * (1-PERCENTILE))}")
        i += 1
    if(i > 0):
        i -= 1
    
    
    
    j = len(counts) - 1
    while(sum(counts[:j]) > (total * PERCENTILE)):
        j -= 1
    if(j < len(counts) - 1):
        j += 1
    
    print(f"here lies i: {i} here lies j: {j}")
    return i,j
        


def parse_histogram(data):
    """
    Parses the histogram data into ranges and frequencies.

    Args:
        data (str): The histogram string.

    Returns:
        dict: A dictionary with keys 'range' and 'frequency'.
    """
    histogram = {
        "range": [0],
        "frequency": [0],
        "middle": [0]
    }

    for line in data.splitlines():
        match = re.match(r"\[(\d+(?:\.\d+)?)([KMG]?)\s*,\s*(\d+(?:\.\d+)?)([KMG]?)\)\s+(\d+)", line)
        if match:
            # print(match.groups())
            start, start_unit, end, end_unit, frequency = match.groups()
            units = {"K": 1e3, "M": 1e6, "G": 1e9, "": 1}
            start_value = float(start) * units[start_unit]
            end_value = float(end) * units[end_unit]

            histogram["range"].append(f"{start}{start_unit}, {end}{end_unit}")
            histogram["middle"].append(start_value + ((end_value - start_value) / 2))
            histogram["frequency"].append(int(frequency))
    histogram["normalisedFrequency"] = normalise(histogram["frequency"])
    
    displ_start, displ_end = findRange(histogram["frequency"])
    
    if(displ_end != 0 and displ_end != 0 and len(histogram["middle"]) > 5):
        histogram["displayRange"] = (histogram["middle"][displ_start], histogram["middle"][displ_end])

    return histogram

# Example input data (replace this with your file content)
def getHistsFromFile(filePath):
    data =  ""
    hists = {}
    with open(filePath, "r") as f:
        data = f.read()

    # Example usage
    histograms = {}
    current_histogram = None
    for line in data.splitlines():
        if line.startswith("@"):  # New histogram section
            current_histogram = line.strip("@").strip(":")
            histograms[current_histogram] = ""
        elif current_histogram:
            histograms[current_histogram] += line + "\n"

    for title, hist_data in histograms.items():
        # print(f"{title} =====")
        parsed_histogram = parse_histogram(hist_data)
        if(len(parsed_histogram['range']) > 5):
            hists[title] = parsed_histogram
    return hists

filePaths = ["/home/joachim/Documents/GitHub/rocksdb-VE/dataparser/smalltrace/smalltrace-bpf-io_uring_nv.txt"]
names = [
    "io_uring", "libaio", "posix"
]

def plotHistoGramSet(set, name):
    displayRanges = list(map(lambda x: x["displayRange"], set))
    list(map(lambda x: x.pop("displayRange"), set))
    dfs = list(map(lambda x: pd.DataFrame(x), set))
    print(f"amount of hists found for this: {len(dfs)}")
    plt.figure()
    sns.set_theme()
    ax = None
    for i in range(0, len(dfs)):
        ax = sns.lineplot(x="middle", y="normalisedFrequency", data=dfs[i], alpha=0.5, label=names[i], markers=True)
    # plt.xticks(rotation=90)
    print(f"'{name}'")
    nameDict = {
        "deltaHistRD: ":"Time to get to rw_verify_area in reads",
        "deltaHistWR: ":"Time to get to rw_verify_area in writes",
        "deltaHistR: ":"Time to get from ext4_file_read_iter to userspace",
        "deltaHistW: ":"Time to get from ext4_file_write_iter to userspace",
    }
    if(name in nameDict):
        plt.title(nameDict[name])
    else:
        plt.title(name)
    plt.xlabel("time between captured events (ns)")
    plt.ylabel("Frequency")
    ax.set_xlim(min(map(lambda x: x[0], displayRanges)), max(map(lambda x: x[1], displayRanges)))
    plt.savefig(f"{name}-plot")
    plt.show()


hists = []
sortedHists = {}
for i in filePaths:
    currentFileHists = getHistsFromFile(i)
    for key in currentFileHists:
        if(not key in sortedHists):
            sortedHists[key] = []
        sortedHists[key].append(currentFileHists[key])

pprint.pprint(sortedHists)
for title in sortedHists:
    plotHistoGramSet(sortedHists[title], title)