# Absurdle-C
A remake of the game Absurdle written in ANSI C (C89/C90)

## How to install

If you are on Arch Linux, you can download [PKGBUILD](packs/arch/PKGBUILD), and
run the following command:
```sh
makepkg -sci
```
Feel free to submit this to the AUR, but I likely won't.

If you are on another version of Linux, or would rather build the program
yourself, you can use git and cmake to build it with the following commands:
```sh
git clone https://github.com/imabadparent/absurdle-c.git
cd absurdle-c
cmake -B build
cmake --build build
```
followed by this command run as root
```sh
cmake --install build
```
(To clone the dev branch instead, use 
``git clone -b dev https://github.com/imabadparent/absurdle-c.git``)

This will install the binary to ``/usr/local/bin`` and install the wordlists to
``/usr/local/share/absurlde-c``.

This works on Linux and macOS. I have not tested it on Windows.

## How to play

The game starts by asking the player to guess a word. The player resonds by
inputing a 5-letter word. This word is then analysed and a response is given
(for more info on the analysis, see [How it works](#How-it-works)). The
response will contain a mixture of '``.``', '``?``', and '``!``'. A '``.``'
indicates that the corresponding letter in the guessed word is not contained in
the solution (This is as if the letter were greyed out in Wordle). A '``?``'
indicates that the corresponding letter is contained in the word, but not in the
correct position (This is as if the letter were yellow in Wordle). A '``!``'
indicates that the corresponding letter is contained in the word and in the
correct position (This is as if the letter were green in Wordle).

TL;DR for Wordle players: '``.``' is grey, '``?``' is yellow, and "``!``" is
green.

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
buckets had a response of "``!!.!!``" or "``.?...``", the second bucket would be
selected because it gives the player the least amount of useful information.
