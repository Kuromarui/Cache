savedcmd_/home/and/test/cash.mod := printf '%s\n'   cash.o | awk '!x[$$0]++ { print("/home/and/test/"$$0) }' > /home/and/test/cash.mod
