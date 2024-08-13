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
OP_MARK_SOL             = 14
OP_MARK_EOL             = 15




class Section:
    def __init__(self, prog, address, size):
        self.prog = prog
        self.address = address
        self.size = size

    def readn(self, off, n):
        start = self.address + off
        return self.prog.mem[start:start+n]

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
        self.extra = data['extra']

    def __str__(self):
        opcodes_s = ''.join([f"{op:02x}" for op in self.opcodes])
        out = f"0x{self.addr:04x}:   {opcodes_s.ljust(10*2)} {self.text}"
        if self.extra:
            out = f'{out.ljust(60)}; [{self.extra}]'
        return out


class NFA:
    def __init__(self, insts):
        self.insts = insts
        self.g = graphviz.Digraph(format='png')
        self.g.attr(rankdir='LR')

    def build(self):
        for inst in self.insts:
            node_name = hex(inst.addr)
            if inst.opcodes[0] == OP_ACCEPT:
                shape_config = {'shape':'doublecircle', 'width':'.6', 'fixedsize': 'true'}
            else:
                shape_config = {'shape':'circle', 'width':'.6', 'fixedsize': 'true'}
            self.g.node(node_name, node_name, **shape_config)

        for i in range(len(self.insts)):
            opcode = self.insts[i].opcodes[0]
            node_name = hex(self.insts[i].addr)
            if opcode == OP_JMP:
                arr = self.insts[i].text.split(" ")
                target = hex(int(arr[1], 16))
                self.g.edge(node_name, target, "''", style='dotted')
            elif opcode == OP_SPLIT:
                matches = re.findall(r'0x[0-9a-fA-F]{1,8}', self.insts[i].text)
                arr = self.insts[i].text.split(" ")
                l1 = hex(int(matches[0], 16))
                l2 = hex(int(matches[1], 16))
                self.g.edge(node_name, l1, "''", style='dotted')
                self.g.edge(node_name, l2, "''", style='dotted')
            else:
                if i + 1 >= len(self.insts): continue
                target = hex(self.insts[i+1].addr)
                label = ''

                if opcode == OP_MATCH_CHAR:
                    ch = str(chr(self.insts[i].opcodes[1]))
                    label = f"'{ch}'"

                elif opcode == OP_MARK_SOL:
                    label = '(sol)'

                elif opcode == OP_MARK_EOL:
                    label = '(eol)'

                elif opcode == OP_MATCH_DIGIT:
                    label = '[0-9]'

                elif opcode == OP_MATCH_WORDCHAR:
                    label = '[a-zA-Z0-9_]'

                elif opcode == OP_MATCH_WHITESPACE:
                    label = '[\\s]'

                elif opcode == OP_MATCH_IN_SET:
                    label = f'[{self.insts[i].extra}]'

                elif opcode == OP_MATCHNOT_DIGIT:
                    label = '[^0-9]'

                elif opcode == OP_MATCHNOT_WORDCHAR:
                    label = '[^a-zA-Z0-9_]'

                elif opcode == OP_MATCHNOT_WHITESPACE:
                    label = '[^\\s]'

                elif opcode == OP_MATCHNOT_IN_SET:
                    label = f'[^{self.insts[i].extra}]'

                self.g.edge(node_name, target, label)

        self.g.render('/tmp/nfa', view=True)


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
        extra = None

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

        elif opcode == OP_MARK_SOL:
            text = 'mark.sol'

        elif opcode == OP_MARK_EOL:
            text = 'mark.eol'

        elif opcode == OP_MATCH_IN_SET or opcode == OP_MATCHNOT_IN_SET:
            isnt_name = 'match.set'
            if opcode == OP_MATCHNOT_IN_SET: isnt_name = 'matchnot.set'
            opcodes += [self.prog.code.read8(addr+1+i) for i in range(4)]
            bitmap_addr = self.prog.code.read32(addr+1)
            bitmap = self.prog.data.readn(bitmap_addr, 256//8)
            extra = self.bitmap2s(bitmap)
            text = f'{isnt_name} ds:0x{bitmap_addr:04x}'


        return Inst({'addr': addr, 'opcodes': opcodes, 'text': text, 'extra': extra})


    def bitmap2s(self, bitmap):
        bmap = []
        for i in range(256//8): bmap += list(f"{bitmap[i]:08b}")

        chars = [i for i in range(len(bmap)) if bmap[i] == '1']
        ranges = []
        while chars:
            therange = [chars.pop(0)]
            while chars and (chars[0] - therange[-1]) == 1:
                if len(therange) > 1: therange.pop()
                therange.append(chars.pop(0))
            ranges.append(therange)

        ranges_s = []
        for r in ranges:
            a = [f"'{chr(b)}'" if (b>=0x20 and b<=0x7f) else f'0x{b:02x}' for b in r]
            ranges_s.append('-'.join(a))
            #ranges_s.append('-'.join([chr(b) for b in r]))
        return ','.join(ranges_s)


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


    def data(self):
        return self.prog.data.readn(0, self.prog.data.size)


def main(filename):
    disassembler = Disassembler()
    disassembler.load_file(filename)
    insts = disassembler.disas_all()
    for inst in insts:
        print(inst)

    #data = disassembler.data()

    NFA(insts).build()


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print(f"usage: {sys.argv[0]} prog")
        sys.exit(1)
    main(sys.argv[1])
