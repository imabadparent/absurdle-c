# absurdle-c
A remake of the game Absurdle in C

## What it is

This is a clone of the game [Absurdle](https://qntm.org/absurdle), which itself
is a take on the game [Wordle](https://www.nytimes.com/games/wordle/index.html).
This is my second project in C, and was mainly made as a learning experience.

## How it works

Before reading my explanation, I highly recommend you read the [explanation
given by qntm](https://qntm.org/absurdle) about the official version. This
version works in the almost the exact same way, and I will only be describing
elements not covered in their explanation, or differences between their version
and mine.

Assuming you read qntm's explanation, you know that Absurdle works by dividing
the possible secret words into buckets based on the response that would be given
to the player for each word. It then selects the bucket with the most words in
it, until eventually there is only one possible word left. Sometimes, however
there will be two buckets of equal size that are both larger than every other
bucket. When this happens, my version attempts to select the bucket with the
response that provides the least amount of information to the player. This is
done by giving weights to each element in the response. For example, if the two
buckets had a response of "!!.!!" or ".?...", the second bucket would be
selected because it gives the player the least amount of useful information.
