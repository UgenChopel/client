#include "iec61850_client.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

int
main(int argc, char** argv)
{

    char* hostname;
    int tcpPort = 102; /*Server adress*/

    if (argc > 1)
        hostname = argv[1];
    else
        hostname = "localhost";

    if (argc > 2)
        tcpPort = atoi(argv[2]);


  //To initiate the server request after regular time interval
    srand(1);
    for (;;) {
    time_t start = time(NULL);  //start time (present time)
    int time_interval = (.2); //Run after every 5 seconds
    printf("\n%s", ctime(&start));
    //


    IedClientError error;
    MmsError mss_error;
	IedConnection myClient = IedConnection_create();

    IedConnection_connect(myClient, &error, hostname, tcpPort);

    if (error == IED_ERROR_OK) {

        //Declare a LinkedList called dataSetItems
        LinkedList dataSetItems1 = LinkedList_create();
        LinkedList dataSetItems2 = LinkedList_create();
        LinkedList dataSetItems3 = LinkedList_create();

        //
        //Add elements maniupulated in server script in the dataSetItems
        //Hint: LinkedList_add(dataSetItems,'itemreference')
        LinkedList_add(dataSetItems1, "SampleIEDDevice1/MMXU1.TotW.mag.f[MX]");
        LinkedList_add(dataSetItems1, "SampleIEDDevice1/MMXU1.TotWh.mag.f[MX]");
        LinkedList_add(dataSetItems1, "SampleIEDDevice1/MMXU1.A.mag.f[MX]");
        LinkedList_add(dataSetItems1, "SampleIEDDevice1/MMXU2.PhV.mag.f[MX]"); //DataSet members can be from diff. LN as well

        LinkedList_add(dataSetItems2, "SampleIEDDevice1/MMXU2.TotW.mag.f[MX]");
     //   LinkedList_add(dataSetItems2, "SampleIEDDevice1/MMXU2.PhV.mag.f[MX]");

        LinkedList_add(dataSetItems3, "SampleIEDDevice1/DGEN1.TotWh.mag.f[MX]");

        //
        //Create a dataset called 'newDataset'
        //Hint: IedConnection_createDataSet
        IedConnection_createDataSet(myClient, &error, "SampleIEDDevice1/MMXU1.newDataSet", dataSetItems1);

        IedConnection_createDataSet(myClient, &error, "SampleIEDDevice1/MMXU2.newDataSet", dataSetItems2);

        IedConnection_createDataSet(myClient, &error, "SampleIEDDevice1/DGEN1.newDataSet", dataSetItems3);
        //END TASK 5.3

        //START TASK 5.4
        //Declare a string 'dataSet_reference' and assign the reference of newDataSet
        char* dataSet_reference1="SampleIEDDevice1/MMXU1.newDataSet";

        char* dataSet_reference2="SampleIEDDevice1/MMXU2.newDataSet";

        char* dataSet_reference3="SampleIEDDevice1/DGEN1.newDataSet";

        //
        //Declare a ClientDataSet 'dataSetToRead' and read the dataset values from the newDataSet
        //Hint: Use 'IedConnection_readDataSetValues' function
        ClientDataSet dataSetToRead1=IedConnection_readDataSetValues(myClient, &error,dataSet_reference1, NULL);

        ClientDataSet dataSetToRead2=IedConnection_readDataSetValues(myClient, &error,dataSet_reference2, NULL);

        ClientDataSet dataSetToRead3=IedConnection_readDataSetValues(myClient, &error,dataSet_reference3, NULL);

        //
        //Declare an MmsValue 'dataSet_mmsValue'
        //Hint: Use 'ClientDataSet_getValues' function
        MmsValue* dataSet_mmsvalue1=ClientDataSet_getValues(dataSetToRead1);

        MmsValue* dataSet_mmsvalue2=ClientDataSet_getValues(dataSetToRead2);

        MmsValue* dataSet_mmsvalue3=ClientDataSet_getValues(dataSetToRead3);


        printf("MMXU1 Values [Power, Energy, Current and Voltage] are:\n %s %.2f   %.2f  %.2f  %.2f\n",
                                    ctime(&start),
                                    MmsValue_toFloat(MmsValue_getElement(dataSet_mmsvalue1,0)),
                                    MmsValue_toFloat(MmsValue_getElement(dataSet_mmsvalue1,1)),
                                    MmsValue_toFloat(MmsValue_getElement(dataSet_mmsvalue1,2)),
                                    MmsValue_toFloat(MmsValue_getElement(dataSet_mmsvalue1,3)));

        //To write the file

        FILE * finout1;
        finout1=fopen("value.csv", "a");
      //  fprintf(finout1, "Power Energy  Current Voltage\n");

        fprintf(finout1, "%s %.2f %.2f %.2f %.2f\n",
                                    ctime(&start),
                                    MmsValue_toFloat(MmsValue_getElement(dataSet_mmsvalue1,0)),
                                    MmsValue_toFloat(MmsValue_getElement(dataSet_mmsvalue1,1)),
                                    MmsValue_toFloat(MmsValue_getElement(dataSet_mmsvalue1,2)),
                                    MmsValue_toFloat(MmsValue_getElement(dataSet_mmsvalue1,3)));
        fclose(finout1);



        IedConnection_close(myClient);



    }
    else {
        printf("Connection failed!\n");
    }

    cleanup_and_exit:
    IedConnection_destroy(myClient);

     sleep(time_interval);
    }

}
