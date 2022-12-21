# Advent of Code 2022 - Day 19

First and second parts: `day19.c`.

Solutions to test file: 33 (first) and 56 * 62 (second, there isn't really an answer to the second part of the test because you are supposed to multiply the first 3 blueprints, but the test set as only 2).

For this one, I have computed a tree with all the possibilities, but in order to keep things running fast (under 20 seconds combined for the first and second parts), I have pruned it with some magic numbers (found by observing the test case provided in the statement).  The observation is just this one: you create ore and clay-collecting robots only at the beginning (before the 15th and 20th minutes) and after the 20th minute, you create an obsidian collecting robot or a geode cracking robot as soon as you can. So these are the three magic number (or "hyperparameters") I have introduced.
