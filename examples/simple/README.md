# FizzBuzz Example
## Objectives:
- Learn how to compile and run with g--
- Learn how to pass arguments into compiled executable

## Tutorial:
- To compile the program, use the `compile` command:
```
g-- compile main.cpp
```
- Run the following command to run the executable:
```
./bin/main
```
- You should see the first 100 iterations of the FizzBuzz program.
- By default, `g--` takes the main file's name and strips the `.cpp` to generate the executable name. You should also notice that `g--` has created a `bin` folder to house the executable.
- You can override this behavior by using the `--folder` and `--output` flags.
	- The `--folder` (or `-f` for short) flag tells `g--` the folder in which to place the compiled executable. By default, this is `bin`.
	- The `--output` (or `-o` for short) flag tells `g--` the name of the executable to generate, as a path relative to the output folder.
- Thus, the following command will place the executable in the current folder, with the name "program":
```
g-- compile main.cpp -f . -o program
```
- Do note that you can also shorten the `compile` command to just `c`:
```
g-- c main.cpp
```
- or omit the `.cpp`:
```
g-- c main
```
- To both compile and run your program, you can use the `run` command:
```
g-- run main.cpp
```
- Once again, you should get the first 100 iterations of the FizzBuzz program.
- As with the `compile` command, `run` can be shortened to `r`.
- With the `run` command, you can also pass in arguments to the compiled executable with the `--args` flag (or `-a` for short). The `-a` flag will collect all following arguments until it runs into another flag, and pass these in as arguments to the executable:
```
g-- run main.cpp -a Fuzz Bizz
```
- Running the above command should produce the first 100 iterations of FizzBuzz, except "Fizz" is replaced with "Fuzz" and "Buzz" is replaced with "Bizz".
- If you want to pass in a argument that also happens to be a `g--` argument, you can explicitly specify the argument string like so:
```
g-- run main.cpp -a="-o"
```
- The above command should produce the first 100 iterations of FizzBuzz, except the "Fizz" is replaced with "-o"