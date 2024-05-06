# AudioFileEditor
Repo contains two C program for manipulation sounds in audio files.
# remvocals.c
Progeam removes vocals from stereo PCM sounds stored in the canonical WAV format. It operates by subtracting one channel (right) from the other (left) and then dividing by 2 to maintain volume levels. The resulting WAV file will have vocals removed, leaving other instrumentals intact.
- Usage: ~$removocal sourcewav destwav
# addecho.c
The addecho program adds echo to a mono sound file. It does this by creating an echo buffer and mixing it with the original sound at a specified delay and volume scale. 
- Usage: ~$addecho [-d delay] [-v volume] sourcewav destwav

