/*
 * mrhyde.c
 *
 * Copyright (C) 2012 - Giovan Battista Rolandi
 * www.glgprograms.it - giomba@live.it
 *
 * Copyright (C) 2016 - Simone Ferrini
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 *
 * Purpose
 * The program hides text messages in bitmap images with color depth of 24bpp,
 * and is able to decode them later.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "bmp.h"
#include "bit.h"

static bmp_t *mh_map_bmp(char *path) {
    struct stat sb;
    FILE *fp = fopen(path, "r");
    if (!fp) return NULL;
    int fd = fileno(fp);
    fstat(fd, &sb);
    return (bmp_t *)mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
}

static void mh_write_bmp(char *path, bmp_t *bmp_map) {
    FILE *fp = fopen(path, "w");
    fwrite((void *)bmp_map, 1, bmp_map->header.size, fp);
    fclose(fp);
}

static void mh_hide_in_bmp(char *hide, bmp_t *bmp_map)
{
    uint32_t offset = bmp_map->header.offset;
    uint32_t total_pixel = bmp_map->info_header.width * bmp_map->info_header.height;
    
    bmp_pixel_t *pixel = (bmp_pixel_t *)&((char *)bmp_map)[offset];
    for (uint32_t i = 0; i < total_pixel; pixel++, i++) {
        if (hide[i / BITS_IN_ONE_BYTE] == 0) {
            for (uint32_t i = 0; i < BITS_IN_ONE_BYTE; i++) {
                pixel[i].r = 0;
            }
            break;
        }
        BIT_CHECK(hide[i / BITS_IN_ONE_BYTE], i % BITS_IN_ONE_BYTE) ? BIT_SET(pixel->r, 0) : BIT_CLEAR(pixel->r, 0);
    }
}

static char *mh_reveal_from_bmp(bmp_t *bmp_map)
{
    uint32_t offset = bmp_map->header.offset;
    uint32_t total_pixel = bmp_map->info_header.width * bmp_map->info_header.height;
    char *reveal = (char *)malloc(total_pixel / BITS_IN_ONE_BYTE);
    
    bmp_pixel_t *pixel = (bmp_pixel_t *)&((char *)bmp_map)[offset];
    for (uint32_t i = 0; i < total_pixel; pixel++, i++) {
        BIT_CHECK(pixel->r, 0) ? BIT_SET(reveal[i / BITS_IN_ONE_BYTE], i % BITS_IN_ONE_BYTE) : BIT_CLEAR(reveal[i / BITS_IN_ONE_BYTE], i % BITS_IN_ONE_BYTE);
        if (i != 0 && !(i % BITS_IN_ONE_BYTE) && reveal[(i / BITS_IN_ONE_BYTE) - 1] == 0) {
            return realloc(reveal, (i / BITS_IN_ONE_BYTE) - 1);
        }
    }
    
    return NULL;
}

static void mh_usage(void)
{
    printf("Usage:\n");
    printf(" mrhide bmp_path \"message\"\n");
    printf(" mrhide bmp_path\n");
}

static void mh_error(void)
{
    printf("Error! Check your params.\n");
}

int main(int argc, const char * argv[]) {
    
    if (argc < 2 || argc > 3) {
        mh_usage();
        return 0;
    }

    bmp_t *bmp_map = mh_map_bmp((char *)argv[1]);
    
    if (!bmp_map) {
        mh_error();
        return -1;
    }
    
    if (argc >= 3) {
        mh_hide_in_bmp((char *)argv[2], bmp_map);
        mh_write_bmp((char *)argv[1], bmp_map);
    } else {
        printf("%s\n", mh_reveal_from_bmp(bmp_map));
    }
    
    return 0;
}
