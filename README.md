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
Let's use the regex `a{1,3}(b|c)d{1,}` as example

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

### Show the AST
```
python ./tools/show_ast.py /tmp/re.ast.json
```
![AST](/assets/ast.png)


### Show disassembly listing and the NFA
```
python ./tools/disassembler.py /tmp/re.bc
0x0000:   0261                 char 61
0x0002:   0b0b0000000d000000   split 0x000b, 0x000d
0x000b:   0261                 char 61
0x000d:   0b1600000018000000   split 0x0016, 0x0018
0x0016:   0261                 char 61
0x0018:   0b2100000028000000   split 0x0021, 0x0028
0x0021:   0262                 char 62
0x0023:   0c2a000000           jmp 0x002a
0x0028:   0263                 char 63
0x002a:   0264                 char 64
0x002c:   0b2a00000035000000   split 0x002a, 0x0035
0x0035:   0d                   accept
```
![NFA](/assets/nfa.png)

## References
1. https://swtch.com/~rsc/regexp/regexp1.html
2. https://swtch.com/~rsc/regexp/regexp2.html
3. https://kean.blog/post/regex-parser
4. https://github.com/kean/Regex/blob/main/grammar.ebnf
