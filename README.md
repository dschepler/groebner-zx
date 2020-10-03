# groebner-zx
Small tool to calculate Groebner bases for ideals of Z[x]

## Requirements
* Meson
* Pkg-config
* GMP C++ bindings (gmpxx)
* Optional: googletest (for running the unit tests)

For example, on Debian, to install the dependencies:
```
pkcon install meson pkg-config libgmp-dev libgtest-dev
```

## Building
```
meson builddir -Dbuildtype=release
cd builddir
meson compile
meson test  # optional
```

## Running
```
./groebner
```

Example interaction log:
```
groebner-zx  Copyright (C) 2020  Daniel Schepler
This program comes with ABSOLUTELY NO WARRANTY.
This is free software, and you are welcome to redistribute it
under the conditions of the GNU General Public License v3

Enter the list of polynomials, one on each line as a list of coefficients
e.g. enter x^5 - 3x^2 + x as: 1 0 0 -3 1 0
Then end the list with a blank line or EOF
1 2 3 4 5
4 6 6 4

< x^4 + 2 x^3 + 3 x^2 + 4 x + 5, 4 x^3 + 6 x^2 + 6 x + 4 > = < x^4 + 3 x^2 + 10 x + 21, 2 x^3 + 24 x + 14, 6 x^2 + 18 x + 6, 30 >
```
