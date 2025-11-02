# LLVM Assignment 1
**Due Date:** November 3, 2025

This repository contains a framework for constructing and running passes on C files. For this particular assignment, the code implements a loop invariant code motion (LICM) pass and a induction variable elimination (IVE) pass.

The files for the assignment can be found in /lib.
1. SimpleLICM - /lib/SimpleLICM.cpp
2. DerivedInductionVar - /lib/DerivedInductionVar.cpp

NOTE: The original README.md of this framework can be found in the original repo at https://github.com/banach-space/llvm-tutor

All llvm-tutor applications are courtesy of: https://github.com/banach-space/llvm-tutor

# Getting Started
To view and run the programs of the repository, make sure to have cloned "almondragon/llvm-tutor" repository. You can clone the repository by running the following command in your preferred directory:
```bash
git clone https://github.com/almondragon/llvm-tutor.git
```

Once the repository is cloned, you must build the framework. Run the following commands in the given order:
```bash
export LLVM_DIR=/usr/lib/llvm-21
cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make
```

**NOTE:** The given commands are run in Windows Subsystem for Linux (WSL), if running on Mac, some commands will need to be changed. Additionally, the /build directory has already been made for your convenience. However, if there is no /build directory, run the following commands in the given order:
```bash
export LLVM_DIR=/usr/lib/llvm-21
mkdir build
cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make
```

## Assignment Features
1. **SimpleLICM Pass** - runs a simple loop invariant code motion pass on a given C file.

2. **DerivedInductionVar Pass** - runs a induction variable elimination pass on a given C file.

**NOTE** All given C files must be canonical for optimal results.


## Usage
The following commands show how to run each pass. 

NOTE: Input file is in regards to the C file you want to run the pass on. Output file is the file that the output will be outputted to.

**SimpleLICM Pass**
```bash
$LLVM_DIR/bin/opt -load-pass-plugin ./lib/libSimpleLICM.so -passes=simple-licm -S -o [output_file] [input_file]
```

**DerivedInductionVar Pass**
```bash
$LLVM_DIR/bin/opt   -load-pass-plugin ./lib/libDerivedInductionVar.so   -passes=derived-iv -S -o [output_file] [input_file]
```

### Actual Example Runs
**SimpleLICM Pass**
```bash
$LLVM_DIR/bin/opt -load-pass-plugin ./lib/libSimpleLICM.so -passes=simple-licm -S -o ../outputs/matmul-licm.ll ../inputs/matmul-canonical.ll
```

**DerivedInductionVar Pass**
```bash
$LLVM_DIR/bin/opt   -load-pass-plugin ./lib/libDerivedInductionVar.so   -passes=derived-iv -S -o ../outputs/matmul-iv.ll ../inputs/matmul-canonical.ll
```

## Testing & Test Cases
If running on mac, ensure the library variable is correct and .so for passes is changed to .dylib prior to running testing.sh.

### Testing
Use the following commands to run the test cases:
```bash
chmod +x testing.sh
./testing.sh
```

