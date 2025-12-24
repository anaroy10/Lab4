#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

char debug_mode = 0;
char file_name[128] = {0};
int unit_size = 1;

#define BUF_SIZE 10000
unsigned char mem_buf[BUF_SIZE];
size_t mem_count = 0;

char display_mode = 0; // Task 1b: 0 = hex, 1 = dec


typedef struct {
    char *name;
    void (*fun)();
} menu_item;

void toggle_debug() {
    debug_mode = !debug_mode;
    fprintf(stderr, "Debug flag now %s\n\n", debug_mode ? "on" : "off");
}

void set_file_name() {
    printf("Enter file name: ");
    fgets(file_name, sizeof(file_name), stdin);
    file_name[strcspn(file_name, "\n")] = '\0'; // remove newline
    if (debug_mode)
        fprintf(stderr, "Debug: file name set to '%s'\n\n", file_name);
}

void set_unit_size() {
    int size;
    printf("Enter unit size (1, 2, or 4): ");
    if (scanf("%d", &size) == 1) {
        if (size == 1 || size == 2 || size == 4) {
            unit_size = size;
            if (debug_mode)
                fprintf(stderr, "Debug: set size to %d\n\n", unit_size);
        } else {
            printf("Invalid unit size.\n\n");
        }
    } else {
        printf("Failed to read unit size.\n\n");
    }
    while (getchar() != '\n'); // clear input buffer
}

void not_implemented() {
    printf("Not implemented yet\n\n");
}

void quit() {
    if (debug_mode)
        fprintf(stderr, "quitting\n\n");
    exit(0);
}

//************************************ Task 1a ************************************
void loadIntoMemory() {
    if (strcmp(file_name, "") == 0) {
        perror("Error: file name is empty\n");
        return;
    }

    FILE *fp = fopen(file_name, "rb");
    if (!fp) {
        perror("Error opening file");
        return;
    }

    printf("Please enter <location> <length>\n");
    char input[128];
    fgets(input, sizeof(input), stdin);

    int location = 0, length = 0;
    if (sscanf(input, "%x %d", &location, &length) != 2) {
        printf("Invalid input format\n");
        fclose(fp);
        return;
    }

    if (debug_mode) {
        fprintf(stderr, "Debug: file_name='%s' location=0x%x length=%d\n", file_name, location, length);
    }

    fseek(fp, location, SEEK_SET);
    size_t to_read = length * unit_size;
    if (to_read > BUF_SIZE) to_read = BUF_SIZE;

    mem_count = fread(mem_buf, unit_size, length, fp);
    printf("Loaded %zu units into memory\n\n", mem_count);

    fclose(fp);
}

//************************************ Task 1b ************************************
void toggle_display_mode() {
    display_mode = !display_mode;
    if (display_mode)
        printf("Display flag now on, decimal representation\n\n");
    else
        printf("Display flag now off, hexadecimal representation\n\n");
}

//************************************ Task 1c ************************************
void memory_display() {
    printf("Enter address and length\n");

    char input[128];
    fgets(input, sizeof(input), stdin);

    unsigned int addr;
    int u;

    if (sscanf(input, "%x %d", &addr, &u) != 2) {
        printf("Invalid input\n\n");
        return;
    }

    unsigned char *start = (addr == 0) ? mem_buf : (unsigned char *)addr;

    printf("%s\n", display_mode ? "Decimal" : "Hexadecimal");
    printf("=======\n");

    for (int i = 0; i < u; i++) {
        if (display_mode) { // Decimal
            switch (unit_size) {
                case 1: printf("%d\n", *(start + i)); break;
                case 2: printf("%d\n", *((short *)(start + i * 2))); break;
                case 4: printf("%d\n", *((int *)(start + i * 4))); break;
            }
        } else { // Hexadecimal
            switch (unit_size) {
                case 1: printf("%02X\n", *(start + i)); break;
                case 2: printf("%04X\n", *((unsigned short *)(start + i * 2))); break;
                case 4: printf("%08X\n", *((unsigned int *)(start + i * 4))); break;
            }
        }
    }
    
    printf("\n");
}

//************************************ Task 1d ************************************
void saveIntoFile() {
    if (strcmp(file_name, "") == 0) {
        printf("Error: file name is empty\n\n");
        return;
    }

    int fd = open(file_name, O_RDWR);
    if (fd < 0) {
        perror("Error opening file");
        return;
    }

    printf("Please enter <source-address> <target-location> <length>\n");
    char input[128];
    fgets(input, sizeof(input), stdin);

    unsigned int source, target;
    int length;
    if (sscanf(input, "%x %x %d", &source, &target, &length) != 3) {
        printf("Invalid input format\n\n");
        close(fd);
        return;
    }

    if (debug_mode) {
        fprintf(stderr, "Debug: file_name='%s' source=0x%x target=0x%x length=%d\n", file_name, source, target, length);
    }

    size_t bytes_to_write = length * unit_size;
    off_t file_size = lseek(fd, 0, SEEK_END);
    if (target + bytes_to_write > (size_t)file_size) {
        printf("Error: target-location out of file bounds\n\n");
        close(fd);
        return;
    }

    unsigned char *src_ptr = (source == 0) ? mem_buf : (unsigned char *)source;
    lseek(fd, target, SEEK_SET);
    ssize_t written = write(fd, src_ptr, bytes_to_write);
    if (written < 0) {
        perror("Error writing to file");
    } else if ((size_t)written < bytes_to_write) {
        printf("Warning: wrote only %zd of %zu bytes\n\n", written, bytes_to_write);
    } else {
        printf("Wrote %d units into file\n\n", length);
    }

    close(fd);
}

//************************************ Task 1e ************************************
void memory_modify() {
    printf("Please enter <location> <val>\n");
    char input[128];
    fgets(input, sizeof(input), stdin);
    unsigned int location, val;
    if (sscanf(input, "%x %x", &location, &val) != 2) {
        printf("Invalid input format\n\n");
        return;
    }
    if (debug_mode)
        fprintf(stderr, "Debug: location=0x%x, val=0x%x\n", location, val);
    if (location >= mem_count) {
        printf("Error: location out of bounds\n\n");
        return;
    }
    unsigned char *target = mem_buf + location * unit_size;
    switch (unit_size) {
        case 1: *target = (unsigned char)val; break;
        case 2: *((unsigned short *)target) = (unsigned short)val; break;
        case 4: *((unsigned int *)target) = (unsigned int)val; break;
    }
    printf("Modified memory at location 0x%x to 0x%x\n\n", location, val);
}

menu_item menu[] = {
    {"Toggle Debug Mode", toggle_debug},
    {"Set File Name", set_file_name},
    {"Set Unit Size", set_unit_size},
    {"Load Into Memory", loadIntoMemory},
    {"Toggle Display Mode", toggle_display_mode},
    {"Memory Display", memory_display},
    {"Save Into File", saveIntoFile},
    {"Memory Modify", memory_modify},
    {"Quit\n", quit},
    {NULL, NULL}
};

int main() {
    while (1) {
        if (debug_mode)
            fprintf(stderr, "Debug: unit_size=%d file_name='%s' mem_count=%zu\n\n", unit_size, file_name, mem_count);

        printf("Choose action:\n");
        for (int i = 0; menu[i].name != NULL; i++)
            printf("%d-%s\n", i, menu[i].name);

        int choice = -1;
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input\n\n");
            while (getchar() != '\n'); // flush input
            continue;
        }
        while (getchar() != '\n'); // flush input

        if (choice >= 0 && menu[choice].fun)
            menu[choice].fun();
        else
            printf("Invalid choice\n\n");
    }
}