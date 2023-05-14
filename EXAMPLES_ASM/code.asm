;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    push 0
    pop rax

    push 0
    pop rcx

    jump lbl_main

;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

lbl_discriminant:
    push rax
    pop rbx

    push rbx
    push 1
    add
    pop rbx
    pop [rbx]

    push rbx
    push 1
    add
    pop rbx
    pop [rbx]

    push rbx
    push 1
    add
    pop rbx
    pop [rbx]

    push 2
    push rax
    add
    pop rbx
    push [rbx]

    push 2
    push rax
    add
    pop rbx
    push [rbx]

    mul
    push 4
    push 1
    push rax
    add
    pop rbx
    push [rbx]

    push 3
    push rax
    add
    pop rbx
    push [rbx]

    mul
    mul
    sub
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

    push 0
    push 2
    push rax
    add
    pop rbx
    pop [rbx]

    push rcx
    push 1
    add
    pop rcx

    push 0
    push 3
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

    inp
    push 2
    push rax
    add
    pop rbx
    pop [rbx]

    inp
    push 3
    push rax
    add
    pop rbx
    pop [rbx]

    push 0
    push 4
    push rax
    add
    pop rbx
    pop [rbx]

    push rcx
    push 1
    add
    pop rcx

    push 0
    push 5
    push rax
    add
    pop rbx
    pop [rbx]

    push rcx
    push 1
    add
    pop rcx

;++

    push rcx
    push 3
    push rax
    add
    pop rbx
    push [rbx]

    push 2
    push rax
    add
    pop rbx
    push [rbx]

    push 1
    push rax
    add
    pop rbx
    push [rbx]

    push rcx
    pop rax

    call lbl_discriminant
    pop rcx
;++

    push rdx
    push 1
    push rax
    add
    pop rbx
    push [rbx]

    push 0
    jne lbl_0
    push 0
    jump lbl_1
lbl_0:
    push 1
lbl_1:
    push 0
    je lbl_2
;++

    push rcx
    push 3
    push rax
    add
    pop rbx
    push [rbx]

    push 2
    push rax
    add
    pop rbx
    push [rbx]

    push 1
    push rax
    add
    pop rbx
    push [rbx]

    push rcx
    pop rax

    call lbl_discriminant
    pop rcx
;++

    push rdx
    push 0
    ja lbl_3
    push 0
    jump lbl_4
lbl_3:
    push 1
lbl_4:
    push 0
    je lbl_5
    push 1337
    push 4
    push rax
    add
    pop rbx
    pop [rbx]

    push 4
    push rax
    add
    pop rbx
    push [rbx]

    out
    jump lbl_6
lbl_5:
    push 0
    push 2
    push rax
    add
    pop rbx
    push [rbx]

    sub
;++

    push rcx
    push 3
    push rax
    add
    pop rbx
    push [rbx]

    push 2
    push rax
    add
    pop rbx
    push [rbx]

    push 1
    push rax
    add
    pop rbx
    push [rbx]

    push rcx
    pop rax

    call lbl_discriminant
    pop rcx
;++

    push rdx
    sqrt
    sub
    push 2
    push 1
    push rax
    add
    pop rbx
    push [rbx]

    mul
    div
    push 4
    push rax
    add
    pop rbx
    pop [rbx]

    push 0
    push 2
    push rax
    add
    pop rbx
    push [rbx]

    sub
;++

    push rcx
    push 3
    push rax
    add
    pop rbx
    push [rbx]

    push 2
    push rax
    add
    pop rbx
    push [rbx]

    push 1
    push rax
    add
    pop rbx
    push [rbx]

    push rcx
    pop rax

    call lbl_discriminant
    pop rcx
;++

    push rdx
    sqrt
    add
    push 2
    push 1
    push rax
    add
    pop rbx
    push [rbx]

    mul
    div
    push 5
    push rax
    add
    pop rbx
    pop [rbx]

lbl_6:
    jump lbl_7
lbl_2:
    push 0
    push 3
    push rax
    add
    pop rbx
    push [rbx]

    push 2
    push rax
    add
    pop rbx
    push [rbx]

    div
    sub
    push 4
    push rax
    add
    pop rbx
    pop [rbx]

    push 4
    push rax
    add
    pop rbx
    push [rbx]

    out
lbl_7:
    push 4
    push rax
    add
    pop rbx
    push [rbx]

    out
    push 5
    push rax
    add
    pop rbx
    push [rbx]

    out
    hlt


;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


