#pragma once

struct Cinema
{
	int id;
	char name[16];
	long firstGoerAddress;
	int goersCount;
};

struct Goer
{
	int CinemaId;
	int ticketId;
	int ticketPrice;
	int exists;
	long selfAddress;
	long nextAddress;
};

struct Indexer
{
	int id;
	int address;
	int exists;
};