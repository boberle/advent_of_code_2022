import sys
from dataclasses import dataclass

MUL = 811589153
TURNS = 10


@dataclass
class Number:
    value: int = None
    original_index: int = None


def load_file(path):
    numbers = []
    with open(path) as fh:
        for i, line in enumerate(fh.readlines()):
            line = line.strip()
            numbers.append(Number(value=int(line), original_index=i))
    return numbers


def swap_right(numbers, start, n):
    for i in range(n):
        if start+1 < len(numbers):
            numbers[start], numbers[start+1] = numbers[start+1], numbers[start]
            start += 1
        else:
            numbers[start], numbers[0] = numbers[0], numbers[start]
            start = 0
    return numbers


def swap_left(numbers, start, n):
    l = len(numbers)
    for i in range(n):
        if start == 0:
            numbers[start], numbers[l-1] = numbers[l-1], numbers[start]
            start = l - 1
        else:
            numbers[start], numbers[start-1] = numbers[start-1], numbers[start]
            start -= 1
    return numbers


def get_number_at_original_index(numbers, index):
    for i in range(len(numbers)):
        if numbers[i].original_index == index:
            return i
    assert False


def mix(numbers):
    for i in range(len(numbers)):
        current_index = get_number_at_original_index(numbers, i)
        number = numbers[current_index]
        value = number.value
        if value > 0:
            f = swap_right
        else:
            f = swap_left
        f(numbers, current_index, abs(value) % (len(numbers)-1))


def get_coordinates(numbers):
    for i, number in enumerate(numbers):
        if number.value == 0:
            break
    a = (i + 1000) % len(numbers)
    b = (i + 2000) % len(numbers)
    c = (i + 3000) % len(numbers)
    return numbers[a].value + numbers[b].value + numbers[c].value


def main():
    numbers1 = load_file(sys.argv[1])
    numbers2 = [Number(n.value * MUL, n.original_index) for n in numbers1]

    mix(numbers1)
    print(f"coordinates (part 1): {get_coordinates(numbers1)}")

    for i in range(TURNS):
        mix(numbers2)
    print(f"coordinates (part 2): {get_coordinates(numbers2)}")


if __name__ == "__main__":
    main()
