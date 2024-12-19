This is a demonstration package for GNU Automake.
The original files are:
- In src:
    + main.cc
    + Makefile.am
- configure.ac
- Makefile.am
- autotools.sh
- .gitignore
- README <br>

You can run file `autotools.sh` to automatically build SW. <br>
Or type in these commands in order to build the demo <br>
1. `autoreconf -i`
2. `./configure` 
3. `make`

Finally run `./hello` to run the program. <br>
`./hello` can be adjusted by changing `bin_PROGRAMS = hello` in **Makefile.am** <br>

The demo works if the result is like below:
```{bash}
Hello World!
```

If you change codes in **main.cc**, just need to run `make` command and `./hello` again.<br>
Other files' changes need to run all commands again.
