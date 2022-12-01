import sys
from functools import reduce

def f(acc, item):
    item = item.strip()
    if item:
        acc[-1] += int(item)
    else:
        acc.append(0)
    return acc

path = sys.argv[1]
print("first part:", max(reduce(f, open(path), [0])))
print("second part:", sum(sorted(reduce(f, open(path), [0]))[-3:]))
