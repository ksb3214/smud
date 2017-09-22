flag=0

rm -f .perc

clear

cat <<*END*

SlightMUD compilation script...

This should make SMUD, if there are any errors it will show up and majorly
screw up this funky little progress bar, anyway have fun and enjoy.

*END*

#
# makefile objects
#

sleep 1

line="     "
count=`ls -1 *.c | wc -l`

while [ "$count" != "1" ]
do
  line="${line}."
  # count=`expr $(count) - 1`
  count=`expr $count - 1`
done
echo "   Compiling..."
sleep 1
echo -e "$line\r\c"

count=`ls -1 *.o 2>/dev/null | wc -l`
#echo "\n\ncount = [$count]\n\n"

count=`expr $count - 0`
while [ "$count" != "0" ]
do
	printf "#" >> .perc
	count=`expr $count - 1`
done

exit 0
