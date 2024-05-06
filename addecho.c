#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int process_options(int argc, char **argv, int *delay, int *volume, int *num_options) {
    // Take argc and argv and update delay and volume if specified
    // Update num_options to match the number of given options
    //
    // addecho -d 3 -v 2 short.wav short_3_2.wav
    // Should error if delay or volume <= 0
    // default delay value is 8000 and default volumne value is 4
    #define DELAY_DEFAULT 8000
    #define VOL_DEFAULT 4
    int option;
    int curr = 0;

    *delay = DELAY_DEFAULT;
    *volume = VOL_DEFAULT;

    while((option = getopt(argc, argv, ":d:v:")) != -1) {
        // Use switch statement to check options
	switch (option) {
		case 'd': //-d tag is for delay
			
			// the value for delay is stored in optarg, must convert to int
			*delay = strtol(optarg, NULL, 10);
			if (*delay < 0) {
				return 1;
			}
			break;
		case 'v':
			*volume = strtol(optarg, NULL, 10);
			if (*volume <= 0) {
				return 1;
			}
			break;
		case '?':
		default:
			*delay = DELAY_DEFAULT;
			*volume = VOL_DEFAULT;
	    }
	    curr++;
    }
    *num_options = curr;
    // If everything is alright, return 0
    return 0;
}

int copy_header(FILE **source, FILE **dest, int delay) {
    // Change bytes in the header to increase length of file
    // by 2 * delay
    // Copy the rest of the header as is
    // Return 0 on success

    #define HEADER_SIZE 22
    short header[HEADER_SIZE];
    unsigned int *sizeptr, *sizeptr2;
    
    int bytes_read;
    bytes_read = fread(header, sizeof(short), HEADER_SIZE, *source);
    if(bytes_read != HEADER_SIZE) {
        fprintf(stderr, "Error: could not read header\n");
        return 1;
    }
    
    sizeptr = (unsigned int *)(header + 2);
    sizeptr2 = (unsigned int *)(header + 20);
    
    *sizeptr += delay*2;
    *sizeptr2 += delay*2;

    int bytes_written;
    bytes_written = fwrite(header, sizeof(short), HEADER_SIZE, *dest);
    if(bytes_written != HEADER_SIZE) {
    	fprintf(stderr, "Error: could not write header\n");
	return 1;
    }

    return 0;

}

//alan
int copy_unchanged(short *buffer, int size, FILE **dest) {
    if ((fwrite(buffer, sizeof(short), size, *dest)) != size) {
        fprintf(stderr, "Could not write unchanged portion\n");
		return 1;	    
	}
   	return 0;
    // Write size bytes from buffer to file dest
    // Return 0 on success
    
}

int write_zeroes(FILE **dest, int size) {
    if(size <= 0) {
        return 0;
    }
    
    else {
        short zeroes[size];
	for(int i = 0; i < size; i++) {
	    zeroes[i] = 0;
	}
	
	int bytes_written;
	bytes_written = fwrite(zeroes, sizeof(short), size, *dest);
	if(bytes_written != size) {
	    fprintf(stderr, "Error: could not write into destination file\n");
            fprintf(stderr, "Wrote %d 0s (should be %d)\n", bytes_written, size);
	    return 1;
	}
	return 0;
	}
}

int mix_sample(short *buffer, int echo, int volume, FILE **dest) {
    // Read the first and echo element from buffer into dest
    // The first element should be scaled by volume
    // Return 0 on success
    short first = buffer[0] / volume;
    short last = buffer[echo];
    short write = first + last;
    
    if(fwrite(&write, sizeof(short), 1, *dest) != 1) {
        fprintf(stderr, "Error: could not mix sample\n");
        fprintf(stderr, "First: %d\nLast: %d\nWrite: %d\n", first, last, write);
        exit(1);
    }
    
    return 0;
}

int copy_buffer(short *buffer, int size, int volume, FILE **dest) {
    // Read size bytes from buffer to dest
    // Scale by volume
    // Return 0 on success
    short c;
    for(int i = 0; i < size; i++) {
        c = buffer[i] / volume;
        if(fwrite(&c, sizeof(short), 1, *dest) != 1) {
            fprintf(stderr, "Error: could not copy buffer at byte %d\n", i);
            return 1;
        }
    }

    return 0;
}

int main(int argc, char **argv) {

    int delay, volume, num_options;
    if(process_options(argc, argv, &delay, &volume, &num_options) != 0) {
        fprintf(stderr, "Error: could not read options\n");
        return 1;
    }

    if(argc != 3 + 2 * num_options) {
        fprintf(stderr, "Error: must have at least two commands\n");
        return 1;
    }
   
    FILE *sourcewav, *destwav;
    sourcewav = fopen(argv[1 + 2 * num_options], "rb");
    destwav = fopen(argv[2 + 2 * num_options], "wb");
    if(sourcewav == NULL) {
        fprintf(stderr, "Error: sourcewav could not be opened\n");
        return 1;
    }
    if(destwav == NULL) {
        fprintf(stderr, "Error: destwav could not be opened\n");
        return 1;
    }

    if(copy_header(&sourcewav, &destwav, delay) != 0) {
        fprintf(stderr, "Error: could not copy header\n");
        return 1;
    }
    int buffer_size = delay + 1;
    short *echo_buffer = malloc(buffer_size * sizeof(short));
    if(echo_buffer == NULL) {
        fprintf(stderr, "Error: Could not allocate %d shorts for echo buffer\n", delay);
    }
    int bytes_to_read = delay;
    int bytes_read = fread(echo_buffer, sizeof(short), bytes_to_read, sourcewav);
    if(copy_unchanged(echo_buffer, bytes_read, &destwav) != 0) {
        fprintf(stderr, "Error: could not copy original sound\n");
    }
    if(bytes_read != bytes_to_read) {
        buffer_size = bytes_read + 1;
        if(write_zeroes(&destwav, delay - bytes_read) != 0) {
            fprintf(stderr, "Error: could not read zeroes to destination\n");
        }
    } else {
        if(fseek(sourcewav, -1 * sizeof(short) * (bytes_read), SEEK_CUR) != 0) {
            fprintf(stderr, "Error: fseek failed\n");
            return 1;
        }

        while(fread(echo_buffer, sizeof(short), buffer_size, sourcewav) == buffer_size) {
            if(fseek(sourcewav, -1 * sizeof(short) * (buffer_size - 1), SEEK_CUR) != 0) {
                fprintf(stderr, "Error: fseek failed\n");
                return 1;
            }
            if(mix_sample(echo_buffer, delay, volume, &destwav) != 0) {
                fprintf(stderr, "Error: could not mix in delay\n");
            }
        }
    }

    if(copy_buffer(echo_buffer, buffer_size - 1, volume, &destwav) != 0) {
        fprintf(stderr, "Error: could not copy remaining buffer\n");
    }
    free(echo_buffer);

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

