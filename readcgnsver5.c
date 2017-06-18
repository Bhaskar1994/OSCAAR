/*   Program readcgns ver5   */
/*
Reads multiple unstructured grid file along with element data and stores connectivity data, grid coordinates and flow solution in .txt files in folders. 
This version has removed all the previous unnecessary comments from the code. 
*/

/*For Reference 
Different cell types with their notation and their standard number as per SIDS documentation

notation	name 		cgnsvalue  

TRI_3   	triangle	5 
QUAD_4  	quad		7 
PYRA_5  	pyramid		12 
PENTA_6 	prism		14 
HEXA_8   	hex			17 
MIXED   	mixed		20 
TETRA_4 	tetra		10 

*/

#include <sys/types.h>  // these 3 are for linux based gcc only. these are used for writing directories
#include <sys/stat.h>
#include <unistd.h>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "cgnslib.h"		 

#if CGNS_VERSION < 3100
# define cgsize_t int
#else
# if CG_BUILD_SCOPE
#  error enumeration scoping needs to be off
# endif
#endif

//int dataarray(int *sample,int size );

/* Global Variables */

	int index_file, totalbase, totalzone, totalflow, nsections, index_sect, inelem, nbndry, iparent_flag, index_flow;
	int index_base, index_zone;	
	char zonename[500], sectionname[500];
    ElementType_t itype, temp;
	cgsize_t isize[3][1], irmin,irmax,istart,iend,iparentdata,nelem, *ielem, start, end;
    char cgnsfilename[500], filename[500];

	cgsize_t n;

	int readfile(char cgnsfilename[500]);
	int numberbase();
	int numberzone();
	int numberflow();
	int writearray(float *a, char filenamewa[500], int arraysize1, int step);
	int writearraypoint(float *b, char filenamewp[500], int arraysize, int step);
	int writeconnectivity(cgsize_t *e, int condatasize, char sname1[500], char stype[500], int step);
	int writearrayflow(float *flowvar, char filenamewf[500], int arraysize2, int step);
	char numbertostring(int number1, char stringname[20]);
	int writeconfigfile(char filenamewc[500]);

