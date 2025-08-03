# Embattled

[Try the Embattled Arena in your browser!](https://nwalker1208.github.io/embattled/arena.html)

Embattled is a simple robot simulator that allows users to program robots using an artificial assembly language.

The parser and assembler for the language as well as the simulated processor are all written in C. The simulated robot arena is mostly written in C and can be compiled to either a native executable or to web assembly. The web interface, of course, involves some HTML, CSS, and JavaScript.

This project uses several open source libraries and tools, including [Raylib](https://github.com/raysan5/raylib), [Unity Test](https://github.com/raysan5/raylib), [Emscripten](https://github.com/emscripten-core/emscripten), and [CodeMirror](https://github.com/codemirror/dev/).

Documentation of the assembly language/instruction set design can be found under [instruction-set.md](./instruction-set.md). Instructions on how to write programs in the language can be found on [a GitHub Pages site](https://nwalker1208.github.io/embattled/eal.html).

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
