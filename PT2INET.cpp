// PT2INET.cpp : Defines the entry point for the console application.
//
// Route converter for PT LIN to INET
// Author: Andrew Rohne arohne@oki.org Licensed open source 2012 GPLv3

#include "stdafx.h"
#include "stdafx.h"
#include <string>
#include <vector>
#include <stdio.h>
#include <fstream>
#include <sstream>

using namespace std;

class tRoute{
public:
	int Mode;
	int Line;
	int Company;
	int AMHeadway;
	int MDHeadway;
	int PMHeadway;
	int RG; // <~ This is put into USERN2
	string LongName;
	bool OneWay;
	bool Circular;
	vector <int> nodeList;
	tRoute(){
		Mode=0;
		Line=0;
		Company=0;
		AMHeadway=0;
		MDHeadway=0;
		PMHeadway=0;
		RG=0;
		LongName="";
		OneWay=false;
		Circular=false;
		nodeList.clear();
	};
	~tRoute(){
		Mode=0;
		Line=0;
		Company=0;
		AMHeadway=0;
		MDHeadway=0;
		PMHeadway=0;
		RG=0;
		LongName="";
		OneWay=false;
		Circular=false;
		nodeList.clear();
	};
	void clear(){
		Mode=0;
		Line=0;
		Company=0;
		AMHeadway=0;
		MDHeadway=0;
		PMHeadway=0;
		RG=0;
		LongName="";
		OneWay=false;
		Circular=false;
		nodeList.clear();
	};
};

vector <tRoute> transitRoutes;

int _tmain(int argc, _TCHAR* argv[])
{
	if(argc<3){
		printf("\nPT LIN to INET Route converter\n");
		printf("Written by Andrew Rohne, OKI Regional Council, 2012.  Licensed under GPLv3\n\n");
		printf("Usage: \n     PT2INET <input LIN file> <output am transit file> [<output md transit file>] [<output pm transit file>]\n\n");
		return 0;
	}
		if(argc>=3){
		printf("Processing AM...\n");
		//processAM((string)argv[2]);
		printf("Completed AM.\n\n");
	}
	if(argc>=4){
		printf("Processing MD...\n");
		//processMDPM((string)argv[3],0);
		printf("Completed MD.\n\n");
	}
	if(argc>=5){
		printf("Processing PM...\n");
		//processMDPM((string)argv[4],1);
		printf("Completed PM.\n\n");
	}
	printf("Writing output...\n");
	//writePTOut((string)argv[1]);
	printf("Completed!\n\n");
	return 0;
}

