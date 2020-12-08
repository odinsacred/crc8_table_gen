#include<stdint.h>
#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<ctype.h>
#include <string.h>

#define TABLE_SIZE 256
#define STRING_LENGTH 16
#define STRING_NUM TABLE_SIZE / STRING_LENGTH

uint8_t table[TABLE_SIZE] = { 0 };

bool create_crc8_table(uint8_t poly, uint8_t init, bool msb, uint8_t xorout);
uint8_t msb_crc8(uint8_t data, uint8_t init, uint8_t poly, uint8_t xorout);
uint8_t lsb_crc8(uint8_t data, uint8_t init, uint8_t poly, uint8_t xorout);

void save_crc_table(char *filename) {
	int offset = 0;
	FILE* file = NULL;
	errno_t er = fopen_s(&file,filename, "w");
	if (file) {
		fprintf(file, "#ifndef FILE_H_\n");		
		fprintf(file, "#define FILE_H_\n\n");
		fprintf(file, "#include<stdint.h>\n\n");
		fprintf(file, "#define TABLE_SIZE 256\n\n\n\n");
		fprintf(file, "uint8_t crc8_table[TABLE_SIZE] = {\n");
		for (uint8_t i = 0; i < STRING_NUM; i++) {
			offset = i * 16;
			for (uint8_t j = 0; j < STRING_LENGTH; j++) {
				if(i == STRING_NUM-1 && j == STRING_LENGTH-1)
					fprintf(file, "%#x", table[j + offset]);
				else
					fprintf(file, "%#x, ",table[j+offset]);
			}
			fprintf(file, "\n");
		}
		fprintf(file, "};\n\n");
		fprintf(file, "#endif /*FILE_H_*/");
		fclose(file);
	}
	
}

long pow(uint8_t value, uint8_t exp) {
	long power = value;
	for (uint8_t i = 0; i < exp; i++) {
		power *= value;
	}
	return power;
}

int htoi(char* str, uint8_t length) {
	uint8_t value = 0;
	long weight = 1; // = 1, 16, 256
	uint8_t pos = 0;
	for (int8_t i = length-1; i >= 0; i--) {
		uint8_t digit = 0;
		if (isdigit(str[i])) {
			digit = atoi(&str[i]);			
		}
		else if (str[i] > 0x40 && str[i] < 0x47) {
			digit = (str[i] - 0x40) + 9;
		}
		else {
			return -1;
		}
		value += digit * weight;		
		weight = pow(16, pos);
		pos++;
	}
	return value;
}

/* generator.exe, filename, poly,init,refin,xorout*/
int main(int argc, char* argv[]) {
	//long a = htoi("2A",strlen("2A"));
	if (argc < 6)
	{
		printf("too few arguments... program terminated\n");
		return EXIT_FAILURE;
	}

	char* filename = argv[1];
	if (create_crc8_table(htoi(argv[2],strlen(argv[2])), atoi(argv[3]), atoi(argv[4]), atoi(argv[5]))) {
		save_crc_table(filename);
		printf("generation complete!\n");
		return EXIT_SUCCESS;
	}
	else {
		printf("generation failed (poly == 0)\n");
		return EXIT_FAILURE;
	}
	
}

bool create_crc8_table(uint8_t poly, uint8_t init, bool msb, uint8_t xorout) {
	if (poly == 0)
		return false;
	if (msb) {
		for (uint16_t table_index = 0; table_index < TABLE_SIZE; table_index++) {
			table[table_index] = msb_crc8(table_index, init, poly, xorout);
		}
	}
	else {
		for (uint16_t table_index = 0; table_index < TABLE_SIZE; table_index++) {
			table[table_index] = lsb_crc8(table_index, init, poly, xorout);
		}
	}
	return true;
}

uint8_t msb_crc8(uint8_t data, uint8_t init, uint8_t poly, uint8_t xorout) {
	uint8_t crc = init;
	crc ^= data;
	for (uint8_t bit_count = 0; bit_count < 8; bit_count++) {
		if (crc & 0x80) {
			crc <<= 1;
			crc ^= poly;
		}
		else {
			crc <<= 1;
		}
	}
	return crc ^ xorout;
}

uint8_t lsb_crc8(uint8_t data, uint8_t init, uint8_t poly, uint8_t xorout) {
	uint8_t crc = init;
	crc ^= data;
	for (uint8_t bit_count = 0; bit_count < 8; bit_count++) {
		if (crc & 0x1) {
			crc >>= 1;
			crc ^= poly;
		}
		else {
			crc >>= 1;
		}
	}
	return crc ^ xorout;
}