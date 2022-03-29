# tml

`tml` (terminal markup language) is an HTML-like syntax for creating terminal UIs.

This repository provides a compiler that turns TML markup into C code that works on top of my terminal UI library [tuibox](https://github.com/Cubified/tuibox).

**Note**:  This project is currently a work in progress.

## Demo

Markup:

```
<ui>
  <screen>
    <box center width="30" height="15" background="red">
      <text>hello tml!</text>
    </box>
  </screen>
</ui>
```

Compiled output:

![Demo](https://github.com/Cubified/tml/blob/main/demo.png)

## Features

 - Declarative format for terminal UIs
 - Supports interactivity (e.g. mouse click and hover events)
 - Dependency-free (both the `tml` compiler and `tuibox` itself)
 - Familiar syntax coming from HTML/XML

## Motivation

A few months ago, I wrote `tuibox` because I felt myself re-writing nearly the same boilerplate every time I set out to create a terminal-based application ([vex](https://github.com/Cubified/vex), [3n2](https://github.com/Cubified/3n2), [malt](https://github.com/Cubified/malt), etc.). So, I abstracted this away to a few basic UI components (e.g. boxes, text, screens, etc.) and was very happy with the results.

However, working with `tuibox` directly can be cumbersome, simply given the amount of data any one UI element can store (for example, its `ui_add` function takes 13 arguments).  To mitigate this, I created `tml` because I find that representing a UI in an HTML-like format to be much more natural and easier to work with.

## Compiling and Running

To compile the `tml` compiler and run its tests:

     $ make
     $ ./test.sh

---

Then, to compile your own `tml` markup:

     $ ./tml [file.tml]

This produces `app.c` and `tml_events.h` in the current working directory, which can be compiled with:

     $ cc app.c -o app

Adding interactivity is done by editing `tml_events.h` -- refer to `tuibox`'s [demos](https://github.com/Cubified/tuibox/blob/main/demos/demo_drag.c) on to how best to do this.

## Documentation

Coming soon!

## To-Do

 - Demo image
 - Proper layout engine
 - Better error messages
 - More comprehensive tests
 - Documentation
