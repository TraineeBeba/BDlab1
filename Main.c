#include <stdio.h>
#include "Structures.h"
#include "Cinema.h"
#include "Goer.h"
#include "Read.h"
#include "Print.h"


int main()
{
	struct Cinema cinema;
	struct Goer goer;

	while (1)
	{
		int choice;
		int id;
		char error[51];

		printf("Choose option:\n0 - Quit\n1 - Insert Cinema\n2 - Get Cinema\n3 - Update Cinema\n4 - Delete Cinema\n5 - Insert Goer\n6 - Get Goer\n7 - Update Goer\n8 - Delete Goer\n9 - Info\n");
		scanf("%d", &choice);

		switch (choice)
		{
		case 0:
			return 0;

		case 1:
			readCinema(&cinema);
			insertCinema(cinema);
			break;

		case 2:
			printf("Enter ID: ");
			scanf("%d", &id);
			getCinema(&cinema, id, error) ? printCinema(cinema) : printf("Error: %s\n", error);
			break;

		case 3:
			printf("Enter ID: ");
			scanf("%d", &id);

			cinema.id = id;

			readCinema(&cinema);
			updateCinema(cinema, error) ? printf("Updated successfully\n") : printf("Error: %s\n", error);
			break;

		case 4:
			printf("Enter ID: ");
			scanf("%d", &id);
			deleteCinema(id, error) ? printf("Deleted successfully\n") : printf("Error: %s\n", error);
			break;

		case 5:
			printf("Enter cinema\'s ID: ");
			scanf("%d", &id);

			if (getCinema(&cinema, id, error))
			{
				goer.CinemaId = id;
				printf("Enter product ID: ");
				scanf("%d", &id);

				goer.ticketId = id;
				readGoer(&goer);
				insertGoer(cinema, goer, error);
				printf("Inserted successfully. To access, use cinema\'s and ticket\'s IDs\n");
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 6:
			printf("Enter cinema\'s ID: ");
			scanf("%d", &id);

			if (getCinema(&cinema, id, error))
			{
				printf("Enter product ID: ");
				scanf("%d", &id);
				getGoer(cinema, &goer, id, error) ? printGoer(goer, cinema) : printf("Error: %s\n", error);
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 7:
			printf("Enter cinema\'s ID: ");
			scanf("%d", &id);

			if (getCinema(&cinema, id, error))
			{
				printf("Enter ticket ID: ");
				scanf("%d", &id);

				if (getGoer(cinema, &goer, id, error))
				{
					readGoer(&goer);
					updateGoer(goer, id, error);
					printf("Updated successfully\n");
				}
				else
				{
					printf("Error: %s\n", error);
				}
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 8:
			printf("Enter cinema\'s ID: ");
			scanf("%d", &id);

			if (getCinema(&cinema, id, error))
			{
				printf("Enter ticket ID: ");
				scanf("%d", &id);

				if (getGoer(cinema, &goer, id, error))
				{
					deleteGoer(cinema, goer, id, error);
					printf("Deleted successfully\n");
				}
				else
				{
					printf("Error: %s\n", error);
				}
			}
			else
			{
				printf("Error: %s\n", error);
			}
			break;

		case 9:
			info();
			break;

		default:
			printf("Invalid input, please try again\n");
		}

		printf("---------\n");
	}

	return 0;
}