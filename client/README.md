## How to generate Library Database or Logger:

You need install external library

```bash
sudo zypper install libtool
sudo zypper install readline-devel (Only for Database)
```

`cd` in folder database or logger to build library. Then run follow command:

```bash
autoreconf -i
./configure
make
sudo make install
```

```bash
export LD_LIBRARY_PATH=/usr/local/lib:$LD_LIBRARY_PATH 
```

Fix error while loading shared library

```bash
sudo ldconfig -v
```