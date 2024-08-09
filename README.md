# RE Toy

## Intro

* This is a toy regex engine
* This is an ongoing hobby project
* This was inspired by the great articles [1][2][3]
* This represents a subset of the features outlined in kean grammar [4]
* This is incomplete
* There are known bugs

## Repo
The repo is organized as follows:

* tools - tools to help visualize output
* src/cjson - cjson library
* src/utils - some shared files
* src/compiler/frontend - compiler part that parses the regex string and produces an AST
* src/compiler/backend - compiler part that converts the AST to a bytecode
* src/vm - virtual machine to run the bytecode

## How to build
```
cd retoy
mkdir build
cd build
cmake ..
make
```

You can also compile with ASAN `make asan` and with debug enabled `make debug`

To build everything: `make build_all`

## Running

### Run
```
./retoy -r 'a{1,3}(b|c)d{1,}' -o /tmp/re.bc -a /tmp/re.ast.json
Regex:                 a{1,3}(b|c)d{1,}
Output filename:       /tmp/re.bc
AST filename:          /tmp/re.ast.json
================================================================================
AST file:        /tmp/re.ast.json
================================================================================
Compiled program: /tmp/re.bc
```
### Show disaassembly listing and the automata
```
python ./tools/disassembler.py /tmp/re.bc
```

## References
1. https://swtch.com/~rsc/regexp/regexp1.html
2. https://swtch.com/~rsc/regexp/regexp2.html
3. https://kean.blog/post/regex-parser
4. https://github.com/kean/Regex/blob/main/grammar.ebnf
