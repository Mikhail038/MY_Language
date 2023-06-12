;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    push 0
    pop rax

    push 0
    pop rcx

    jump lbl_main

;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

lbl_fac:
    push rax
    pop rbx

    push rbx
    push 1
    add
    pop rbx
    pop [rbx]

    push 1
    push rax
    add
    pop rbx
    push [rbx]

    push 1
    sub
    push 2
    push rax
    add
    pop rbx
    pop [rbx]

    push rcx
    push 1
    add
    pop rcx

    push 1
    push rax
    add
    pop rbx
    push [rbx]

    push 1
    je lbl_0
    push 0
    jump lbl_1
lbl_0:
    push 1
lbl_1:
    push 0
    je lbl_2
    push 1
    pop rdx
    ret
    jump lbl_3
lbl_2:
lbl_3:
    push 2
    push rax
    add
    pop rbx
    push [rbx]

    push 1
    add
;++

    push rcx
    push 2
    push rax
    add
    pop rbx
    push [rbx]

    push rcx
    pop rax

    call lbl_fac
    pop rcx
;++

    push rdx
    mul
    pop rdx
    ret


;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


lbl_main:
    push rax
    pop rbx

    push 0
    push 1
    push rax
    add
    pop rbx
    pop [rbx]

    push rcx
    push 1
    add
    pop rcx

    inp
    push 1
    push rax
    add
    pop rbx
    pop [rbx]

;++

    push rcx
    push 1
    push rax
    add
    pop rbx
    push [rbx]

    push rcx
    pop rax

    call lbl_fac
    pop rcx
;++

    push rdx
    push 1
    push rax
    add
    pop rbx
    pop [rbx]

    push 1
    push rax
    add
    pop rbx
    push [rbx]

    out
    hlt


;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


