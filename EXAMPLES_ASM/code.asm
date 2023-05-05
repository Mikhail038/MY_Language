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

    push 1
    push 2
    push 3
    push 4
    push 5
    add
    push 6
    div
    mul
    sub
    add
    push 4
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

    push 7
    push 8
    add
    push 9
    push 10
    jne lbl_0
    push 0
    jump lbl_1
lbl_0:
    push 1
lbl_1:
    push 11
    add
    push 0
    jne lbl_2
    push 0
    jump lbl_3
lbl_2:
    push 1
lbl_3:
    ja lbl_4
    push 0
    jump lbl_5
lbl_4:
    push 1
lbl_5:
    mul
    push 0
    jne lbl_6
    push 0
    jump lbl_7
lbl_6:
    push 1
lbl_7:
    push 5
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
    push 6
    push rax
    add
    pop rbx
    pop [rbx]

    push rcx
    push 1
    add
    pop rcx

    push 0
    push 7
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
    jne lbl_8
    push 0
    jump lbl_9
lbl_8:
    push 1
lbl_9:
    push 0
    je lbl_10
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
    ja lbl_11
    push 0
    jump lbl_12
lbl_11:
    push 1
lbl_12:
    push 0
    je lbl_13
    push 1337
    push 6
    push rax
    add
    pop rbx
    pop [rbx]

    push 6
    push rax
    add
    pop rbx
    push [rbx]

    out
    jump lbl_14
lbl_13:
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
    push 6
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
    push 7
    push rax
    add
    pop rbx
    pop [rbx]

lbl_14:
    jump lbl_15
lbl_10:
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
    push 6
    push rax
    add
    pop rbx
    pop [rbx]

    push 6
    push rax
    add
    pop rbx
    push [rbx]

    out
lbl_15:
    push 6
    push rax
    add
    pop rbx
    push [rbx]

    out
    push 7
    push rax
    add
    pop rbx
    push [rbx]

    out
    hlt


;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


