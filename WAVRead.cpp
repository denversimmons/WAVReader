#pragma	once
#include <fstream>
#include <iostream>
#include <stdint.h>
#pragma warning(disable:4996)
#define tableSize 100000

struct uint24
{
	unsigned int  int24 : 24;
};

float* readWAV(const char fileName[])
{
	std::cout << "Begin Reading " << fileName << std::endl;
	FILE *fp = NULL;
	fp = fopen(fileName, "r");
	if (!fp)
	{
		std::cout << "Error Reading WAV: Nullpointer" << std::endl;
		return nullptr;
	}

	char type[4];
	uint32_t size, chunk1Size, WAVsampleRate, avgBytesPerSec, chunk2Size;
	short formatType, channels, bytesPerSample, bitsPerSample;

	fread(type, sizeof(char), 4, fp);
	if (!strcmp(type, "RIFF"))
	{
		std::cout << "Error Reading WAV: RIFF" << std::endl;
		return nullptr;
	}

	fread(&size, sizeof(uint32_t), 1, fp);
	size -= 8;
	fread(type, sizeof(char), 4, fp);
	if (!strcmp(type, "WAVE"))
	{
		std::cout << "Error Reading WAV: WAVE" << std::endl;
		return nullptr;
	}
		
	fread(type, sizeof(char), 4, fp);
	if (!strcmp(type, "fmt"))
	{
		std::cout << "Error Reading WAV: fmt" << std::endl;
		return nullptr;
	}

	fread(&chunk1Size, sizeof(uint32_t), 1, fp);
	fread(&formatType, sizeof(short), 1, fp);
	fread(&channels, sizeof(short), 1, fp);
	fread(&WAVsampleRate, sizeof(uint32_t), 1, fp);
	fread(&avgBytesPerSec, sizeof(uint32_t), 1, fp);
	fread(&bytesPerSample, sizeof(short), 1, fp);
	fread(&bitsPerSample, sizeof(short), 1, fp);
	fread(type, sizeof(char), 4, fp);
	if (!strcmp(type, "data"))
	{
		std::cout << "Error Reading WAV" << std::endl;
		return nullptr;
	}

	fread(&chunk2Size, sizeof(uint32_t), 1, fp);

	int sample_size = bitsPerSample / 8;
	int samples_count = (chunk2Size * 8) / (channels * bitsPerSample);
	
	std::cout << "Chunk 1 Size: " << chunk1Size << std::endl;
	std::cout << "Format Type: " << formatType << std::endl;
	std::cout << "Channels: " << channels << std::endl;
	std::cout << "Sample Rate: " << WAVsampleRate << std::endl;
	std::cout << "Byte Rate: " << avgBytesPerSec << std::endl;
	std::cout << "Bytes Per Sample: " << bytesPerSample << std::endl;
	std::cout << "Bits Per Sample: " << bitsPerSample << std::endl;
	std::cout << "Chunk 2 Size: " << chunk2Size << std::endl;
	std::cout << "Sample Size: " << sample_size << std::endl;
	std::cout << "Samples Count: " << samples_count << std::endl;

	// Begin Parsing

	float* table = new float[samples_count];
	float fSamp;
	uint8_t iSamp8;
	int16_t iSamp16;
	uint24 iSamp24;
	int32_t iSamp32;

	if (formatType == 1)
	{
		for (int index = 0; index < (channels * samples_count); index++)
		{
			if (bitsPerSample == 8)
			{
				fread(&iSamp8, sample_size, 1, fp);
				fSamp = ((float)iSamp8 - 127.5f) / (float)127.5;
				if (fSamp > 1) fSamp = 1;					
				if (fSamp < -1) fSamp = -1;
			}
			if (bitsPerSample == 16)
			{
				fread(&iSamp16, sample_size, 1, fp);
				fSamp = ((float)iSamp16) / (float)32768;					
				if (fSamp > 1) fSamp = 1;
				if (fSamp < -1) fSamp = -1;
			}
			if (bitsPerSample == 24)
			{
				fread(&iSamp24, sample_size, 1, fp);
			//	fSamp = ((float)iSamp24) / (float)2147483648;
				if (fSamp > 1) fSamp = 1;
				if (fSamp < -1) fSamp = -1;
			}
			if (bitsPerSample == 32)
			{
				fread(&iSamp32, sample_size, 1, fp);
				fSamp = ((float)iSamp32) / (float)2147483648;
				if (fSamp > 1) fSamp = 1;
				if (fSamp < -1) fSamp = -1;
			}
			//table[index] = fSamp;
		}
	}

	if (formatType == 3)
	{
		if (channels == 1)
		{
			for (int index = 0; index < samples_count; index++)
			{
				fread(&table[index], sample_size, 1, fp);
			}
		}
		if (channels == 2)
		{
			for (int index = 0; index < (samples_count / 2); index++)
			{
				fread(&table[index], sample_size, 1, fp);
			}
			for (int index = (samples_count / 2); index < samples_count; index++)
			{
				fread(&table[index], sample_size, 1, fp);
			}
		}
	}

	fclose(fp);
	std::cout << "Finished" << std::endl;
	return table;
}

int main()
{
	float* table = readWAV("Bolly Snare.wav");
	return 0;
}