main()
{
/* reads  multiple cgns files*/

	int totalstep=100;				
/*totalstep=number of files you want to read. currently the format is file_stepnumber.stepnumber varies from 1 to n. Stepnumber is different from current time. time=stepnumber*time-step*/
	int stepnumber;						//current file number
	char filenumber[20];				// an array which store the current stepnumber in char format
	char cgdirname[100];				//stores the directory name
	char filerootname[100];

	for(stepnumber=1;stepnumber<=totalstep;stepnumber++)
	{
		if(stepnumber<10)
		{
		strcpy(cgdirname,"/home/bm1994/Desktop/OSCAAR/readcgns ver5/output data/");
		strcpy(filerootname,"cylinder-");
		strcat(filerootname,"000");
		numbertostring(stepnumber, filenumber);
		strcat(filerootname,filenumber);
		strcat(filerootname,".cgns");
		strcat(cgdirname,filerootname);
		strcpy(cgnsfilename,cgdirname);
		}
		else if(stepnumber>9 && stepnumber<100)
		{
		strcpy(cgdirname,"/home/bm1994/Desktop/OSCAAR/readcgns ver5/output data/");
		strcpy(filerootname,"cylinder-");
		strcat(filerootname,"00");
		numbertostring(stepnumber, filenumber);
		strcat(filerootname,filenumber);
		strcat(filerootname,".cgns");
		strcat(cgdirname,filerootname);
		strcpy(cgnsfilename,cgdirname);
		}
		else if(stepnumber==100 && stepnumber>100 && stepnumber<1000)	
		{
		strcpy(cgdirname,"/home/bm1994/Desktop/OSCAAR/readcgns ver5/output data/");
		strcpy(filerootname,"cylinder-");
		strcat(filerootname,"0");
		numbertostring(stepnumber, filenumber);
		strcat(filerootname,filenumber);
		strcat(filerootname,".cgns");
		strcat(cgdirname,filerootname);
		strcpy(cgnsfilename,cgdirname);
		}	
		readfile(cgnsfilename);
		printf("Reading file %d \n", stepnumber);
		/* to get number of cgns base nodes in file  */
		index_base=numberbase();
		printf("Number of base in step %d is %d \n", stepnumber, index_base);
		/* to get number of zones in each base node */
		index_zone=numberzone();
		printf("Number of zones in step %d is %d \n", stepnumber, index_zone);
		cg_zone_read(index_file,index_base,index_zone,zonename,isize[0]);

		printf("Reading zone %d \n", index_zone);

		/* lower range index, for unstructured it will be always 1. 
		see index_dim in https://cgns.github.io/CGNS_docs_current/midlevel/structural.html
		*/
	    irmin=1;

		/* upper range index of vertices */
	    irmax=isize[0][0];
    	int s=(int)(irmax-irmin+1);    //s is size of dynamically allocated arrays
		//x,y,z coordinate arrays

			float *x,*y,*z;
			x=(float*) malloc(s * sizeof(float));   
			y=(float*) malloc(s* sizeof(float));
			z=(float*) malloc(s* sizeof(float));
		
	
//		if(stepnumber==1)
	//	{		

			/* read grid coordinates */
		    cg_coord_read(index_file,index_base,index_zone,"CoordinateX", \
	                  RealSingle,&irmin,&irmax,x);

    		cg_coord_read(index_file,index_base,index_zone,"CoordinateY", \
    	              RealSingle,&irmin,&irmax,y);
	
    		cg_coord_read(index_file,index_base,index_zone,"CoordinateZ", \
    	              RealSingle,&irmin,&irmax,z);

			char xname[100],yname[100],zname[100];

			strcpy(xname,"x.txt\0");
			strcpy(yname,"y.txt\0");
			strcpy(zname,"z.txt\0");

			writearray( x, xname, s, stepnumber);
			writearray( y, yname, s, stepnumber);
			writearray( z, zname, s, stepnumber);
			printf("Finished reading and writing files with Grid-coordinates \n");
/*		}
		else
		{
			printf("Read coordinates in step 1");
		}		
*/



//									FLOW SOLUTION WAS HERE


		/*read section data */
	    cg_nsections(index_file,index_base,index_zone,&nsections);
		printf("Reading Sections... \n number of sections = %i \n", nsections);
		
		/* read element connectivity */
		for (index_sect=1; index_sect <= nsections; index_sect++)
		{
			cg_section_read(index_file,index_base,index_zone,index_sect,sectionname, \
			&itype,&istart,&iend,&nbndry,&iparent_flag);

			/* reading array size in each element */ 
			cg_ElementDataSize(index_file, index_base, index_zone, index_sect, &nelem);
//			printf("\n number of elements in element %i %i", index_sect, (int)nelem);    
			int inelem=(int)nelem; 

			ielem=(cgsize_t*) malloc(inelem * sizeof(cgsize_t));
/*
			printf("\nReading section data...\n");
			printf("   section name=%s\n",sectionname);
			printf("   section type=%s\n",ElementTypeName[itype]);
			printf("   istart,iend=%i, %i\n",(int)istart,(int)iend);
*/
			char sectname[500];
			strcpy(sectname, ElementTypeName[itype]);
			if (itype == MIXED)
			{
//				printf("reading element data for this element\n");
				cg_elements_read(index_file,index_base,index_zone,index_sect,ielem, \
				&iparentdata);
				//keep this as backup in case to debug future segmentation errors: fflush(stdout);
				writeconnectivity(ielem, inelem, sectionname, sectname, stepnumber);
				printf("\t\t wrote connectivity data for section py %d %s %s \n", index_sect, sectname, sectionname);
				temp = (ElementType_t) ielem[0];
/*				printf(" section %s has first element as %s \n", sectionname, cg_ElementTypeName(temp));
				if( temp==QUAD_4)
				{
		         	printf("This element is made of following nodes: %i %i %i %i ", (int)ielem[(1*4)+2], (int)ielem[(1*4)+3],(int)ielem[(1*4)+4],    						(int)ielem[(1*4)+5]);
				} */
			}
			else if (itype == QUAD_4) 
			{
				cg_elements_read(index_file,index_base,index_zone,index_sect,ielem, \
				&iparentdata);
				//keep this as backup in case to debug future segmentation errors: fflush(stdout);
				writeconnectivity(ielem, inelem, sectionname, sectname, stepnumber);
				printf("\t\t wrote connectivity data for section %d %s %s \n", index_sect, sectname, sectionname);
				temp = (ElementType_t) ielem[0];
			}
			else if (itype == TRI_3) 
			{
				cg_elements_read(index_file,index_base,index_zone,index_sect,ielem, \
				&iparentdata);
				//keep this as backup in case to debug future segmentation errors: fflush(stdout);
				writeconnectivity(ielem, inelem, sectionname, sectname, stepnumber);
				printf("\t\t wrote connectivity data for section %d %s %s \n", index_sect, sectname, sectionname);
				temp = (ElementType_t) ielem[0];
			}
					else if (itype == TETRA_4) 
			{
				cg_elements_read(index_file,index_base,index_zone,index_sect,ielem, \
				&iparentdata);
				//keep this as backup in case to debug future segmentation errors: fflush(stdout);
				writeconnectivity(ielem, inelem, sectionname, sectname, stepnumber);
				printf("\t\t wrote connectivity data for section %d %s %s \n", index_sect, sectname, sectionname);
				temp = (ElementType_t) ielem[0];
			}
	
			else 
			{
			printf("   not reading element data for this element %s \n", sectname);
			}
		}


		/* read flow solution */

		index_flow=numberflow();
	
		float *vx, *vy, *vz, *sp, *dp, *rp, *sfx, *sfy, *sfz;
		vx=(float*) malloc(s * sizeof(float));
		vy=(float*) malloc(s* sizeof(float));
		vz=(float*) malloc(s* sizeof(float));
		sp=(float*) malloc(s* sizeof(float));
		dp=(float*) malloc(s* sizeof(float));
		rp=(float*) malloc(s* sizeof(float));
		sfx=(float*) malloc(s* sizeof(float));
		sfy=(float*) malloc(s* sizeof(float));
		sfz=(float*) malloc(s* sizeof(float));


	    cg_field_read(index_file,index_base,index_zone,index_flow,"VelocityX", \
                  RealSingle,&irmin,&irmax,vx);
		cg_field_read(index_file,index_base,index_zone,index_flow,"VelocityY", \
                  RealSingle,&irmin,&irmax,vy);
		cg_field_read(index_file,index_base,index_zone,index_flow,"VelocityZ", \
                  RealSingle,&irmin,&irmax,vz);
	    cg_field_read(index_file,index_base,index_zone,index_flow,"Pressure", \
                  RealSingle,&irmin,&irmax,sp);
	    cg_field_read(index_file,index_base,index_zone,index_flow,"PressureDynamic", \
                  RealSingle,&irmin,&irmax,dp);
		cg_field_read(index_file,index_base,index_zone,index_flow,"Relative_Total_Pressure", \
                  RealSingle,&irmin,&irmax,rp);
		cg_field_read(index_file,index_base,index_zone,index_flow,"SkinFrictionX", \
                  RealSingle,&irmin,&irmax,sfx);
		cg_field_read(index_file,index_base,index_zone,index_flow,"SkinFrictionY", \
                  RealSingle,&irmin,&irmax,sfy);
		cg_field_read(index_file,index_base,index_zone,index_flow,"SkinFrictionZ", \
                  RealSingle,&irmin,&irmax,sfz);

 
		char vxname[300], vyname[300], vzname[300],spname[300], dpname[300], rpname[300], sfxname[300], sfyname[300], sfzname[300];
		strcpy(vxname,"velocity_X.txt\0");
		strcpy(vyname,"velocity_y.txt\0");
		strcpy(vzname,"velocity_z.txt\0");

		strcpy(spname,"Static_Pressure.txt\0");
		strcpy(dpname, "Dynamic_Pressure.txt\0");
		strcpy(rpname,"RelativeTotal_Pressure.txt\0");

		strcpy(sfxname,"WallShear_X.txt\0");
		strcpy(sfyname,"WallShear_Y.txt\0");
		strcpy(sfzname,"WallShear_Z.txt\0");

		writearrayflow( vx, vxname, s, stepnumber);
		writearrayflow( vy, vyname, s, stepnumber);
		writearrayflow( vz, vzname, s, stepnumber);
		writearrayflow( sp, spname, s, stepnumber);
		writearrayflow( dp, dpname, s, stepnumber);
		writearrayflow( rp, rpname, s, stepnumber);
		writearrayflow( sfx, sfxname, s, stepnumber);
		writearrayflow( sfy, sfyname, s, stepnumber);
		writearrayflow( sfz, sfzname, s, stepnumber);

		printf("Finished reading and writing files for flow-solution parameters \n");








	/* close CGNS file */
	cg_close(index_file);
	printf("closing cgns file for step %d \n", stepnumber);
/*	printf("\nSuccessfully read unstructured grid from file grid_c.cgns");
	printf("\n x[20]=%f y[5]=%f z[10]=%f \n ", x[20], y[5], z[10]);
*/
	free(ielem);
	free(x);
	free(y);
	free(z);
	free(vx);
	free(vy);
	free(vz);
	free(sp);
	free(dp);
	free(rp);
	free(sfx);
	free(sfy);
	free(sfz);



	printf("freed allocated heap memory \n");
	}
}

