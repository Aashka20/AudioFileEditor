#! /bin/bash

make
echo Testing remvocals
./remvocals.o audio_files/love.wav audio_files/love_out.wav
cmp audio_files/love_out.wav audio_files/love_novocals.wav

echo Testing addecho
./addecho.o -d 12000 -v 4 audio_files/door.wav audio_files/door_12000_4_out.wav
cmp audio_files/door_12000_4_out.wav audio_files/door_12000_4.wav

./addecho.o -d 12000 -v 2 audio_files/door.wav audio_files/door_12000_2_out.wav
cmp audio_files/door_12000_2_out.wav audio_files/door_12000_2.wav

./addecho.o -d 20000 -v 4 audio_files/door.wav audio_files/door_20000_4_out.wav
cmp audio_files/door_20000_4_out.wav audio_files/door_20000_4.wav

./addecho.o -d 35000 -v 2 audio_files/door.wav audio_files/door_35000_2_out.wav
cmp audio_files/door_35000_2_out.wav audio_files/door_35000_2.wav

./addecho.o -d 60000 -v 1 audio_files/door.wav audio_files/door_60000_1_out.wav
cmp audio_files/door_60000_1_out.wav audio_files/door_60000_1.wav

./addecho.o -d 3 -v 2 audio_files/short.wav audio_files/short_3_2_out.wav
cmp audio_files/short_3_2_out.wav audio_files/short_3_2.wav

echo "short.wav (original file)"
dd if=audio_files/short.wav ibs=1 count=100 skip=0 2>/dev/null | od -tu2

echo "short_3_2.wav (example output)"
dd if=audio_files/short_3_2.wav ibs=1 count=100 skip=0 2>/dev/null | od -tu2

echo "short_3_2_out.wav (our output)"
dd if=audio_files/short_3_2_out.wav ibs=1 count=100 skip=0 2>/dev/null | od -tu2

# Default testing
./addecho.o audio_files/welcome.wav audio_files/welcome_out_10_5.wav
