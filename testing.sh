#!/bin/bash

set -e

# ==== Set up environment variables ====

echo "---Setting up environment variables---"

export LLVM_DIR=/usr/lib/llvm-21

# ==== Building llvm-tutor framework ====

echo "---Building llvm-tutor framework---"
cd build
cmake -DLT_LLVM_INSTALL_DIR=$LLVM_DIR ..
make

# ==== Testing SimpleLICM pass ====
echo "---Testing SimpleLICM pass---"
echo "Testing on ../inputs/matmul-canonical.ll"
$LLVM_DIR/bin/opt -load-pass-plugin ./lib/libSimpleLICM.so -passes=simple-licm -S -o ../outputs/matmul-licm.ll ../inputs/matmul-canonical.ll
echo "Testing on ../inputs/gcd-canoncial.ll"
$LLVM_DIR/bin/opt -load-pass-plugin ./lib/libSimpleLICM.so -passes=simple-licm -S -o ../outputs/gcd-licm.ll ../inputs/gcd-canonical.ll
echo "Testing for SimpleLICM complete. Outputs found in ../outputs/{file}-licm."

# ==== Testing DerivedInductionVar pass ====
echo "---Testing DerivedInductionVar pass---"
echo "Testing on ../inputs/matmul-canonical.ll"
$LLVM_DIR/bin/opt   -load-pass-plugin ./lib/libDerivedInductionVar.so   -passes=derived-iv -S -o ../outputs/matmul-iv.ll ../inputs/matmul-canonical.ll
echo "Testing on ../inputs/test2.ll"
$LLVM_DIR/bin/opt   -load-pass-plugin ./lib/libDerivedInductionVar.so   -passes=derived-iv -S -o ../outputs/test2-iv.ll ../inputs/test2.ll
echo "Testing on ../inputs/test3.ll"
$LLVM_DIR/bin/opt   -load-pass-plugin ./lib/libDerivedInductionVar.so   -passes=derived-iv -S -o ../outputs/test3-iv.ll ../inputs/test3.ll
echo "Testing for DerivedInductionVar complete. Outputs found in ../outputs/{file}-iv."

echo "---All tests completed---"