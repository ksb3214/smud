printf "#" >> .perc

line="     "
line="${line}`cat .perc`"
echo -e "$line\r\c"
