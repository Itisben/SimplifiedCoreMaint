# this is for drawing the experiment figure by Matplot
import sys
import re
import os.path
import time
import datetime
import csv
import numpy as np
import scipy.stats
import scipy as sp
import re
from tabulate import tabulate

path = "/home/guob15/git-code/core-maintenance/experiment/results/"
#path = "/mnt/d/git-code/scc-trim-v3/experiments/results/"
files = [
    "results-8-20-2021-hp.csv"
]

graph_name = [
    "dbpedia",
    "baidu",
    "livej",
    "patent", 
    "pokec",
    "wiki_talk_en",
    "wikitalk",
    "wiki-links",
    "ssca2",
    
    # "com-friendster",
    # "twitter",
    # "twitter_mpi",

    "er-1m-8m",
    "ba-1m-8m",
    "rmat-1m-8m"
]

graph_title = [
    "dbpedia",
    "baidu",
    "livej",
    "patent", "pokec",
    "wiki-talk-en",
    "wikitalk",
    "wiki-links",
    "ssca2",
    
    # "com-friendster",
    # "twitter",
    # "twitter-mpi",

    "ER",
    "BA",
    "RMAT",
]

all_graph_name = set() # load all graphs

algs = ["2", "3"] # Order-Based, our Simplified Order-Based, 
workers = [1] #[1, 2, 4, 6, 8, 10, 12, 14, 16, 32]
data = [] # load all data in csv files

def mean_confidence_interval(data, confidence=0.95):
    a = 1.0*np.array(data)
    n = len(a)
    m, se = np.mean(a), scipy.stats.sem(a)
    h = se * sp.stats.t._ppf((1+confidence)/2., n-1)
    return round(m, 2), round(h, 2), round(h, 2)
def mean_min_max(data):
    mean_time = np.mean(data)
    min_time = np.min(data)
    max_time = np.max(data)
    return (mean_time, round(max_time-mean_time,2), round(mean_time-min_time,2))
    
################################################################
def parse_data_time(model, worker, alg):
    global data
    mstimes = []
    for row in data:
        if model != row["model"]: continue
        if str(worker) != row["workers"]: continue
        if alg != row["alg"]: continue
        mstimes.append(float(row["mstime"]))
    if len(mstimes) != 0 : 
        return mean_confidence_interval(mstimes)

#csv table for running time
def output_csvtable_time(isall):
    global all_graph_name, graph_name
    #header with plus and minus
    head="worker,AC3Trim,plus1,minus1,AC4Trim,plus2,minus2,AC6Trim,plus3,minus3"
    if isall: graphs = all_graph_name
    else: graphs = graph_name
    for model in graphs:
        print ("\\begin{filecontents*}{"+model+".csv}")
        print (head)
        for w in workers:
            line = [str(w)]
            for alg in algs:
                result = parse_data_time(model,w, alg)
                for item in result: line.append(str(item))
            print (",".join(line))
        print ("\\end{filecontents*}")
        print ("")

# 1 worker vs 16 worker (AC3Trim, AC4Trim, AC6Trim), AC3Trim vs AC6Trim, AC4 vs AC6Trim
def output_table_time_speedup():
    global graph_name, graph_title, algs
    for model, title in zip(graph_name, graph_title):
        line = [title]
        
        r11 = parse_data_time(model, 1, "trim")[0]
        r12 = parse_data_time(model, 1, "ac4trim")[0]
        r13 = parse_data_time(model, 1, "fasttrim")[0]

        r1 = parse_data_time(model, 16, "trim")[0]
        r2 = parse_data_time(model, 16, "ac4trim")[0]
        r3 = parse_data_time(model, 16, "fasttrim")[0]

        speedup = '{:.2f}'.format(r11/r1); line.append(str(speedup))
        speedup = '{:.2f}'.format(r12/r2); line.append(str(speedup))
        speedup = '{:.2f}'.format(r13/r3); line.append(str(speedup))

        speedup = '{:.2f}'.format(r1/r3); line.append(str(speedup))
        speedup = '{:.2f}'.format(r2/r3); line.append(str(speedup))
        line[-1] += " \\\\"
        print("\t& ".join(line))
    print("")

######################################################################################################  
#return the number of traversed edges. 
def parse_data_edge_num(model, worker, alg):
    global data
    nums = []
    for row in data:
        if model != row["model"]: continue
        if str(worker) != row["workers"]: continue
        if alg != row["alg"]: continue
        n = re.search(r"\d+", row["traveledge"]).group()
        nums.append(int(n))
    if len(nums) != 0 : 
        return mean_confidence_interval(nums)

