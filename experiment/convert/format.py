#format the graph as the input graph for the experiments.

filepath = "../graph/facebook-snap.txt"
filepath2 = "../graph/facebook.txt"
haszero = True

edgeset = set()
n = 0
with open(filepath) as fp:
    cnt = 0
    for line in fp:
        #print("line {} contents {}".format(cnt, line))
        a = line.split(' ')
        v0 = int(a[0])
        v1 = int(a[1])
        
        if haszero: v0+=1; v1+=1

        if v0 == v1: continue
        if v0 > v1: v1, v0 = v0, v1
        if v1 > n: n = v1
        #edge = str(v0) + ' ' + str(v1) + '\n'
        #print(edge)
        edgeset.add((v0, v1))
        cnt+=1
        #if cnt > 1000: break
        if cnt % 100000 is 0: print(cnt)

n += 1
m = len(edgeset)

def func(e):
    return e[0]

edgeset = sorted(edgeset,  key = func)
fp = open(filepath2, "w+")
head = str(n) + ' ' + str(m) + '\n'
fp.write(head)
for e in edgeset:
    fp.write(str(e[0]) + ' ' + str(e[1]) + '\n')
fp.close()





