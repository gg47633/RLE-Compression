#include <stdio.h>
#include <stdlib.h>

void compress_stream(FILE *input, FILE *output) {
    int count = 0;
    char current, previous = '\0';
    int first = 1;

    while ((current = fgetc(input)) != EOF) {
        if (first) {
            previous = current;
            count = 1;
            first = 0;
        } else if (current == previous) {
            count++;
        } else {
            fwrite(&count, sizeof(count), 1, output);
            fputc(previous, output);
            previous = current;
            count = 1;
        }
    }

    if (!first) {
        fwrite(&count, sizeof(count), 1, output);
        fputc(previous, output);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("wzip: file1 [file2 ...]\n");
        exit(1);
    }

    FILE *temp = tmpfile();
    if (!temp) {
        perror("Error creating temporary file");
        exit(1);
    }

    for (int i = 1; i < argc; i++) {
        FILE *input = fopen(argv[i], "rb");
        if (!input) {
            perror("Error opening file");
            continue;
        }

        char buffer[1000];
        size_t bytes_read;
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), input)) > 0) {
            fwrite(buffer, 1, bytes_read, temp);
        }

        fclose(input);
    }

    rewind(temp);

    compress_stream(temp, stdout);

    fclose(temp);

    return 0;
}
