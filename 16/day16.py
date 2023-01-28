import re
import sys
from dataclasses import dataclass
from collections import defaultdict
from pprint import pprint
from itertools import chain
from copy import deepcopy
import datetime

PART = 1

MAX_TIME = 30 if PART == 1 else 26

nodes: dict[str, int] = dict()
edges: dict[str, list[str]] = defaultdict(list)
valve_count: int = 0


class Map:
    def __init__(self, valves_names: list[str]) -> None:
        self.valves: dict[str, list[list[dict[str, int]] | None]] = {
            name: [None] * (MAX_TIME+1) for name in valves_names
        }
        self.valves["AA"][0] = []
        self.time: int = 0


    def explore(self) -> None:
        print("explore:", self.time)
        for valve_name, tl in self.valves.items():
            if tl[self.time] is None:
                continue
            
            for target_name in edges[valve_name]:
                self._move(valve_name, target_name)

            if nodes[valve_name]:
                self._open(valve_name)

        #pprint(self.valves)

        self.time += 1

    def _move(self, valve_name: str, target_name: str) -> None:
        t = self.time
        valves = self.valves

        valves[target_name][t+1] = merge(
            valves[target_name][t+1],
            valves[valve_name][t],
        )

    def _open(self, valve_name: str) -> None:
        t = self.time
        valves = self.valves

        new_combos: list[dict[str, int]] = []
        for combo in valves[valve_name][t]:
            new_combos.append(combo | {valve_name: t+1})

        new_combo = {valve_name: t+1}
        for combo in new_combos:
            if combo.keys() == new_combo.keys():
                break
        else:
            new_combos.append(new_combo)

        valves[valve_name][t+1] = merge(
            valves[valve_name][t+1],
            new_combos,
        )

    def iter_combos(self, time: int):
        for tl in self.valves.values():
            if tl[time] is None:
                continue
            for combo in tl[time]:
                yield combo

    def get_max_pressure(self, time: int) -> int:
        rv: int = 0
        count = 0
        for combo in self.iter_combos(time):
            count += 1
            if (v := compute(combo)) > rv:
                rv = v
        print("count", count)
        return rv

    def get_max_pressure2(self, time: int) -> int:
        combos = {
            tuple(combo.items())
            for combo in self.iter_combos(time)
        }
        combos = merge(
            [],
            [dict(combo) for combo in combos],
        )

        print("count", len(combos))
        rv: int = 0
        for combo1 in combos:
            for combo2 in combos:
                if len(set(combo1.keys()) & set(combo2.keys())) == 0:
                    if (v := (compute(combo1) + compute(combo2))) > rv:
                        rv = v
        return rv



def merge(old_combos: list[dict[str, int]] | None, new_combos: list[dict[str, int]] | None) -> list[dict[str, int]]:
    if old_combos is None:
        return new_combos

    rv: list[dict[str, int]] = []
    for test_combo in chain(old_combos, new_combos):
        append = True
        for i, combo in enumerate(rv):

            if combo.keys() == test_combo.keys():
                if compute(test_combo) > compute(combo):
                    rv[i] = test_combo
                append = False
                break

        if append:
            rv.append(test_combo)

    return rv


def compute(combo: dict[str, int]) -> int:
    total: int = 0
    for name, time in combo.items():
        total += nodes[name] * (MAX_TIME - time)
    return total


def parse_file(path: str) -> None:
    global nodes, edges, valve_count
    with open(path) as fh:
        for line in fh.readlines():
            line = line.strip()
            m = re.fullmatch(r"Valve ([A-Z]{2}) has flow rate=(\d+); tunnels? leads? to valves? ([A-Z]{2}(?:, [A-Z]{2})*)", line)
            nodes[m.group(1)] = int(m.group(2))
            if int(m.group(2)):
                valve_count += 1
            for target in m.group(3).split(", "):
                edges[m.group(1)].append(target)



def main():
    parse_file(sys.argv[1])
    pprint(nodes)
    pprint(edges)
    print(f"valves: {valve_count}")

    map_ = Map(list(nodes.keys()))
    for i in range(MAX_TIME):
        map_.explore()
        if PART == 1:
            print(map_.get_max_pressure(i + 1))
        else:
            print(map_.get_max_pressure2(i + 1))


def test_merge():
    parse_file(sys.argv[1])
    ans = merge(
        [{'BB': 1}, {'DD': 1}],
        [{'BB': 3}],
    )
    assert ans == [{'BB': 1}, {'DD': 1}], ans

    ans = merge(
        [{'BB': 1, 'DD': 1}],
        [{'BB': 3}],
    )
    assert ans == [{'BB': 1, 'DD': 1}], ans


def test_compute():
    parse_file(sys.argv[1])
    assert compute(dict(BB=2)) == 364


if __name__ == "__main__":
    main()
    #test_merge()
    #test_compute()

