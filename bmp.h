/*
 * bmp.h
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
 */

#ifndef bmp_h
#define bmp_h

typedef struct __attribute__((packed)) {
    uint16_t type;     // Magic identifier
    uint32_t size;     // File size in bytes
    uint32_t reserved; // Reserved bytes
    uint32_t offset;   // Offset to image data, bytes
} bmp_header_t;

typedef struct __attribute__((packed)) {
    uint32_t size;              // Header size in bytes
    uint32_t width;             // Width of image
    uint32_t height;            // Height of image
    uint16_t planes;            // Number of colour planes
    uint16_t bits;              // Bits per pixel
    uint32_t compression;       // Compression type
    uint32_t image_size;        // Image size in bytes
    int32_t x_resolution;       // Pixels per meter
    int32_t y_resolution;       // Pixels per meter
    uint32_t n_colours;         // Number of colours
    uint32_t important_colours; // Important colours
} bmp_info_header_t;

typedef struct __attribute__((packed)) {
    uint8_t r;
    uint8_t g;
    uint8_t b;
} bmp_pixel_t;

typedef struct __attribute__((packed)) {
    bmp_header_t header;
    bmp_info_header_t info_header;
} bmp_t;

#endif /* bmp_h */
