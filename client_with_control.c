#include "iec61850_client.h"

#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <unistd.h>

static void commandTerminationHandler(void *parameter, ControlObjectClient connection)
{
    LastApplError lastApplError = ControlObjectClient_getLastApplError(connection);

    /* if lastApplError.error != 0 this indicates a CommandTermination- */
    if (lastApplError.error != 0) {
        printf("Received CommandTermination-.\n");
        printf(" LastApplError: %i\n", lastApplError.error);
        printf("      addCause: %i\n", lastApplError.addCause);
    }
    else
        printf("Received CommandTermination+.\n");
}




int main(int argc, char** argv)
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
    printf("\n%s", ctime(&start));
    int time_interval = (5); //Run after every 5 seconds

    //


    IedClientError error;
    MmsError mss_error;
	IedConnection myClient = IedConnection_create();

    IedConnection_connect(myClient, &error, hostname, tcpPort);
	//END TASK 1.6


	//START TASK 2.3
	// Obtain the tree shown in Figure 7
	// Hint use LinkedList structure to loop through LD, LN, DO
    if (error == IED_ERROR_OK) {


        //Declare a LinkedList called dataSetItems
        LinkedList dataSetItems1 = LinkedList_create();
        LinkedList dataSetItems2 = LinkedList_create();
        LinkedList dataSetItems3 = LinkedList_create();

        LinkedList_add(dataSetItems1, "SampleIEDDevice1/MMXU1.TotW.mag.f[MX]");
        LinkedList_add(dataSetItems1, "SampleIEDDevice1/MMXU1.TotWh.mag.f[MX]");
        LinkedList_add(dataSetItems1, "SampleIEDDevice1/MMXU1.A.mag.f[MX]");
        LinkedList_add(dataSetItems1, "SampleIEDDevice1/MMXU2.PhV.mag.f[MX]"); //DataSet members can be from diff. LN as well
        LinkedList_add(dataSetItems1, "SampleIEDDevice1/MMXU2.TotW.mag.f[MX]");
     //   LinkedList_add(dataSetItems2, "SampleIEDDevice1/MMXU2.PhV.mag.f[MX]");

        LinkedList_add(dataSetItems2, "SampleIEDDevice1/DGEN1.TotWh.mag.f[MX]");

        //
        //Create a dataset called 'newDataset'
        //Hint: IedConnection_createDataSet
        IedConnection_createDataSet(myClient, &error, "SampleIEDDevice1/MMXU1.newDataSet", dataSetItems1);

        IedConnection_createDataSet(myClient, &error, "SampleIEDDevice1/MMXU2.newDataSet", dataSetItems2);
        char* dataSet_reference1="SampleIEDDevice1/MMXU1.newDataSet";

        char* dataSet_reference2="SampleIEDDevice1/MMXU2.newDataSet";

 //       char* dataSet_reference3="SampleIEDDevice1/DGEN1.newDataSet";

        //
        //Declare a ClientDataSet 'dataSetToRead' and read the dataset values from the newDataSet
        //Hint: Use 'IedConnection_readDataSetValues' function
        ClientDataSet dataSetToRead1=IedConnection_readDataSetValues(myClient, &error,dataSet_reference1, NULL);

        ClientDataSet dataSetToRead2=IedConnection_readDataSetValues(myClient, &error,dataSet_reference2, NULL);

//        ClientDataSet dataSetToRead3=IedConnection_readDataSetValues(myClient, &error,dataSet_reference3, NULL);

        //
        //Declare an MmsValue 'dataSet_mmsValue'
        //Hint: Use 'ClientDataSet_getValues' function
        MmsValue* dataSet_mmsvalue1=ClientDataSet_getValues(dataSetToRead1);

        MmsValue* dataSet_mmsvalue2=ClientDataSet_getValues(dataSetToRead2);


        int index_number;
 //       printf("MMXU1 Values [Power, Energy, Current and Voltage]: %s", ctime(&start));

        for(index_number=0; index_number<5; index_number++){

            printf("%.2f\n", MmsValue_toFloat(MmsValue_getElement(dataSet_mmsvalue1,index_number)));

        }


        //To write the file

        FILE * finout1;
        finout1=fopen("value.xls", "a");
      //  fprintf(finout1, "Power Energy  Current Voltage\n");
        fprintf(finout1, "\n%s  ", ctime(&start));
        fprintf(finout1, "\n");
        for(index_number=0; index_number<5; index_number++){
            fprintf(finout1, "  %.2f  ", MmsValue_toFloat(MmsValue_getElement(dataSet_mmsvalue1,index_number)));

            }
  //      fprintf(finout1, "\n");
        fclose(finout1);



        //Destroy logical devices
//        LinkedList_destroy(deviceList);
        //
        ////*/

        IedConnection_close(myClient);


        //for control

        /************************
         * Direct control
         ***********************/

        ControlObjectClient control
            = ControlObjectClient_create("SampleIEDDevice1/MMXU1.SPCSO1", myClient);

        MmsValue* ctlVal = MmsValue_newBoolean(true);

        ControlObjectClient_setOrigin(control, NULL, 3);

        if (ControlObjectClient_operate(control, ctlVal, 0 /* operate now */)) {
            printf("SampleIEDDevice1/MMXU1.SPCSO1 operated successfully\n");
        }
        else {
            printf("failed to operate SampleIEDDevice1/MMXU1.SPCSO1\n");
        }

        MmsValue_delete(ctlVal);

        ControlObjectClient_destroy(control);

        /* Check if status value has changed */

        MmsValue* stVal = IedConnection_readObject(myClient, &error, "SampleIEDDevice1/MMXU1.SPCSO1.stVal", IEC61850_FC_ST);

        if (error == IED_ERROR_OK) {
            bool state = MmsValue_getBoolean(stVal);
            MmsValue_delete(stVal);

            printf("New status of SampleIEDDevice1/MMXU1.SPCSO1.stVal: %i\n", state);
        }
        else {
            printf("Reading status for SampleIEDDevice1/MMXU1.SPCSO1 failed!\n");
        }


    ///

    }
    else {
        printf("Connection failed!\n");
    }

    cleanup_and_exit:
    IedConnection_destroy(myClient);

     sleep(time_interval);
    }

}
