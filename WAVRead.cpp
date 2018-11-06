#pragma	once
#include <fstream>
#include <iostream>
#include <stdint.h>
#include "int24.h"
#pragma warning(disable:4996)

float* readWAV(const char fileName[])
{
	FILE *fp = nullptr;
	char type[4];
	uint32_t fileSize;			//Size of file in bytes - 8
	uint32_t chunk1Size;		//Size of format chunk in bytes
	uint32_t WAVsampleRate;		//Samplerate of file
	uint32_t avgBytesPerSec;	//Number of bytes per second (sampleRate * blockAlign)
	uint32_t chunk2Size;		//Size of data chunk in bytes
	short formatType;			//Format type. This program will only support 1 and 3
	short channels;				//Number of channels
	short blockAlign;			//Number of bytes in a frame (bytes per sample in ALL CHANNELS)			
	short bitsPerSample;		//Bit depth. This program will only support 8, 16, 24, and 32 bit depth

	std::cout << "Begin Reading " << fileName << std::endl;

	fp = fopen(fileName, "r");
	if (!fp)
	{
		return nullptr;
	}

	//Parse through format chunk

	fread(type, sizeof(char), 4, fp);
	if (!strcmp(type, "RIFF"))
	{
		return nullptr;
	}

	fread(&fileSize, sizeof(uint32_t), 1, fp);

	fread(type, sizeof(char), 4, fp);
	if (!strcmp(type, "WAVE"))
	{
		return nullptr;
	}
		
	fread(type, sizeof(char), 4, fp);
	if (!strcmp(type, "fmt"))
	{
		return nullptr;
	}

	fread(&chunk1Size, sizeof(uint32_t), 1, fp);

	fread(&formatType, sizeof(short), 1, fp);

	fread(&channels, sizeof(short), 1, fp);

	fread(&WAVsampleRate, sizeof(uint32_t), 1, fp);

	fread(&avgBytesPerSec, sizeof(uint32_t), 1, fp);

	fread(&blockAlign, sizeof(short), 1, fp);

	fread(&bitsPerSample, sizeof(short), 1, fp);

	fread(type, sizeof(char), 4, fp);
	if (!strcmp(type, "data"))
	{
		return nullptr;
	}

	fread(&chunk2Size, sizeof(uint32_t), 1, fp);

	int bytesPerSample = bitsPerSample / 8;
	int samples_count = (chunk2Size) / (blockAlign);
	
	std::cout << "Chunk 1 Size: " << chunk1Size << std::endl;
	std::cout << "Format Type: " << formatType << std::endl;
	std::cout << "Channels: " << channels << std::endl;
	std::cout << "Sample Rate: " << WAVsampleRate << std::endl;
	std::cout << "Byte Rate: " << avgBytesPerSec << std::endl;
	std::cout << "Block Align: " << blockAlign << std::endl;
	std::cout << "Bits Per Sample: " << bitsPerSample << std::endl;


	std::cout << "Chunk 2 Size: " << chunk2Size << std::endl;
	std::cout << "Samples Count: " << samples_count << std::endl;

	// Parse through data chunk

	float* table = new float[samples_count * channels + 1]; // store samples as floats. First space reserved for number of channels
	float fSamp;
	uint8_t bit8;
	int16_t bit16;
	Int24 bit24;
	float bit32;

	table[0] = channels;

	if (formatType == 1) // PCM
	{
		for (int index = 0; index < (channels * samples_count); index++)
		{
			if (bitsPerSample == 8)
			{
				fread(&bit8, bytesPerSample, 1, fp);
				fSamp = ((float)bit8 - 127.5f) / (float)127.5;
				if (fSamp > 1) fSamp = 1;					
				if (fSamp < -1) fSamp = -1;
			}
			else if (bitsPerSample == 16)
			{
				fread(&bit16, bytesPerSample, 1, fp);
				fSamp = ((float)bit16) / (float)32768;
				if (fSamp > 1) fSamp = 1;
				if (fSamp < -1) fSamp = -1;
			}
			else if (bitsPerSample == 24)
			{
				fread(&bit24, bytesPerSample, 1, fp);
				fSamp = ((float)bit24) / (float)(8388607);
				if (fSamp > 1) fSamp = 1;
				if (fSamp < -1) fSamp = -1;
			}
			else if (bitsPerSample == 32)
			{
				fread(&bit32, bytesPerSample, 1, fp);
				fSamp = bit32;
			}
			else
			{
				std::cout << "Unsupported bit depth: " << bitsPerSample << std::endl;
				return nullptr;
			}
			table[index+1] = fSamp;
		}
	}

	if (formatType == 3) // Incomplete!!!!!!!
	{
		if (channels == 1)
		{
			for (int index = 0; index < samples_count; index++)
			{
				fread(&table[index], bytesPerSample, 1, fp);
			}
		}
		if (channels == 2)
		{
			for (int index = 0; index < (samples_count / 2); index++)
			{
				fread(&table[index], bytesPerSample, 1, fp);
			}
			for (int index = (samples_count / 2); index < samples_count; index++)
			{
				fread(&table[index], bytesPerSample, 1, fp);
			}
		}
	}

	fclose(fp);
	std::cout << "Finished" << std::endl;
	return table;
}

int main()
{
	float* table = readWAV("JPGC_vocal_cats.wav");
	return 0;
}
