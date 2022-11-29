#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIDTH 800
#define HEIGHT 600

typedef unsigned int uint;

typedef struct color {
    int r;
    int g;
    int b;
} COLOR;

static COLOR pixels[WIDTH][HEIGHT];

static COLOR RED = { .r = 255, .g = 0, .b = 0 };
static COLOR GREEN = { .r = 0, .g = 255, .b = 0 };
static COLOR BLUE = { .r = 0, .g = 0, .b = 255 };

void
fill_with_color(COLOR *pixels, COLOR color)
{
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        pixels[i] = color;
    }
}

void
set_pixel_color(COLOR *pixels, uint x, uint y, COLOR color)
{
    if (x < WIDTH && y < HEIGHT)
        pixels[y * WIDTH + x] = color;
}

void
write_error(char *msg)
{
    printf("[\033[;31mERROR\033[0m] %s\n", msg);
}

void
write_success(char *msg)
{
    printf("[\033[;32mSUCCESS\033[0m] %s\n", msg);
}

int
write_ppm_file(COLOR *pixels, char *filename)
{
    FILE *file = fopen(filename, "w+");
    int ret_code = 0;

    if (file == NULL) {
        ret_code = -1;
        goto defer;
    }

    fprintf(file, "P3\n%d %d\n255\n", WIDTH, HEIGHT);

    for (int y = 0; y < HEIGHT; ++y) {
        for (int x = 0; x < WIDTH; ++x) {
            COLOR current = pixels[y * WIDTH + x];
            fprintf(file, "%d %d %d\n", current.r, current.g, current.b);
        }
    }

    if (ferror(file)) {
        ret_code = -1;
        goto defer;
    }

defer:
    if (file) fclose(file);
    return ret_code;
}

char *
parse_filename(int argc, char **argv)
{
    char *filename = "output.ppm";

    if (argc > 1) {
        filename = strncat(argv[argc - 1], ".ppm", 5);
        if (argc == 2)
            goto ret;
    } else {
        filename = "output.ppm";
        goto ret;
    }

    if (strncmp(argv[argc - 2], "-e", 3) == 0) {
        char *example_filename = (char *) malloc(512 * sizeof(char));
        strncpy(example_filename, "examples/", 10);
        filename = strncat(example_filename, filename, strlen(filename) + 1);
        goto ret;
    }

ret:
    return filename;
}

void
checkers()
{
    COLOR black = { .r = 0, .g = 0, .b = 0 };
    COLOR white = { .r = 255, .g = 255, .b = 255 };

    uint checker_w = WIDTH / 8;
    uint checker_h = HEIGHT / 6;
    int checker_idx = 0;

    int swap[] = {1, 0};
    COLOR lookup[] = {black, white};

    for (uint y = 0; y < HEIGHT; ++y) {
        for (uint x = 0; x < WIDTH; ++x) {
            if ((x % checker_w) == 0) {
                checker_idx = swap[checker_idx];
            }
            set_pixel_color(*pixels, x, y, lookup[checker_idx]);
        }
        if (y != 0 && (y % checker_h) == 0) {
            checker_idx = swap[checker_idx];
        } else if (checker_h % 2 != 0) {
            checker_idx = 1;
        }
    }
}

int 
main(int argc, char **argv) 
{
    char *filename = parse_filename(argc, argv);
    write_success(filename);

    checkers();

    if (write_ppm_file(*pixels, filename) == -1) {
        write_error("Failed to write file!");
        return 1;
    }

    char *info_msg = (char *) malloc(256 * sizeof(char));
    sprintf(info_msg, "Wrote %d pixels to %s", WIDTH * HEIGHT, filename);
    write_success(info_msg);
    free(info_msg);

    return 0;
}
