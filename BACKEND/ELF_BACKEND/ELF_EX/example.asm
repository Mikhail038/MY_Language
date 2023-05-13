; nasm -f elf64 -g -o example.o example.asm
; ld -m elf_x86_64 example.o -o example

;==========================================================
;04.05.2023 //Language ELF test
;==========================================================

global _start

_start:

    push 1230000000
    pop [rax]

    mov rax, 60
    mov rdi, 0
    syscall

;00000000:00401000|b8 3c 00 00 00 bf 00 00 00 00 0f 05            |.<..........    |

