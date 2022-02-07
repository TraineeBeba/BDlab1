#pragma once

#include <stdio.h>
#include "Cinema.h"
#include "Structures.h"

void printCinema(struct Cinema cinema)
{
	printf("Cinema\'s name: %s\n", cinema.name);
}

void printGoer(struct Goer goer, struct Cinema cinema)
{
	printf("Cinema: %s\n", cinema.name);
	printf("Price: %d\n", goer.ticketPrice);
}