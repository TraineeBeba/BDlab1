#pragma once
#include <stdio.h>
#include <string.h>
#include "Structures.h"
#include "Cinema.h"

void readCinema(struct Cinema* cinema)
{
	char name[16];

	name[0] = '\0';

	printf("Enter cinema\'s name: ");
	scanf("%s", name);

	strcpy(cinema->name, name);
}

void readGoer(struct Goer* goer)
{
	int x;

	printf("Enter ticketPrice: ");
	scanf("%d", &x);

	goer->ticketPrice = x;
}