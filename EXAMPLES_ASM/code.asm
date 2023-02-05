;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    jump lbl_main

lbl_sum:
    push    push [0]
    push [1]
    add
    ret


;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


lbl_main:
    push 5
    push 1
    push 6
    push 1
    mul
    sub
    add
    pop [2]

    push 0
    pop [3]

    push 3
    push 4
    jbe lbl_0
    push 0
    jump lbl_1
lbl_0:
    push 1
lbl_1:
    push 2
    mul
    push 0
    jne lbl_2
    push 0
    jump lbl_3
lbl_2:
    push 1
lbl_3:
    pop [3]

    push [3]

    out
    inp
    pop [2]

    inp
    pop [3]

    push [3]
    push 2
    je lbl_4
    push 0
    jump lbl_5
lbl_4:
    push 1
lbl_5:
    push 0
    je lbl_6
    push 7
    pop [4]

    push 6
    pop [3]

    push [4]

    out
    jump lbl_7
lbl_6:
    push 0
    push 0
    je lbl_8
    push [3]

    out
    jump lbl_9
lbl_8:
    push [2]

    out
lbl_9:
lbl_7:
    push 5
    push 7
    push 4
    sub
    mul
    push [2]
    push [2]
    ceil
    add
    add
    pop [5]

    push [5]
    push [2]
    push 7
    push 1
    add
    add
    sub
    push 5
    div
    pop [3]

    push [3]

    out
    push    push 2
    push 4
    sub
    ret


;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


lbl_умн:
    push 1
    pop [6]

    push    push [6]
    ret


;----------------------------------------------------------------------------------------------------------------------------------------------------------------------------