/* Functions defined */

int readfile(char cgnsfilename[500])
{
	/* function to read a file */
	if(cg_open(cgnsfilename,CG_MODE_READ,&index_file)) cg_error_exit() ;
	return 0;
}

int numberbase()
{
	/*function to read number of base nodes in cgns file */
	cg_nbases(index_file,&totalbase);
	if(totalbase==1)
	{
	index_base=1;
	}
	else
	{
	printf("more than 1 base");
	}
	return index_base;
}

int numberzone()
{
	/*function to read number of zones in each base */
	cg_nzones(index_file,index_base,&totalzone);
	if(totalzone==1)
	{
	index_zone=1;
	}
	else
	{
	printf("more than 1 zone");
	}
	return index_zone;
}

int numberflow()
{
	/*function to read number of flow solution zones */
	cg_nsols(index_file,index_base,index_zone,&totalflow);
	if(totalflow==1)
	{
	index_flow=1;
	}
	else
	{
	printf("more than 1 flow solution");
	}
	return index_flow;
}

int writearray(float *a, char filenamewa[500], int arraysize1, int step)
{
	/*function to read the contents of an array and store it to a folder */
		
		struct stat st1 = {0};
		struct stat st2 = {0};
		char tempdirectoryname[500];
		char directoryappend[100];
		char *filepoint;
		strcpy(tempdirectoryname,"/home/bm1994/Desktop/OSCAAR/readcgns ver5/results/");
		if (stat("/home/bm1994/Desktop/OSCAAR/readcgns ver5/results/", &st1) < 0)
		{
			mkdir("/home/bm1994/Desktop/OSCAAR/readcgns ver5/results", 0777);
		}
		strcat(tempdirectoryname,"step_");
		numbertostring(step, directoryappend);	
		strcat(tempdirectoryname,directoryappend);
		strcat(tempdirectoryname,"/");
		
		if (stat(tempdirectoryname, &st2) < 0)
		{
			mkdir(tempdirectoryname, 0777);   //folder "results/step_1" has been created
			if(step==1)
			{
			strcat(tempdirectoryname,"coordinate_data/");
			mkdir(tempdirectoryname, 0777);    //folder "step_1/coordinate_data/" has been created
			}
		}
		if(step==1)
		{
		    char dirname1[500];	
			strcpy(dirname1, tempdirectoryname); //results/coordinate_data
		    strcat(dirname1,filenamewa);
			strcpy(filenamewa,dirname1);
			printf("\n before writeconfig in writearray %s ",filenamewa);
			writeconfigfile(filenamewa);
			printf("\nAfter writeconfig in writearray ");

			FILE *fp = fopen(filenamewa, "w");
			int i;
			if(!fp)
			{
				return 1;
			}
			else
			{	
				for(i=0;i<arraysize1;i++)
				{
				fprintf(fp,"%f\n",a[i]);
				}
				return 0;
			}
//			strcat(filenamewa,"\0");
//			writeconfigfile(filenamewa);

			fclose(fp);
		}
}
int writearrayflow(float *flowvar, char filenamewf[500], int arraysize2, int step)
{
	struct stat st2 = {0};
	char tempdirectoryname[500];
	char directoryappend[100];
	strcpy(tempdirectoryname,"/home/bm1994/Desktop/OSCAAR/readcgns ver5/results/");
	strcat(tempdirectoryname,"step_");
	numbertostring(step, directoryappend);
	strcat(tempdirectoryname,directoryappend);
	strcat(tempdirectoryname,"/flow_data/");
	mkdir(tempdirectoryname, 0777);
    char dirname2[500];	
	strcpy(dirname2, tempdirectoryname); //results/coordinate_data
    strcat(dirname2,filenamewf);
	strcpy(filenamewf,dirname2);
	FILE *fp = fopen(filenamewf, "w");
	writeconfigfile(filenamewf);
	int i;
	if(!fp)
	{
		return 1;
	}
	else
	{	
		for(i=0;i<arraysize2;i++)
		{
		fprintf(fp,"%f\n",flowvar[i]);
		}
		return 0;
	}
	fclose(fp);
}
int writeconnectivity(cgsize_t *e, int condatasize, char sname1[500], char stype[500], int step)
{
	if(step==1)
	{	
	char sname[500];
	strcpy(sname,sname1);
	ElementType_t ftemp = (ElementType_t) ielem[0];
	strcat(sname,"_");
	char elname[500];
//    strcpy(elname,cg_ElementTypeName(ftemp));
	strcpy(elname, stype);
	strcat(sname,elname);
	strcat(sname,".txt");
	float *conarray;
	conarray=(float*) malloc(condatasize * sizeof(float));
	int i;
	for(i=0;i<condatasize;i++)   
	{
		conarray[i]=(float)e[i];   

	}
	writearraypoint(conarray, sname, condatasize, step);
	return 0;
	free(conarray);
	}
}


