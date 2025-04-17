# R with conda
So long annoyed by the incompatibility of RStudio and VSCode R Plugin with R in conda virtual environment.

Supports running R with conda by simply changing the executable name.

## Install
Name the exe file `R.conda.exe`, and put it the same folder of your R executable.

When using a conda environment, like `C:\Users\<your-name>\.conda\envs\<your-env>\`, the R executable should be at `C:\Users\<your-name>\.conda\envs\<your-env>\Scripts\R.exe`.

Then you should put `R.conda.exe` at `C:\Users\<your-name>\.conda\envs\<your-env>\R.conda.exe`.

For `Rterm.exe` and other R executables, copy `R.conda.exe`, and rename it after them and ending with `.conda.exe`. Also put it in `C:\Users\<your-name>\.conda\envs\<your-env>\Scripts\`.

Be aware that other R executables should be in `C:\Users\<your-name>\.conda\envs\<your-env>\Scripts\` as well.

So now you have `R.conda.exe` and `Rterm.conda.exe` (a copy of `R.conda.exe` with name altered) and other executables.

## Usage
Run these files with **ABSOLUTE PATH**.

Replace `/path/to/R.conda.exe` with `R.exe` and `/path/to/Rterm.conda.exe` with `Rterm.exe` when setting `r.rpath.windows` and `r.rterm.windows` fields in R Plugin for VSCode.

## Compatibility
Support Windows Only NOW!

Easy to modify yourself for Linux/MacOS support

## Compile yourself
Just compile it yourself.

My version is compiled using MinGW gcc 14.0.0 240107 (experimental) from winlibs.
