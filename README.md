# C--: a swiss army knife for C++

Commands
---
- help (h)     - display this message
- compile (c)  - compile a given file and its dependencies
- run (r)      - compile the given file and run it
- debug (d)    - compile the given file and gdb it
- valgrind (v) - compile the given file and run valgrind

Usage: `c-- <command> <file> [options]`

See `c-- help` for more information.

Config
---
- A `.cmmrc`, `.cmmrc.yml`, `.c--rc`, or `.c--rc.yml` can be used to set default flags or scripts.
- A flag set in a config file or script will be overriden by any explicitly passed in by the command line.
- Example config in `.cmmrc.yml` in this repo.