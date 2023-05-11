#!/bin/bash
set -e
cd $(dirname $0)

echo ">>>>>>>>>>> COMPILATION <<<<<<<<<<<"
gcc -Wall -Werror -Wextra -DBUFFER_SIZE=5 main1.c get_next_line.c -o your_gnl_1.out || (echo "KO -> COMPILATION FAILED" && exit 1)
gcc -Wall -Werror -Wextra -DBUFFER_SIZE=5 main2.c get_next_line.c -o your_gnl_2.out || (echo "KO -> COMPILATION FAILED" && exit 1)
gcc -Wall -Werror -Wextra main2_generator.c -o generator.out
echo "OK"

echo ">>>>>>>>>>>   TESTING   <<<<<<<<<<<"
rm -f empty_file big_file big_file2 your_output our_output

echo "> Reading main1.c"
touch your_output
./your_gnl_1.out < main1.c > your_output || (echo "KO -> EXECUTION FAILED" && exit 1)
diff your_output main1.c || (echo "KO -> TEST FAILED" && exit 1)
rm -f your_output
echo "OK"

echo "> Reading from an empty file"
touch empty_file your_output
./your_gnl_1.out < empty_file > your_output
diff your_output empty_file || (echo "KO -> TEST FAILED" && exit 1)
rm -f empty_file your_output
echo "OK"

echo "> Reading from a big file 1"
touch big_file your_output our_output
for i in `seq 1 10000 100000`; do
    for j in `seq 1 10`; do
        base64 /dev/urandom | tr -d '/+' | fold -w $i | head -n $j > big_file
        ./your_gnl_1.out < big_file > your_output || (echo "KO -> EXECUTION FAILED" && exit 1)
        diff your_output big_file || (echo KO -> TEST FAILED && exit 1)
    done
done
rm -f big_file your_output our_output
echo "OK"

echo "> Reading from a big file 2"
touch big_file big_file2 your_output our_output
echo > big_file
for i in `seq 1 10`; do cat big_file >> big_file2; cat big_file2 >> big_file; done
./your_gnl_1.out < big_file > your_output || (echo "KO -> EXECUTION FAILED" && exit 1)
diff your_output big_file || (echo "KO -> TEST FAILED" && exit 1)
rm -f big_file big_file2 your_output our_output
echo "OK"

echo "> Reading from a big file 3"
touch big_file big_file2 your_output our_output
echo a > big_file
for i in `seq 1 10`; do cat big_file >> big_file2; cat big_file2 >> big_file; done
./your_gnl_1.out < big_file > your_output || (echo "KO -> EXECUTION FAILED" && exit 1)
diff your_output big_file || (echo "KO -> TEST FAILED" && exit 1)
rm -f big_file big_file2 your_output our_output your_gnl_1.out
echo "OK"

echo "> Sequential Read"
./generator.out > our_output
./your_gnl_2.out > your_output || (echo "KO -> EXECUTION FAILED" && exit 1)
diff your_output our_output || (echo "KO -> TEST FAILED" && exit 1)
rm -f your_output our_output your_gnl_2.out generator.out
echo "OK"

echo "THIS SCRIPT DOES NOT CHECK FOR LEAKS. MAKE SURE THAT YOU HAVE NONE BECAUSE IT WILL BE CHECKED"