def parse_data_m(model):
    global data
    for row in data:
        if model != row["model"]: continue
        return row["M"]

def output_csvtable_edge_num(isall):
    global all_graph_name, graph_name
    #header with plus and minus
    head="worker,AC3Trim,plus1,minus1,AC4Trim,plus2,minus2,AC6Trim,plus3,minus3,M,AC4TrimAll"
    if isall: graphs = all_graph_name
    else: graphs = graph_name
    for model in graphs:
        print ("\\begin{filecontents*}{"+model+"-.csv}") #to make the csv file name different by adding "-"
        print (head)
        for w in workers:
            line = [str(w)]
            for alg in algs:
                result = parse_data_edge_num(model,w, alg)
                for item in result: line.append(str(item))
            m = parse_data_m(model)
            line.append(m)
            ac4all = float(line[4]) + float(m)/w
            line.append(str(ac4all))
            print (",".join(line))
        print ("\\end{filecontents*}")
        print ("")

# 1 worker vs 16 worker (AC3Trim, AC4Trim, AC6Trim), AC3Trim vs AC6Trim, AC4 vs AC6Trim
def output_table_edge_num_speedup():
    global graph_name, graph_title, algs
    for model, title in zip(graph_name, graph_title):
        line = [title]
        
        r11 = parse_data_edge_num(model, 1, "trim")[0]
        r12 = parse_data_edge_num(model, 1, "ac4trim")[0]
        r12 = r12 + float(parse_data_m(model))
        r13 = parse_data_edge_num(model, 1, "fasttrim")[0]

        r1 = parse_data_edge_num(model, 16, "trim")[0]
        r2 = parse_data_edge_num(model, 16, "ac4trim")[0]
        r2 = r2 + float(parse_data_m(model))/16
        r3 = parse_data_edge_num(model, 16, "fasttrim")[0]

        speedup = '{:.2f}'.format(r11/r1); line.append(str(speedup))
        speedup = '{:.2f}'.format(r12/r2); line.append(str(speedup))
        speedup = '{:.2f}'.format(r13/r3); line.append(str(speedup))

        speedup = '{:.2f}'.format(r1/r3); line.append(str(speedup))
        speedup = '{:.2f}'.format(r2/r3); line.append(str(speedup))
        line[-1] += " \\\\"
        print("\t& ".join(line))
    print("")

#for the first experiment
def parse_data_time(model, worker, alg, isInsert):
    global data
    mstimes = []
    for row in data:
        if model != row["model"]: continue
        if str(worker) != row["workers"]: continue
        if alg != row["alg"]: continue
        if isInsert and "0" != row["insert-num"]: 
            mstimes.append(float(row["mstime"]))
        if (not isInsert) and "0" != row["remove-num"]:
            mstimes.append(float(row["mstime"]))
    if len(mstimes) != 0 : 
        return mean_confidence_interval(mstimes)
    else: assert("data not find!")

    
def draw_speedup_table():
    global graph_name, graph_title, algs
    lines = []
    for model, title in zip(graph_name, graph_title):
        line = [title]
        r2 = parse_data_time(model, 1, "2", True)[0]
        r3 = parse_data_time(model, 1, "3", True)[0] #ours insert
        speedup = r2/r3
        line.append(str(round(speedup,2)))

        r2 = parse_data_time(model, 1, "2", False)[0]
        r3 = parse_data_time(model, 1, "3", False)[0] #ours remove
        speedup = r2/r3
        line.append(str(round(speedup,2)))

        lines.append(line)
    print(tabulate(lines))

#def draw_speedup_table_matplot(lines):
    

#######################################################################################
# read the multi-file to the data
def read_data():
    global data
    for p in files:
        p = path + p
        f = open(p,'rt')
        reader = csv.DictReader(f)
        for row in reader:
            all_graph_name.add(row["model"])
            data.append(row)
        f.close()

#######################################################################################
def main():
    read_data()
    print("")
    print("")
    
    draw_speedup_table()

    #output_csvtable_edge_num(True)
    #output_csvtable_time(True)
    #output_csvtable_parstep_time(True)


    #output_csv_table_max_push()
    #output_table_max_push()

    # output_table_edge_num_speedup()
    # output_table_time_speedup()
if __name__ == "__main__":
    main()
