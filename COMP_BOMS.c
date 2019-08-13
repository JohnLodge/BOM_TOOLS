/*--------------------------------------------------------------------
Deconstruct_bom.c
(c) 2019 Jolo Solutions
Author: John Lodge
Read data from a CSV BOM and output one designator per line

----------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#define MAX_PARTS 5000
#define NEW_LOG_FILE new_log.csv
#define OLD_LOG_FILE old_log.csv


void main(int argc, char *argv[])
{
    int byte_count, Des_count, OLD_Des_count, i, x;
    char PartNum[16];           // Array to hold part number
    char designator[16];         // Array to hold part number
    char c;
    int Part_Count;

    struct Part_Struct
    {
        char number[16];
        char desig[16];
    };

    struct Part_Struct New_BOM_part[MAX_PARTS];
    struct Part_Struct OLD_BOM_Part[MAX_PARTS];
    struct Part_Struct TEMP_Part[MAX_PARTS];

    FILE *wrfp;
    FILE *rdfp;
    FILE *old_rdfp;
    FILE *new_log_wrfp;
    FILE *old_log_wrfp;

    // wrfp = fopen("log.txt", "w+");
    // rdfp = fopen("NHA21121_P01.csv", "r+");
    wrfp = fopen(argv[3], "w+");            // Output Diff File
    rdfp = fopen(argv[1], "r+");            // Input New BOM in CSV comma separated
    old_rdfp = fopen(argv[2], "r+");        // Input Old BOM in CSV comma separated
    new_log_wrfp = fopen("NEW_LOG_FILE", "w+");
    old_log_wrfp = fopen("OLD_LOG_FILE", "w+");

    printf("# of ARGS : %d \n", argc);

    // if (argc < 4)
    //          {
    //          printf("\n **** ERROR incorrect number of arguments **** \n");
    //          printf("USAGE: compbom NewBom.csv OldBom.csv Diff.csv \n");
    //          return 0;
    //      }

    printf("Files : Argv[1] %s, Argv[2] %s , Argv[2] %s\n", argv[1], argv[2], argv[3]);
    fprintf(wrfp, "Designator, NEW Part_Number, OLD Part_Num, Status, \n");


    byte_count = 0;
    Des_count = 0;
    OLD_Des_count = 0;
    Part_Count = 0;

    //Strip the first line
    //printf("\n Searching For End Of Line \n");
//    while((c != '\n') && (c  != EOF))  // Get to end of line
//        c = fgetc(rdfp);

//READ NEW BOM
    do
    {
        //Blank out the Part number
        for(i=0;i<16;i++)
                PartNum[i] = 0x00;

        i = 0;
        while ((c != ','))
        {
            c = fgetc(rdfp);
            PartNum[i] = c;
            i++;
        }
        PartNum[i-1] = 0x00;  // Null out last byte (comma)
        Part_Count++;

        //Read Designator
        do {
            //Blank out the Designator
            for(i=0;i<16;i++)
                designator[i] = 0x00;

            i = 0;
            do
            {
                //c = getchar();
                c = fgetc(rdfp);
                designator[i] = c;
                i++;
            } while((c != ' ') && (c !=','));

            designator[i-1] = 0x00;  // Null out last byte (comma)

        if (i != 1)
            {
                for(i=0;i<16;i++)
                New_BOM_part[Des_count].number[i] = PartNum[i];
                for(i=0;i<16;i++)
                New_BOM_part[Des_count].desig[i] = designator[i];

                fprintf(new_log_wrfp, "%d, %s, %s \n", Des_count, New_BOM_part[Des_count].desig, New_BOM_part[Des_count].number);
             Des_count++;
           }

        } while(c != ',');

        while((c != '\n') && (c  != EOF))  // Get to end of line
            c = fgetc(rdfp);

    }while (c  != EOF);

//READ OLD BOM
    do
    {
        //Blank out the Part number
        for(i=0;i<16;i++)
                PartNum[i] = 0x00;

        i = 0;
        while ((c != ',')) // && (c != '\n'))
        {
            c = fgetc(old_rdfp);
            PartNum[i] = c;
            i++;
        }
        PartNum[i-1] = 0x00;  // Null out last byte (comma)
        Part_Count++;

        //Read Designator
        do {
            //Blank out the Part number
            for(i=0;i<16;i++)
                designator[i] = 0x00;

            i = 0;
            do
            {
                c = fgetc(old_rdfp);
                designator[i] = c;
                i++;
            } while((c != ' ') && (c !=','));

            designator[i-1] = 0x00;  // Null out last byte (comma)

         if (i != 1)
            {
                 for(i=0;i<16;i++)
                    OLD_BOM_Part[OLD_Des_count].number[i] = PartNum[i];
                 for(i=0;i<16;i++)
                    OLD_BOM_Part[OLD_Des_count].desig[i] = designator[i];

                fprintf(old_log_wrfp, "%d, %s, %s \n", OLD_Des_count, OLD_BOM_Part[OLD_Des_count].desig, OLD_BOM_Part[OLD_Des_count].number);
                OLD_Des_count++;
            }
             if (c == ',')
                 break;
        } while(c != ',');
        //printf("\n Searching For End Of Line \n");
        while((c != '\n') && (c  != EOF))  // Get to end of line
            c = fgetc(old_rdfp);

    }while (c  != EOF);


    for(i=0; i<Des_count; i++)
    {
        x = 0;
        while((strcmp(New_BOM_part[i].desig,OLD_BOM_Part[x].desig) != 0) && x <= (OLD_Des_count))
           {
               x++;
           }
       if (x > OLD_Des_count)  // New Part Added
       {
           fprintf(wrfp, "%s, %s, ,ADDED \n", New_BOM_part[i].desig, New_BOM_part[i].number);
       }
       else if ((strcmp(New_BOM_part[i].number,OLD_BOM_Part[x].number) != 0))
       {
           fprintf(wrfp, "%s, %s, %s, CHANGED \n", New_BOM_part[i].desig, New_BOM_part[i].number, OLD_BOM_Part[i].number);
       }
       else if (strcmp(New_BOM_part[i].number,OLD_BOM_Part[x].number)== 0)
       {
           fprintf(wrfp, "%s, %s, %s, SAME \n", New_BOM_part[i].desig, New_BOM_part[i].number, OLD_BOM_Part[i].number);
        }
    }

    for(i=0; i<OLD_Des_count; i++)
    {
        x=0;
        while((strcmp(OLD_BOM_Part[i].desig ,New_BOM_part[x].desig) != 0) && (x <= Des_count))
        {
            x++;
        }
        if (x > Des_count)  //  Part removed
            fprintf(wrfp, "%s, %s, ,REMOVED \n",OLD_BOM_Part[i].desig, OLD_BOM_Part[i].number);
    }

    fclose(rdfp);
    fclose(wrfp);
    fclose(old_rdfp);
    fclose(new_log_wrfp);
    fclose(old_log_wrfp);

    printf("Part Count %d \n", Part_Count);
}
