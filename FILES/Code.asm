push 5
push 6
mul
pop [0]

push 5
push 7
push 4
sub
mul
push [0]
push [0]
add
add
pop [1]

push 0
pop [2]

push [1]
push [0]
push 7
add
sub
push 5
div
pop [2]

push 3
sin
push 2
push 4
push 5
add
mul
push 6
dup
sin
cos
div
add
sub
hlt
