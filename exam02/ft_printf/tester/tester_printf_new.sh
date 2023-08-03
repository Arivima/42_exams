gcc -Wall -Werror -Wextra ft_printf*.c main_new.c -o ft_printf
./ft_printf > yy
cat -e yy > y
gcc -Wall -Werror -Wextra -D REAL main_new.c -o printf
./printf | cat -e > r
diff -y r y
rm -rf yy y r ft_printf printf
