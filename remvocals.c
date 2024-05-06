#include <stdlib.h>
#include <stdio.h>

int copy_header(FILE **source, FILE **dest) {	
	#define HEADER_SIZE 44
	char header[HEADER_SIZE];
	int bytes_read;
	if((bytes_read = fread(header, HEADER_SIZE, 1, *source)) != 1) {
		fprintf(stderr, "Could not read header\n");
		fprintf(stderr, "Read %d times\n", bytes_read);
		return 1;
	}
	int bytes_written;
	if((bytes_written = fwrite(header, HEADER_SIZE, 1, *dest)) != 1) {
        	fprintf(stderr, "Could not write header\n");
        	fprintf(stderr, "Wrote %d times\n", bytes_written);
   	 }

	return 0;
}

int write_destwav(FILE **source, FILE  **dest) {
	short left, right, combined;

	//loop through each pair of short
	//compute combined = (left - right) / 2
	// write combined twice in destwav
	
	while(fread(&left, sizeof(short), 1, *source) == 1 && fread(&right, sizeof(short), 1, *source) == 1) {	
		combined = (left - right) / 2;
		if(fwrite(&combined, sizeof(short), 1, *dest) != 1){
			fprintf(stderr, "Could not write left sample\n");
			return 1;
		}		
		if(fwrite(&combined, sizeof(short), 1, *dest) != 1) {
			fprintf(stderr, "Could not write right sample\n");
			return 1;
		}
	}
	return 0;
	
}

int main(int argc, char **argv) {
	FILE *sourcewav, *destwav;
	if(argc != 3) {
		fprintf(stderr, "Must have two commands\n");
		return 1;
	}

	sourcewav = fopen(argv[1], "rb");
	destwav = fopen(argv[2], "wb");
	if(sourcewav == NULL) {
		fprintf(stderr, "sourcewav could not be opened\n");
		return 1;
	}
	if(destwav == NULL) {
		fprintf(stderr, "destwav could not be opened\n");
		return 1;
	}

	// Copy first 44 bytes from sourcewav to destwav
	if(copy_header(&sourcewav, &destwav) != 0) {
		fprintf(stderr, "could not copy header file\n");
		return 1;
	}

	// Next, treat the rest of the input file as a sequence of shorts.
	// Take each pair of shorts left and right,
	// and compute combined = (left - right) / 2.
	// Write two copies of combined to the output file.
	
	if(write_destwav(&sourcewav, &destwav) != 0) {
		fprintf(stderr, "could not write to destination file\n");
		return 1;
	}
	
	if(fclose(sourcewav) != 0) {
		fprintf(stderr, "Error: could not close source file\n");
		return 1;
	}
	if(fclose(destwav) != 0) {
		fprintf(stderr, "Error: could not close destination file\n");
		return 1;
	}
	return 0;


}
