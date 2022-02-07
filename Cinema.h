#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Checks.h"
#include "Goer.h"

#define CINEMA_IND "cinema.ind"
#define CINEMA_DATA "cinema.fl"
#define CINEMA_GARBAGE "cinema_garbage.txt"
#define INDEXER_SIZE sizeof(struct Indexer)
#define CINEMA_SIZE sizeof(struct Cinema)

void noteDeletedCinema(int id)
{
	FILE* garbageZone = fopen(CINEMA_GARBAGE, "rb");		

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	int* delIds = malloc(garbageCount * sizeof(int));		

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);				
	}

	fclose(garbageZone);									
	garbageZone = fopen(CINEMA_GARBAGE, "wb");				
	fprintf(garbageZone, "%d", garbageCount + 1);			

	for (int i = 0; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);				
	}

	fprintf(garbageZone, " %d", id);						
	free(delIds);											
	fclose(garbageZone);									
}

void overwriteGarbageId(int garbageCount, FILE* garbageZone, struct Cinema* record)
{
	int* delIds = malloc(garbageCount * sizeof(int));		

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);				
	}

	record->id = delIds[0];

	fclose(garbageZone);									
	fopen(CINEMA_GARBAGE, "wb");							
	fprintf(garbageZone, "%d", garbageCount - 1);			

	for (int i = 1; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);				
	}

	free(delIds);											
	fclose(garbageZone);									
}

int insertCinema(struct Cinema record)
{
	FILE* indexTable = fopen(CINEMA_IND, "a+b");			
	FILE* database = fopen(CINEMA_DATA, "a+b");				
	FILE* garbageZone = fopen(CINEMA_GARBAGE, "rb");		
	struct Indexer indexer;
	int garbageCount;

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount)										
	{
		overwriteGarbageId(garbageCount, garbageZone, &record);

		fclose(indexTable);									
		fclose(database);									

		indexTable = fopen(CINEMA_IND, "r+b");				
		database = fopen(CINEMA_DATA, "r+b");				

		fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
		fread(&indexer, INDEXER_SIZE, 1, indexTable);
		fseek(database, indexer.address, SEEK_SET);			
	}
	else                                                    
	{
		long indexerSize = INDEXER_SIZE;

		fseek(indexTable, 0, SEEK_END);						

		if (ftell(indexTable))								
		{
			fseek(indexTable, -indexerSize, SEEK_END);		
			fread(&indexer, INDEXER_SIZE, 1, indexTable);	

			record.id = indexer.id + 1;						
		}
		else                                                
		{
			record.id = 1;									
		}
	}

	record.firstGoerAddress = -1;
	record.goersCount = 0;

	fwrite(&record, CINEMA_SIZE, 1, database);				

	indexer.id = record.id;									
	indexer.address = (record.id - 1) * CINEMA_SIZE;		
	indexer.exists = 1;										

	printf("Your cinema\'s id is %d\n", record.id);
	printf("% d\n", record.goersCount);

	fseek(indexTable, (record.id - 1) * INDEXER_SIZE, SEEK_SET);
	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);			
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int getCinema(struct Cinema* cinema, int id, char* error)
{
	FILE* indexTable = fopen(CINEMA_IND, "rb");				
	FILE* database = fopen(CINEMA_DATA, "rb");				

	if (!checkFileExistence(indexTable, database, error))
	{
		return 0;
	}

	struct Indexer indexer;

	if (!checkIndexExistence(indexTable, error, id))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);	
	fread(&indexer, INDEXER_SIZE, 1, indexTable);			

	if (!checkRecordExistence(indexer, error))
	{
		return 0;
	}

	fseek(database, indexer.address, SEEK_SET);				
	fread(cinema, sizeof(struct Cinema), 1, database);		
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int updateCinema(struct Cinema cinema, char* error)
{
	FILE* indexTable = fopen(CINEMA_IND, "r+b");			
	FILE* database = fopen(CINEMA_DATA, "r+b");				

	if (!checkFileExistence(indexTable, database, error))
	{
		return 0;
	}

	struct Indexer indexer;
	int id = cinema.id;

	if (!checkIndexExistence(indexTable, error, id))
	{
		return 0;
	}

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);	
	fread(&indexer, INDEXER_SIZE, 1, indexTable);			

	if (!checkRecordExistence(indexer, error))
	{
		return 0;
	}

	fseek(database, indexer.address, SEEK_SET);				
	fwrite(&cinema, CINEMA_SIZE, 1, database);				
	fclose(indexTable);										
	fclose(database);

	return 1;
}

int deleteCinema(int id, char* error)
{
	FILE* indexTable = fopen(CINEMA_IND, "r+b");			
															
	if (indexTable == NULL)
	{
		strcpy(error, "database files are not created yet");
		return 0;
	}

	if (!checkIndexExistence(indexTable, error, id))
	{
		return 0;
	}

	struct Cinema cinema;
	getCinema(&cinema, id, error);

	struct Indexer indexer;

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);	
	fread(&indexer, INDEXER_SIZE, 1, indexTable);			

	indexer.exists = 0;										

	fseek(indexTable, (id - 1) * INDEXER_SIZE, SEEK_SET);

	fwrite(&indexer, INDEXER_SIZE, 1, indexTable);			
	fclose(indexTable);										

	noteDeletedCinema(id);									


	if (cinema.goersCount)								
	{
		FILE* cinemasDb = fopen(GOER_DATA, "r+b");
		struct Goer goer;

		fseek(cinemasDb, cinema.firstGoerAddress, SEEK_SET);

		for (int i = 0; i < cinema.goersCount; i++)
		{
			fread(&goer, GOER_SIZE, 1, cinemasDb);
			fclose(cinemasDb);
			deleteGoer(cinema, goer, goer.ticketId, error);

			cinemasDb = fopen(GOER_DATA, "r+b");
			fseek(cinemasDb, goer.nextAddress, SEEK_SET);
		}

		fclose(cinemasDb);
	}
	return 1;
}