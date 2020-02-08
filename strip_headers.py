import os
import sys
import struct
import random
import time

page_align = lambda k: (((k)+((0x1000)-1))&(~((0x1000)-1)))

class Elf64Phdr:
    def __init__(self, bytes):
        (self.p_type,
        self.p_offset,
        self.p_vaddr,
        self.p_paddr,
        self.p_filesz,
        self.p_memsz,
        self.p_flags,
        self.p_align,
        ) = struct.unpack("2I6Q", bytes[:struct.calcsize("2I6Q")])

class Elf64Ehdr:
    def __init__(self, bytes):
         (self.e_type,
         self.e_machine,
         self.e_version,
         self.e_entry,
         self.e_phoff,
         self.e_shoff,
         self.e_flags,
         self.e_ehsize,
         self.e_phentsize,
         self.e_phnum,
         self.e_shentsize,
         self.e_shnum,
         self.e_shstrndx) = struct.unpack("2HI3QI6H", bytes[16:struct.calcsize("2HI3QI6H")+16])
    
def main(path):
    with open(path, 'rb') as fd:
        buff = fd.read()
        ehdr = Elf64Ehdr(buff)
        phdr = buff[ehdr.e_phoff:ehdr.e_phoff+(ehdr.e_phnum * ehdr.e_phentsize)]

    delta = (ehdr.e_ehsize + (ehdr.e_phnum * ehdr.e_phentsize))
    with open(path, 'wb') as fd:
        fd.write(buff[delta:])

    os.system("xxd -i test > %s" % os.path.join("./include", "embedded.h"))

    with open(os.path.join("./include", "embedded.h"), "wb") as fd:
        fd.write("#define G_AT_ENTRY " + hex(ehdr.e_entry) + "\n")
        fd.write("#define G_AT_PHNUM " + hex(ehdr.e_phnum) + "\n")

        random.seed(time.time())
        fd.write("#define G_IMAGEBASE " + hex(page_align(random.randint(0x100000, 0xffffffff))) + "\n")
    
    with open("phdr", "wb") as fd:
        fd.write(phdr)

    os.system("xxd -i phdr >> %s" % os.path.join("./include", "embedded.h"))
    os.system("rm ./phdr")

if __name__ == '__main__': 
    main(sys.argv[1])
