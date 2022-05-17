# G--: a swiss army knife for C++

## Commands
- help (h)     - display this message
- compile (c)  - compile a given file and its dependencies
- run (r)      - compile the given file and run it
- debug (d)    - compile the given file and gdb it
- valgrind (v) - compile the given file and run valgrind
- gcov (g)     - compile the given tests file and perform code coverage analysis

Usage: `g-- <command> <file> [options]`

See `g-- help` for more information.

## Config
- A `.gmmrc`, `.gmmrc.yml`, `.g--rc`, or `.g--rc.yml` can be used to set default flags or scripts.
- A flag set in a config file or script will be overriden by any explicitly passed in by the command line.
- Example config in `.gmmrc.yml` in this repo.

## Compilation Instructions:
- Since `g--` uses threads for watching stuff, use the `-pthread` flag when compiling:
```
g++ -Wall -W -s -pedantic-errors *.cpp -o bin/g--
```
- To be able to run the executable from anywhere, be sure to add to your `PATH`.