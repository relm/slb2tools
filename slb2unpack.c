// Copyright (C) 2013       Hykem <hykem@hotmail.com>
// Licensed under the terms of the GNU GPL, version 2
// http://www.gnu.org/licenses/gpl-2.0.txt

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SLB2_MAGIC        0x32424C53 // SLB2
#define SLB2_HEADER_SIZE  0x200
#define READ_BUFFER_SIZE  512

// PUP file entry (size == 0x30):
// 0x00: 01 00 00 00 -> Offset (in blocks, so 1 is the first block of 512 bytes after the header)
// 0x04: 00 76 AE 0D -> File size
// 0x08: 00 00 00 00 -> Unknown
// 0x0C: 00 00 00 00 -> Unknown
// 0x10: 50 53 34 55 -> File name (e.g.: PS4UPDATE1.PUP) (0x20 bytes)
// 0x14: 50 44 41 54 
// 0x18: 45 31 2E 50 
// 0x1C: 55 50 00 00 
// 0x20: 00 00 00 00 
// 0x24: 00 00 00 00 
// 0x28: 00 00 00 00 
// 0x2C: 00 00 00 00
struct slb2_entry {
	uint32_t block_offset;
	uint32_t file_size;
	uint32_t unk[2];
	char     file_name[32];
} __attribute__((packed));

// Main SLB2 packed header (size == 0x20):
// 0x00: 53 4C 42 32 -> SLB2 
// 0x04: 01 00 00 00 -> Version?
// 0x08: 00 00 00 00 -> Unknown
// 0x0C: 02 00 00 00 -> Number of PUP files in this pack
// 0x10: 03 9F 09 00 -> Total number of blocks (512 bytes)
// 0x14: 00 00 00 00 -> Unknown
// 0x18: 00 00 00 00 -> Unknown
// 0x1C: 00 00 00 00 -> Unknown
struct slb2_header {
	uint32_t magic;
	uint32_t version;
	uint32_t flags;
	uint32_t file_count;
	uint32_t total_blocks;
	uint32_t unk[3];
	struct slb2_entry entry_list[0];
} __attribute__((packed));

int main (int argc, char *argv[])
{
	FILE *in;
	FILE *out;
	struct slb2_header *slb2;

	if (argc < 2) {
		printf("Usage: %s file\n", argv[0]);
		return 0;
	}
	
	// Open file and set up the header struct.
	if ((slb2 = malloc(SLB2_HEADER_SIZE)) == NULL) {
		perror("main");
		return 1;
	}

	if ((in = fopen(argv[1], "rb")) == NULL) {
		perror("open");
		return 1;
	}
	
	// Read in the main pack header.
	if (fread(slb2, sizeof(*slb2), 1, in) < 1) {
		perror("read");
		return 1;
	}
	
	if (slb2->magic != SLB2_MAGIC) {
		printf("Invalid SLB2 file!\n");
		return 1;
	}
	
	printf("SLB2 pack header:\n");
	printf("- SLB2 magic: 0x%X\n", slb2->magic);
	printf("- SLB2 version: %i\n", slb2->version);
	printf("- Files in this pack: %i\n", slb2->file_count);
	printf("- Total number of blocks: %i\n", slb2->total_blocks);
	printf("\n");
	
	// Read in all the PUP entries.
	if (sizeof(*slb2) + slb2->file_count * sizeof(struct slb2_entry) > SLB2_HEADER_SIZE) {
		fprintf(stderr, "main: number of entries exceed header size\n");
		return 1;
	}

	if (fread(&slb2->entry_list[0], sizeof(struct slb2_entry), slb2->file_count, in) < slb2->file_count) {
		perror("read");
		return 1;
	}

	// Create a large enough buffer and start copying the data.
	size_t i;
	size_t fsize;
	size_t bytes_read;
	size_t slb2_offset = SLB2_HEADER_SIZE;
	uint8_t buffer[READ_BUFFER_SIZE];
	
	for (i = 0; i < slb2->file_count; ++i) {
		if (fseek(in, slb2_offset, SEEK_SET) < 0) {
			perror("read");
			return 1;
		}
		if ((out = fopen(slb2->entry_list[i].file_name, "wb")) == NULL) {
			perror("open");
			return 1;
		}

		printf("Dumping SLB2 file entry %zi:\n", i);
		printf("- Block offset: 0x%X\n", slb2->entry_list[i].block_offset);
		printf("- File size: %i\n", slb2->entry_list[i].file_size);
		printf("- File name: %s\n", slb2->entry_list[i].file_name);
		printf("- SLB2 offset: 0x%zX\n", slb2_offset);
		printf("\n");
		
		fsize = slb2->entry_list[i].file_size;
		slb2_offset += (fsize + 511) & ~511; // 512 bytes alignment.
		
		while ((bytes_read = fread(buffer, sizeof(uint8_t), fsize > READ_BUFFER_SIZE ? READ_BUFFER_SIZE : fsize, in)) > 0) {
			if (fwrite(buffer, sizeof(uint8_t), bytes_read, out) < bytes_read) {
				perror("write");
				return 1;
			}
			fsize -= bytes_read;
		}
	
		fclose(out);
	}	

	fclose(in);
	free(slb2);
	printf("Finished!\n");
	
	return 0;
}
