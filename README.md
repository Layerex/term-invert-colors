# term-invert-colors

A simple program for inverting [color ANSI escape codes](https://en.wikipedia.org/wiki/ANSI_escape_code#Colors). I made this for finely viewing hardcoded colorschemes with light terminal background.

## Installation

First install [libansiescape](https://github.com/paultag/libansiescape/).

Then clone this repository and execute:

```sh
sudo make install
```

## Usage

```
Usage: term-invert-colors [-bg -fg -8 -8bg -8fg -b8 -b8bg -b8fg -16 -16bg -16fg -216 -216bg -216fg -grayscale -grayscalebg -grayscalefg -256 -256bg -256fg -rgb -rgbbg -rgbfg]
Options specify which colors to invert. By default all colors all inverted. bg and fg suffixes specify whether to invert only background or only foreground colors respectively.
-fg        All foreground colors.
-bg        All background colors.
-8         8 standart colors.
-8b        8 bright colors.
-16        All colors of 16-color palette. Equal to -8 -8b.
-216       216 colors of color cube of 256-color palette.
-grayscale 24 shades of gray of 256-color palette.
-256       All colors of 256-color palette. Equal to -16 -216 -grayscale.
-rgb       All rgb colors.
Take a look at https://en.wikipedia.org/wiki/ANSI_escape_code#Colors if in doubt.
```

## TODO

- Fix incorrect handling of ^[(B escape code (probably requires fix in library). As a consequence most ncurses applications work even more terribly when piped.
