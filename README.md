# Embattled

[Try the Embattled Arena in your browser!](https://nwalker1208.github.io/embattled/arena.html)

Embattled is a simple robot simulator that allows users to program robots using an artificial assembly language.

This is a project that I worked on in my spare time after work and on the weekends. At the time, my knowledge of assembly languages and related concepts was fairly limited. I had some exposure to these topics in university, and while this project did involve some research into existing instruction sets and terminology, my goal was less so to perfectly emulate existing real-world systems and more so to demonstrate competency in working with low-level programming languages such as C. As such, there is a decent chance that this project uses certain technical terms or phrases incorrectly. Hopefully such issues don't distract too much from the project as a whole, but if there are any serious errors, I would gladly accept feedback via a GitHub issue!

The parser and assembler for the language as well as the simulated processor are all written in C. The simulated robot arena is mostly written in C and can be compiled to either a native executable or to web assembly. The web interface, of course, involves some HTML, CSS, and JavaScript.

This project uses several open source libraries and tools, including [Raylib](https://github.com/raysan5/raylib), [Unity Test](https://github.com/raysan5/raylib), [Emscripten](https://github.com/emscripten-core/emscripten), and [CodeMirror](https://github.com/codemirror/dev/).

More details about the Embattled simulator and how to write programs for Embattled robots can be found on the following GitHub Pages site: https://nwalker1208.github.io/embattled/guide.html

## Requirements to build the project locally

- CMake
- Emscripten (for web build)
- Ruby (for regenerating test runners)

## Building for desktop

```sh
cmake -S . -B ./build
cmake --build ./build
```

## Building for web

```sh
emcmake cmake -S . -B ./build -DPLATFORM=Web
cmake --build ./build
```

> Note: At the moment, an issue with Emscripten (or with the way I'm using it) causes backspaces and tabs to be ignored by anything other than the canvas. To fix this, the following code needs to be replaced in the `arena.js` file that gets built:
> ```js
> if(event.key=="Backspace"||event.key=="Tab"){event.preventDefault()}
> ```
> Replace the above with:
> ```js
> if(event.key=="Backspace"||event.key=="Tab"){}
> ```
> (This fix is based on [this Stack Overflow answer](https://stackoverflow.com/a/62914145).)

TODO: Setup GitHub action to run web build on push.

## Running tests

```sh
cmake --build ./build --target test
```

## Regenerating test runners

TODO: Integrate generate_test_runner into CMake setup.

```sh
ruby ./build/_deps/unity-src/auto/generate_test_runner.rb ./processor/tests/process_tests.c ./processor/tests/process_tests_Runner.c --use_param_tests=1
ruby ./build/_deps/unity-src/auto/generate_test_runner.rb ./processor/tests/instruction_tests.c ./processor/tests/instruction_tests_Runner.c --use_param_tests=1
```

<!-- Note: MSVC ins't quite compatible with Unity's parameterized tests. -->
