#pragma once
#pragma warning(disable:4996)

#include <stdio.h>
#include <stdlib.h>
#include "Structures.h"
#include "Checks.h"
#include "Cinema.h"

#define GOER_DATA "goer.fl"
#define GOER_GARBAGE "goer_garbage.txt"
#define GOER_SIZE sizeof(struct Goer)

void reopenDatabase(FILE* database)
{
	fclose(database);
	database = fopen(GOER_DATA, "r+b");
}

void linkAddresses(FILE* database, struct Cinema cinema, struct Goer goer)
{
	reopenDatabase(database);								

	struct Goer previous;

	fseek(database, cinema.firstGoerAddress, SEEK_SET);

	for (int i = 0; i < cinema.goersCount; i++)		    
	{
		fread(&previous, GOER_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	}

	previous.nextAddress = goer.selfAddress;				
	fwrite(&previous, GOER_SIZE, 1, database);				
}

void relinkAddresses(FILE* database, struct Goer previous, struct Goer goer, struct Cinema* cinema)
{
	if (goer.selfAddress == cinema->firstGoerAddress)		
	{
		if (goer.selfAddress == goer.nextAddress)			
		{
			cinema->firstGoerAddress = -1;					
		}
		else                                                
		{
			cinema->firstGoerAddress = goer.nextAddress;  
		}
	}
	else                                                    
	{
		if (goer.selfAddress == goer.nextAddress)			
		{
			previous.nextAddress = previous.selfAddress;    
		}
		else                                                
		{
			previous.nextAddress = goer.nextAddress;		
		}

		fseek(database, previous.selfAddress, SEEK_SET);	
		fwrite(&previous, GOER_SIZE, 1, database);			
	}
}

void noteDeletedGoer(long address)
{
	FILE* garbageZone = fopen(GOER_GARBAGE, "rb");			

	int garbageCount;
	fscanf(garbageZone, "%d", &garbageCount);

	long* delAddresses = malloc(garbageCount * sizeof(long)); 

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%ld", delAddresses + i);		
	}

	fclose(garbageZone);									
	garbageZone = fopen(GOER_GARBAGE, "wb");				
	fprintf(garbageZone, "%ld", garbageCount + 1);			

	for (int i = 0; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %ld", delAddresses[i]);		
	}

	fprintf(garbageZone, " %d", address);					
	free(delAddresses);										
	fclose(garbageZone);									
}

void overwriteGarbageAddress(int garbageCount, FILE* garbageZone, struct Goer* record)
{
	long* delIds = malloc(garbageCount * sizeof(long));		

	for (int i = 0; i < garbageCount; i++)
	{
		fscanf(garbageZone, "%d", delIds + i);				
	}

	record->selfAddress = delIds[0];						
	record->nextAddress = delIds[0];

	fclose(garbageZone);									
	fopen(GOER_GARBAGE, "wb");							    
	fprintf(garbageZone, "%d", garbageCount - 1);			

	for (int i = 1; i < garbageCount; i++)
	{
		fprintf(garbageZone, " %d", delIds[i]);				
	}

	free(delIds);											
	fclose(garbageZone);									
}

int insertGoer(struct Cinema cinema, struct Goer goer, char* error)
{
	goer.exists = 1;

	FILE* database = fopen(GOER_DATA, "a+b");
	FILE* garbageZone = fopen(GOER_GARBAGE, "rb");

	int garbageCount;

	fscanf(garbageZone, "%d", &garbageCount);

	if (garbageCount)										
	{
		overwriteGarbageAddress(garbageCount, garbageZone, &goer);
		reopenDatabase(database);							
		fseek(database, goer.selfAddress, SEEK_SET);		
	}
	else                                                    
	{
		fseek(database, 0, SEEK_END);

		int dbSize = ftell(database);

		goer.selfAddress = dbSize;
		goer.nextAddress = dbSize;
	}

	fwrite(&goer, GOER_SIZE, 1, database);					

	if (!cinema.goersCount)								    
	{
		cinema.firstGoerAddress = goer.selfAddress;
	}
	else                                                    
	{
		linkAddresses(database, cinema, goer);
	}

	fclose(database);											

	cinema.goersCount++;										
	updateCinema(cinema, error);								

	return 1;
}

int getGoer(struct Cinema cinema, struct Goer* goer, int ticketId, char* error)
{
	if (!cinema.goersCount)								
	{
		strcpy(error, "This cinema has no goer yet");
		return 0;
	}

	FILE* database = fopen(GOER_DATA, "rb");


	fseek(database, cinema.firstGoerAddress, SEEK_SET);		
	fread(goer, GOER_SIZE, 1, database);

	for (int i = 0; i < cinema.goersCount; i++)				
	{
		if (goer->ticketId == ticketId)						
		{
			fclose(database);
			return 1;
		}

		fseek(database, goer->nextAddress, SEEK_SET);
		fread(goer, GOER_SIZE, 1, database);
	}

	strcpy(error, "No such goer in database");				
	fclose(database);
	return 0;
}

int updateGoer(struct Goer goer, int ticketId)
{
	FILE* database = fopen(GOER_DATA, "r+b");

	fseek(database, goer.selfAddress, SEEK_SET);
	fwrite(&goer, GOER_SIZE, 1, database);
	fclose(database);

	return 1;
}

int deleteGoer(struct Cinema cinema, struct Goer goer, int ticketId, char* error)
{
	FILE* database = fopen(GOER_DATA, "r+b");
	struct Goer previous;

	fseek(database, cinema.firstGoerAddress, SEEK_SET);

	do		                                                    
	{															
		fread(&previous, GOER_SIZE, 1, database);
		fseek(database, previous.nextAddress, SEEK_SET);
	} while (previous.nextAddress != goer.selfAddress && goer.selfAddress != cinema.firstGoerAddress);

	relinkAddresses(database, previous, goer, &cinema);
	noteDeletedGoer(goer.selfAddress);						

	goer.exists = 0;										

	fseek(database, goer.selfAddress, SEEK_SET);			
	fwrite(&goer, GOER_SIZE, 1, database);					
	fclose(database);

	cinema.goersCount--;									
	updateCinema(cinema, error);

}