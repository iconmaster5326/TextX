# TextX

TextX is a simple terminal text editor written in C++, designed to make use of features and controls commonly seen in modern GUI text editors, while making full use of its text-based environment.

## Features

TextX is still under development. This means many features are currently lacking, but the features we do have include:

* __Obvious to use!__ TextX controls just like every other text editor you're used to. Control-C copies, and Control-V pastes, for example! Imagine that! TextX also lets you discover new options though the menu bar, and lets you use the mouse just about everywhere.
* __Stupidly portable!__ Any platform that has C++ and libncurses can support TextX.
* __No installation needed!__ Just build it and do what you will. Put TextX on portable media and use it anywhere! (Anywhere that's the correct platform you built TextX for, of course).

## Compiling

To build TextX, you will need:

* A gcc-compatible C++ compiler that supports C++ 2003
* [ncurses](https://invisible-island.net/ncurses/)
    * Ubuntu: `sudo apt-get install libncurses-dev`
    * Cygwin: `apt-cyg install libncurses-devel`

Just pull in this repository, and on the command line:

```
make
```

An executable called `textx` (or `textx.exe`) should now be available in the current directory.

## Usage

* Press `^Q` to quit at any time.
* Press `F1`, `F2`, etc. to access the menu bar at the top (or just click on it).
* Double-press `ESC` to exit out of any menu.
* Press `Alt-Left` and `Alt-Right` to quickly switch between tabs.
* `^S`, `^X`, `^C`, and `^V` should work like you're used to.

## Troubleshooting

Most bugs are caused by `$TERM`. Ensure you have the correct value of `$TERM` for the terminal you're trying to use. Note that not all terminals support all features (such as color, many key combinations, and mouse support). Call `make key_tester && ./key_tester` to see if full keyboard and mouse support exists for your value of `$TERM`.

If the bug is not caused by a bad terminal or terminfo file, feel free to use our [issue tracker](https://github.com/iconmaster5326/TextX/issues).

## More coming soon!
