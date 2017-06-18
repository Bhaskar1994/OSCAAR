/*
	int totalstepnumber;
	totalstepnumber=20;
	/home/bm1994/Desktop/OSCAAR/readcgns ver5/results/step_1/y.txt
	/home/bm1994/Desktop/OSCAAR/readcgns ver5/results/step_1/z.txt
	/home/bm1994/Desktop/OSCAAR/readcgns ver5/results/step_1/coordinate_data/x.txt
	char filename[10][500];
	strcpy(filename[1],"/home/bm1994/Desktop/OSCAAR/readcgns ver5/results/step_1/y.txt");	
	FILE *fp=fopen(
*/

//Assuming I got the data from coordinate files, order 

/*
			float *x,*y,*z;

	    	int s=(int)(irmax-irmin+1);    //s is size of dynamically allocated arrays, here however we need to count it from file in a different way

			x=(float*) malloc(s * sizeof(float));   
			y=(float*) malloc(s* sizeof(float));
			z=(float*) malloc(s* sizeof(float));

*/
/*
step 1- get coordinates into 3 arrays

We have node numbers. each node number represents corresponding coordinate. 
for ex. node number 32 represent the 32nd value in the coordinate file .txt. 
Each node is connected in a particular order with other nodes depending on the element type, represented by the first number. 

Different cell types with their notation and their standard number as per SIDS documentation

notation	name 		cgnsvalue  

TRI_3   	triangle	5 
QUAD_4  	quad		7 
PYRA_5  	pyramid		12 
PENTA_6 	prism		14 
HEXA_8   	hex			17 
MIXED   	mixed		20 
TETRA_4 	tetra		10 


WRITE A FUNCTION TO READ NODES ALONG WITH CELL TYPES OF ZONES WHERE THE NOISE WILL HAVE IT'S SOURCE. ESPESCIALLY WALLS, MOVING WALLS, ETC. 



*/

#include <sys/types.h>  // these 3 are for linux based gcc only. these are used for writing directories
#include <sys/stat.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int coordinatecount;

int filecount(char filename[500]);

//int filecount(*a);

main()
{
		
		char configfilename[500]; 
		strcpy(configfilename,"/home/bm1994/Desktop/OSCAAR/readcgns ver5/configfile.txt");

		char line[1000];
		int i, nline;


		nline=filecount(configfilename);
		FILE *configf=fopen(configfilename, "r");

		int maxlinesize;
		maxlinesize =200;

		char **address;   //pointer to a pointer 
		address = (char**) malloc(nline * sizeof(char*));

		for(i=0;i<nline;i++)
		{
			address[i] = (char*) malloc(maxlinesize *sizeof(char)) ;
		}


		i=0;
		while(fgets(line, sizeof(line), configf))
		{		
			printf(" %s ", line);
			strncpy(address[i], line, strlen(line)-1);     // removes \n from the end of file name stored in line. If \n is not removed it will give segmentation error when trying to open the file sotred in the address. 
			i=i+1;
		}
 
// printf("\n %s", address[0]);


int addresslen = strlen(address[0]);

// printf("\n length of %c is %d ", address[0][61], addresslen); 


/*Initialize dynamic coordinate array

			float *coordinate[3];
			coordinate[i]=(float*) malloc(coordinatecount * sizeof(float));   
			int j;			
				for(j=0;j<coordinatecount;j++)
				{
					fscanf(coordinatefile, "%f", &coordinate[i][j]);
				}

*/

//  TO BE DEBUGGED	



	for(i=0;i<=2;i++)
	{
			char coordinateaddress[500],nccoordinate ;
			FILE *coordinatefile= fopen(address[i], "r");
			int coordinatecountcheck[3];
			coordinatecount=filecount(address[i]);
			printf("\n file opened is %s and number of datapoints is %d", address[i],coordinatecount );
			rewind(coordinatefile);
			coordinatecountcheck[i]=coordinatecount;
			if(coordinatecountcheck[i] == coordinatecount)
			{
			printf("\n coordinate counts are equal \n");
			continue ;
			}
			else
			{
			printf("\n coordinate counts are not equal \n");
			exit(1);
			}

	fclose(coordinatefile);
	fflush(stdout);  
	} 		


			float **coordinate;
			int coordinatetype;    //to initialize the dynamic array. There are 3 coordinate types: x,y,z
			coordinatetype=3;
			coordinate=(float**) malloc(coordinatetype * sizeof(float*));
			for(i=0;i<coordinatetype;i++)
			{
				coordinate[i] = (float*) malloc(coordinatecount * sizeof(float));
			}


			int j;
			float tempcoordinate;


	for(i=0;i<=2;i++)
	{
			FILE *coordinatefiledatastore= fopen(address[i], "r");
			tempcoordinate=0;
			j=0;
			for(j=0;j<coordinatecount;j++)
			{
			fscanf(coordinatefiledatastore, "%f", &tempcoordinate);
			coordinate[i][j]=tempcoordinate;
			}

			rewind(coordinatefiledatastore);
			fflush(stdout);
	fclose(coordinatefiledatastore);
	}

// READING CONNECTIVITY DATA FILES USING CONFIGFILE.TXT


	
		int	connectivitydomainnumber;
		connectivitydomainnumber=7;
		float **connectivitydata;		
		connectivitydata=(float**) malloc (connectivitydomainnumber * sizeof(float*));
		int connectivitydatadomaincount;
		float tempconnectivitydata;

for(i=5;i<12;i++)
	{

		FILE *fconnectivitydata = fopen(address[i], "r");
		
		connectivitydatadomaincount=filecount(address[i]);
		connectivitydata[i-5]= (float*) malloc (connectivitydatadomaincount * sizeof(float));		
		tempconnectivitydata=0;
		j=0;
		for(j=0;j<connectivitydatadomaincount;j++)
		{
		fscanf(fconnectivitydata, "%f", &tempconnectivitydata);
		connectivitydata[i-5][j]=tempconnectivitydata;
		}

			rewind(fconnectivitydata);
			fflush(stdout);
	fclose(fconnectivitydata);


	}		

printf("\n finished loading connectivity data \n");
 
fclose(configf);


for(i=0;i<3;i++)
{			free(coordinate[i]);	}
for(i=0;i<nline;i++)
{			free(address[i]);		}
for(i=0;i<connectivitydomainnumber;i++)
{			free(connectivitydata[i]);	}

}


int filecount(char filename[500])
{

	FILE *filecount = fopen(filename, "r");
	int filecountnumber;
	filecountnumber=0;
	char ncline;
	ncline = getc(filecount);   //ncline = number count line
		
		while(ncline != EOF)
		{
			//count where new line symbol '\n' is encountered 

			if(ncline == '\n')
			{
				filecountnumber=filecountnumber +1;
			}
			ncline = getc(filecount);
		// here the pointer gets incremented automatically -- source Aakash Bangalore. 
		// use rewind for future use. 			
		}
//		printf("\n number of lines in config file is %d \n", nline);

//rewind function
		fclose(filecount);
return filecountnumber;

}
