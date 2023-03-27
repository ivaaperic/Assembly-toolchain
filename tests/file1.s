.extern null, prog, exit, main, intr
.global start
.section ivt
.word prog
.word exit
.word null
.word null
.word intr
.skip 6

.section text
start:
  jmp main