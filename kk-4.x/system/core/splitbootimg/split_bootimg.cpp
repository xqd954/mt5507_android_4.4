#include <stdio.h>
#include <string.h>

// Constants (from bootimg.h)
static const char* BOOT_MAGIC = "ANDROID!";
static const int BOOT_MAGIC_SIZE = 8;
static const int BOOT_NAME_SIZE = 16;
static const int BOOT_ARGS_SIZE = 512;

// Unsigned integers are 4 bytes
static const int UNSIGNED_SIZE = 4;

// The file name suffix
static const char* KERNEL = "-kernel";
static const char* RAMDISK = "-ramdisk.gz";
static const char* SECOND = "-second.gz";
static const int KERNEL_LEN = strlen(KERNEL);
static const int RAMDISK_LEN = strlen(RAMDISK);
static const int SECOND_LEN = strlen(SECOND);

// Format (from bootimg.h)
// +-------------------+
// |  boot header      |  1 page
// +-------------------+
// |  kernel           |  n pages
// +-------------------+
// |  ramdisk          |  m pages
// +-------------------+
// |  second stage     |  o pages
// +-------------------+
//
// n = (kernel_size + page_size - 1) / page_size
// m = (ramdisk_size + page_size - 1) / page_size
// o = (second_size + page_size - 1) / page_size

typedef struct
{
    unsigned char magic[BOOT_MAGIC_SIZE];

    unsigned kernel_size;                   // size in bytes
    unsigned kernel_addr;                   // physical load addr

    unsigned ramdisk_size;                  // size in bytes
    unsigned ramdisk_addr;                  // physical load addr

    unsigned second_size;                   // size in bytes
    unsigned second_addr;                   // physical load addr

    unsigned tags_addr;                     // physical addr for kernel tags
    unsigned page_size;                     // flash page size we assume
    unsigned unused[2];                     // future expansion: should be zero

    unsigned char name[BOOT_NAME_SIZE];     // asciiz product name

    unsigned char cmdline[BOOT_ARGS_SIZE];

    unsigned id[8];                         // timestamp checksum sha1 etc
}BOOT_IMG_HEADER, *PBOOT_IMG_HEADER;

void parse_file(const char* bootimg)
{
    FILE* f = fopen(bootimg, "rb+");
    if (!f)
    {
        return;
    }

    char* k_file = 0;
    char* r_file = 0;
    char* s_file = 0;
    do
    {
        BOOT_IMG_HEADER bih;
        if (fread(&bih, sizeof(BOOT_IMG_HEADER), 1, f) != 1)
        {
            break;
        }

        if (memcmp(bih.magic, BOOT_MAGIC, BOOT_MAGIC_SIZE) != 0)
        {
            break;
        }

        int n = (bih.kernel_size + bih.page_size - 1) / bih.page_size;
        int m = (bih.ramdisk_size + bih.page_size - 1) / bih.page_size;
        int o = (bih.second_size + bih.page_size - 1) / bih.page_size;

        int k_offset = bih.page_size;
        int r_offset = k_offset + n * bih.page_size;
        int s_offset = r_offset + m * bih.page_size;

        int bootlen = strlen(bootimg);

        k_file = new char[bootlen + KERNEL_LEN + 1];
        r_file = new char[bootlen + RAMDISK_LEN + 1];
        s_file = new char[bootlen + SECOND_LEN + 1];
        memcpy(k_file, bootimg, bootlen);
        memcpy(r_file, bootimg, bootlen);
        memcpy(s_file, bootimg, bootlen);
        memcpy(k_file + bootlen, KERNEL, KERNEL_LEN);
        memcpy(r_file + bootlen, RAMDISK, RAMDISK_LEN);
        memcpy(s_file + bootlen, SECOND, SECOND_LEN);
        k_file[bootlen + KERNEL_LEN] = 0;
        r_file[bootlen + RAMDISK_LEN] = 0;
        s_file[bootlen + SECOND_LEN] = 0;

        FILE* f_kernel = fopen(k_file, "wb+");
        fseek(f, k_offset, SEEK_SET);
        char* tmp = new char[bih.kernel_size];
        int count = fread(tmp, 1, bih.kernel_size, f);
        fwrite(tmp, 1, count, f_kernel);
        fclose(f_kernel);
        delete[] tmp;

        FILE* f_ramdisk = fopen(r_file, "wb+");
        fseek(f, r_offset, SEEK_SET);
        tmp = new char[bih.ramdisk_size];
        count = fread(tmp, 1, bih.ramdisk_size, f);
        fwrite(tmp, 1, count, f_ramdisk);
        fclose(f_ramdisk);
        delete[] tmp;

        if (bih.second_size != 0)
        {
            FILE* f_second = fopen(s_file, "wb+");
            fseek(f, s_offset, SEEK_SET);
            tmp = new char[bih.second_size];
            count = fread(tmp, 1, bih.second_size, f);
            fwrite(tmp, 1, count, f_second);
            fclose(f_second);
            delete[] tmp;
        }
    } while (0);

    delete[] k_file;
    delete[] r_file;
    delete[] s_file;
    fclose(f);
}

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        printf("Usage: split_bootimg file1\n");
        return 1;
    }

    parse_file(argv[1]);

    return 0;
}

