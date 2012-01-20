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
	void nameToML(string name){
		if(name.find("M")!=string::npos){
			if(name.find("L")!=string::npos){
				Mode=stoi(name.substr(name.find("M")+1,name.find("L")));
				Line=stoi(name.substr(name.find("L")+1,3));
			}
		}
	};
};

vector <tRoute> transitRoutes;
string AMPeriod, MDPeriod;

string itos(int number)
{
	stringstream ss;
	ss << number;
	return ss.str();
}

void splitN(tRoute &routeObj, string NLine)
{
	char* Ns;
	Ns=strtok((char*)NLine.c_str(),",& ");
	while(Ns != NULL){
		routeObj.nodeList.push_back(-1*stoi(Ns));
		Ns=strtok(NULL,",& ");
	}
}

void loadPTLin(string PTLinFile)
{
	ifstream linFileStream;
	linFileStream.open(PTLinFile);
	string line;
	int status=0;
	tRoute tmp;
	if(linFileStream.is_open())
	{
		while(linFileStream.good())
		{
			getline(linFileStream,line);
			if(line.length()>0)
			{
				if(line.compare(1,1,";")==0)
					goto skip;
restart:;
				switch(status){
				case 0:
					//begin line

					//Note that this expects the lines to be named as
					// MmLlll where m is the INET mode and lll is the INET line
					if(line.find("NAME=")!=string::npos)
					{
						int tmpS, tmpE;
						tmpS=line.find("NAME=")+6;
						tmpE=line.find(",",line.find("NAME="));
						tmp.nameToML(line.substr(tmpS,tmpE-tmpS-1));
					}
					tmp.Company=6;  // ~~~HARDCODE ALERT!!!
					if(line.find("HEADWAY[1]=")!=string::npos)
					{
						int tmpS, tmpE;
						tmpS=line.find("HEADWAY[1]=")+11;
						tmpE=line.find(",",line.find("HEADWAY[1]="));
						tmp.AMHeadway=stoi(line.substr(tmpS,tmpE-tmpS));
					}
					if(line.find("HEADWAY[2]=")!=string::npos)
					{
						int tmpS, tmpE;
						tmpS=line.find("HEADWAY[2]=")+11;
						tmpE=line.find(",",line.find("HEADWAY[2]="));
						tmp.MDHeadway=stoi(line.substr(tmpS,tmpE-tmpS));
					}
					if(line.find("LONGNAME=")!=string::npos)
					{
						int tmpS, tmpE;
						tmpS=line.find("LONGNAME=")+10;
						tmpE=line.find(",",line.find("LONGNAME="));
						tmp.LongName=line.substr(tmpS,tmpE-tmpS-1);
					}
					if(line.find("N=")!=string::npos)
					{
						status=1;
						int tmpS;
						tmpS=line.find("N=")+2;
						splitN(tmp,line.substr(tmpS,line.length()));
					}
					break;
				case 1:
					if(line.find("LINE ")==string::npos)
					{
						splitN(tmp,line);
					}else{
						transitRoutes.push_back(tmp);
						tmp.clear();
						status=0;
						goto restart;
					}
					break;
skip:;
				} //end switch
			} //end if line.length
		} //end while
	} //end if file open
} //end subroutine

void writeFile(string filename, int period)
{
	unsigned int col=70;
	ofstream outFile;
	outFile.open(filename);
	string outLine="";
	for(unsigned int t=0;t<transitRoutes.size();t++)
	{
		if((period==1 && transitRoutes[t].AMHeadway>0) || (period==2 && transitRoutes[t].MDHeadway>0))
		{


			outLine="&ROUTE M="+itos(transitRoutes[t].Mode)+", L="+itos(transitRoutes[t].Line)+", C="+itos(transitRoutes[t].Company)+", ONEWAY=";
			if(transitRoutes[t].OneWay)
				outLine+="T, HEADWAY=";
			else
				outLine+="F, HEADWAY=";
			if(period==1)
				outLine+=itos(transitRoutes[t].AMHeadway)+", PERIOD="+AMPeriod+", ID='";
			else
				outLine+=itos(transitRoutes[t].MDHeadway)+", PERIOD="+MDPeriod+", ID='";
			outLine+=transitRoutes[t].LongName+"', N=";
			for(unsigned int n=0;n<transitRoutes[t].nodeList.size();n++)
			{
				outLine+=itos(transitRoutes[t].nodeList[n]);
				if(n<transitRoutes[t].nodeList.size()-1)
					outLine+=", ";
				else
					outLine+=" &END";
			}
			//split the line into col-sized pieces
			vector <unsigned int> comma;
			while(outLine.length()>col){
				for(unsigned int sp=0;sp<=outLine.length();sp++)
				{
					if(outLine.substr(sp,1).compare(",")==0)
						comma.push_back(sp);
				}
				for(unsigned int c=0;c<comma.size();c++){
					if(comma[c]>col){
						outFile << outLine.substr(0,comma[c-1]+2) << endl;
						outLine="     "+outLine.substr(comma[c-1]+2,outLine.length());
						break;
					}
					if(c==comma.size()-1){
						outFile << outLine.substr(0,comma[c-1]+2) << endl;
						outLine="     "+outLine.substr(comma[c-1]+2,outLine.length());
					}
				}
				comma.clear();
			} // end comma stuff
			outFile << outLine << endl;
		} // end headway/period check
	}  // end route loop

	outFile.close();
}

int _tmain(int argc, _TCHAR* argv[])
{
	// *************************************************
	// * SETUP STUFF - THIS STUFF IS HARDCODED AND MAY *
	// * NEED TO BE ADJUSTED                           *
	// *************************************************
	AMPeriod="(0630,0830)";
	MDPeriod="(0830,1530)";
	
	if(argc<3){
		printf("\nPT LIN to INET Route converter\n");
		printf("Written by Andrew Rohne, OKI Regional Council, 2012.  Licensed under GPLv3\n\n");
		printf("Usage: \n     PT2INET <input LIN file> <output am transit file> [<output md transit file>] [<output pm transit file>]\n\n");
		return 0;
	}
	if(argc>=3){
		printf("Processing LIN File...\n");
		loadPTLin(argv[1]);
		//processAM((string)argv[2]);
		printf("Completed LIN File Processing.\n\n");
	}

	for(int f=2;f<argc;f++)
	{
		printf("Writing file %i of %i...\n",f-2,argc-2);
		writeFile(argv[f],f-1);
		//write(argv[f]);
	}
	printf("Completed!\n\n");
	return 0;
}