int writearraypoint(float *b, char filenamewp[500], int arraysize, int step)
{
//written for arrays printed dynamically also creates a folder to write the files

	struct stat st = {0};

/*
	if (stat("/home/bm1994/Desktop/OSCAAR/readcgns ver5/results/", &st) < 0)
	{
	printf("\n whyhy \n");	
	printf("\nfailure to cerate folder\n");

		mkdir("/home/bm1994/Desktop/OSCAAR/readcgns ver5/results", 0777);
*/

	char tempdirectoryname[500];
	char directoryappend[100];
	strcpy(tempdirectoryname,"/home/bm1994/Desktop/OSCAAR/readcgns ver5/results/");

	strcat(tempdirectoryname,"step_");
	
	numbertostring(step, directoryappend);
//	directoryappend=step + '0';
	strcat(tempdirectoryname,directoryappend);
	strcat(tempdirectoryname,"/connectivity_data/");
	
	mkdir(tempdirectoryname, 0777);



//		mkdir("/home/bm1994/Desktop/OSCAAR/readcgns ver5/results/connectivity_data", 0777);

	
	

    char dirname[100];	
	strcpy(dirname, tempdirectoryname);
	strcat(dirname,filenamewp);
	strcpy(filenamewp,dirname);



	FILE *fp = fopen(filenamewp, "w");
	writeconfigfile(filenamewp);
	int i;
	if(!fp)
	{
		return 1;
	}
	else
	{	
		for(i=0;i<arraysize;i++)
		{
		fprintf(fp,"%f\n",b[i]);
		}
		return 0;
	}
	fclose(fp);
}

char numbertostring(int number1, char stringname[20])
{
	snprintf(stringname, 20, "%d", number1);
	return *stringname;
}

int writeconfigfile(char filenamewc[500])
{
	printf("\nbeggining of writeconfig");
	char filenameconfig[500];
	int i;
	i=100;			// program reached here
	strcpy(filenameconfig,"/home/bm1994/Desktop/OSCAAR/readcgns ver5/configfile.txt");
/*	FILE *fp;
	if( access( filenameconfig, F_OK ) != -1)
	{
		int temp1;
		temp1=5;
		FILE *fp = fopen(filenameconfig, "w");
//		for(i=0;i<500;i++)
//		{
		fprintf(fp,"%s\n",filenamewc);
//		}
		
	}
	else
	{
		int temp2;
		temp2=10;
*/
		FILE *fp = fopen(filenameconfig, "a+");	
		fprintf(fp,"%s\n",filenamewc);
	
	fclose(fp);
//	return 0;
}
/*int dataarray(int *sample, int size )
{

	int lo;
		for(lo=0;lo<size;lo++)
		{
		sample[lo]=lo+1;
		}
	return 0;
}
*/
