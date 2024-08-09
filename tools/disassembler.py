import sys
import struct
import graphviz
import re


OP_NOP                  =  0
OP_MATCH_ANY            =  1
OP_MATCH_CHAR           =  2
OP_MATCH_DIGIT          =  3
OP_MATCH_WORDCHAR       =  4
OP_MATCH_WHITESPACE     =  5
OP_MATCH_IN_SET         =  6
OP_MATCHNOT_DIGIT       =  7
OP_MATCHNOT_WORDCHAR    =  8
OP_MATCHNOT_WHITESPACE  =  9
OP_MATCHNOT_IN_SET      = 10
OP_SPLIT                = 11
OP_JMP                  = 12
OP_ACCEPT               = 13



class Section:
    def __init__(self, prog, address, size):
        self.prog = prog
        self.address = address
        self.size = size


    def read8(self, off):
        return self.prog.mem[self.address+off]

    def read32(self, off):
        return struct.unpack_from('<I', self.prog.mem, self.address+off)[0]


class Prog:
    def __init__(self, mem):
        self.mem = mem
        self.code = Section(self, self.read32(0x00), self.read32(0x08))
        self.data = Section(self, self.read32(0x10), self.read32(0x18))

    def read8(self, off):
        return self.mem[off]

    def read32(self, off):
        return struct.unpack_from('<I', self.mem, off)[0]


class Inst:
    def __init__(self, data):
        self.addr = data['addr']
        self.opcodes = data['opcodes']
        self.text = data['text']

    def __str__(self):
        opcodes_s = ''.join([f"{op:02x}" for op in self.opcodes])
        return f"0x{self.addr:04x}:   {opcodes_s.ljust(10*2)} {self.text}"


class CFG:
    def __init__(self, insts):
        self.insts = insts
        self.g = graphviz.Digraph(format='png')
        self.g.attr(rankdir='LR')

    def build(self):
        shape_config = {'shape':'circle'}

        for inst in self.insts:
            if inst.opcodes[0] == OP_ACCEPT:
                self.g.node(str(inst.addr), str(inst.addr), **{'shape':'doublecircle'})
            else:
                self.g.node(str(inst.addr), str(inst.addr), **shape_config)

        for i in range(len(self.insts)):
            opcode = self.insts[i].opcodes[0]
            if opcode == OP_JMP:
                arr = self.insts[i].text.split(" ")
                target = int(arr[1], 16)
                self.g.edge(str(self.insts[i].addr), str(target), "''", style='dotted')
            elif opcode == OP_SPLIT:
                matches = re.findall(r'0x[0-9a-fA-F]{1,8}', self.insts[i].text)
                arr = self.insts[i].text.split(" ")
                l1 = int(matches[0], 16)
                l2 = int(matches[1], 16)
                self.g.edge(str(self.insts[i].addr), str(l1), "''", style='dotted')
                self.g.edge(str(self.insts[i].addr), str(l2), "''", style='dotted')
            else:
                if i + 1 >= len(self.insts): continue
                a = str(self.insts[i].addr)
                b = str(self.insts[i+1].addr)
                label = ''

                if opcode == OP_MATCH_CHAR:
                    ch = str(chr(self.insts[i].opcodes[1]))
                    label = f"'{ch}'"

                elif opcode == OP_MATCH_DIGIT:
                    label = '[0-9]'

                elif opcode == OP_MATCH_WORDCHAR:
                    label = '[a-zA-Z0-9_]'

                elif opcode == OP_MATCH_WHITESPACE:
                    label = '[\\s]'

                elif opcode == OP_MATCHNOT_DIGIT:
                    label = '[^0-9]'

                elif opcode == OP_MATCHNOT_WORDCHAR:
                    label = '[^a-zA-Z0-9_]'

                elif opcode == OP_MATCHNOT_WHITESPACE:
                    label = '[^\\s]'

                self.g.edge(a, b, label)

        self.g.render('/tmp/cfg', view=True)


class Disassembler:
    def __init__(self):
        self.prog = None

    def load_file(self, filename):
        with open(filename, 'rb') as f:
            self.prog = Prog(f.read())

    def disas(self, addr):
        opcodes = []
        text = '?'
        opcode = self.prog.code.read8(addr)
        opcodes.append(opcode)

        if opcode == OP_MATCH_ANY:
            text = 'any'

        elif opcode == OP_MATCH_CHAR:
            ch = self.prog.code.read8(addr+1)
            opcodes.append(ch)
            text = f"char {ch:02x}"

        elif opcode == OP_NOP:
            text = 'nop'

        elif opcode == OP_ACCEPT:
            text = 'accept'

        elif opcode == OP_SPLIT:
            opcodes += [self.prog.code.read8(addr+1+i) for i in range(8)]
            l1 = self.prog.code.read32(addr+1)
            l2 = self.prog.code.read32(addr+1+4)
            text = f'split 0x{l1:04x}, 0x{l2:04x}'

        elif opcode == OP_JMP:
            opcodes += [self.prog.code.read8(addr+1+i) for i in range(4)]
            target = self.prog.code.read32(addr+1)
            text = f'jmp 0x{target:04x}'

        elif opcode == OP_MATCH_DIGIT:
            text = 'match.d'

        elif opcode == OP_MATCH_WORDCHAR:
            text = 'match.w'

        elif opcode == OP_MATCH_WHITESPACE:
            text = 'match.s'

        elif opcode == OP_MATCHNOT_DIGIT:
            text = 'matchnot.d'

        elif opcode == OP_MATCHNOT_WORDCHAR:
            text = 'matchnot.w'

        elif opcode == OP_MATCHNOT_WHITESPACE:
            text = 'matchnot.s'

        return Inst({'addr': addr, 'opcodes': opcodes, 'text': text})

    def disas_all(self):
        insts = []
        pc = 0
        while pc < self.prog.code.size:
            inst = self.disas(pc)
            insts.append(inst)
            pc += len(inst.opcodes)
            if inst.text == 'accept':
                break
        return insts


def main(filename):
    disassembler = Disassembler()
    disassembler.load_file(filename)
    insts = disassembler.disas_all()
    for inst in insts:
        print(inst)
    CFG(insts).build()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"usage: {sys.argv[0]} prog")
        sys.exit(1)
    main(sys.argv[1])
