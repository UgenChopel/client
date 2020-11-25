#include "iec61850_server.h"
#include "hal_thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>


#include "static_model.h"

/* import IEC 61850 device model created from SCL-File */
extern IedModel iedModel;
static int running = 0;

static IedServer myServer = NULL;

void
sigint_handler(int signalId)
{
    running = 0;
}

static CheckHandlerResult
checkHandler(ControlAction action, void* parameter, MmsValue* ctlVal, bool test, bool interlockCheck)
{
    if (ControlAction_isSelect(action))
        printf("check handler called by select command!\n");
    else
        printf("check handler called by operate command!\n");

    if (interlockCheck)
        printf("  with interlock check bit set!\n");

    printf("  ctlNum: %i\n", ControlAction_getCtlNum(action));

    if (parameter == IEDMODEL_Device1_MMXU1_SPCSO1)
        return CONTROL_ACCEPTED;

    return CONTROL_OBJECT_UNDEFINED;
}

static ControlHandlerResult
controlHandlerForBinaryOutput(ControlAction action, void* parameter, MmsValue* value, bool test)
{
    uint64_t timestamp = Hal_getTimeInMs();

    printf("control handler called\n");
    printf("  ctlNum: %i\n", ControlAction_getCtlNum(action));

    if (parameter == IEDMODEL_Device1_MMXU1_SPCSO1) {
        IedServer_updateUTCTimeAttributeValue(myServer, IEDMODEL_Device1_MMXU1_SPCSO1_t, timestamp);
        IedServer_updateAttributeValue(myServer, IEDMODEL_Device1_MMXU1_SPCSO1_stVal, value);
    }
    else
        return CONTROL_RESULT_FAILED;

    return CONTROL_RESULT_OK;
}

static MmsDataAccessError
writeAccessHandler (DataAttribute* dataAttribute, MmsValue* value, ClientConnection connection, void* parameter)
{
    ControlModel ctlModelVal = (ControlModel) MmsValue_toInt32(value);

    /* we only allow status-only and direct-operate */
    if ((ctlModelVal == CONTROL_MODEL_STATUS_ONLY) || (ctlModelVal == CONTROL_MODEL_DIRECT_NORMAL))
    {
        IedServer_updateCtlModel(myServer, IEDMODEL_Device1_MMXU1_SPCSO1, ctlModelVal);

        printf("Changed MMXU1.SPCSI.ctlModel to %i\n", ctlModelVal);

        return DATA_ACCESS_ERROR_SUCCESS;
    }
    else {
        return DATA_ACCESS_ERROR_OBJECT_VALUE_INVALID;
    }
}


static void
connectionHandler (IedServer self, ClientConnection connection, bool connected, void* parameter)
{
    if (connected)
        printf("Connection opened\n");
	// Else print 'connection closed'
	else
        printf("Connection closed\n");
	// END TASK 1.
}



int main(int argc, char** argv) {

    int tcpPort = 102;

    if (argc > 1) {
        tcpPort = atoi(argv[1]);
    }


	IedServer myServer = IedServer_create(&iedModel);


	IedServer_setControlHandler(myServer, IEDMODEL_Device1_MMXU1_SPCSO1,
            (ControlHandler) controlHandlerForBinaryOutput,
            IEDMODEL_Device1_MMXU1_SPCSO1);
	IedServer_handleWriteAccess(myServer, IEDMODEL_Device1_MMXU1_SPCSO1_ctlModel, writeAccessHandler, NULL);


	IedServer_setConnectionIndicationHandler(myServer, (IedConnectionIndicationHandler) connectionHandler, NULL);

	IedServer_start(myServer, tcpPort);
	if (!IedServer_isRunning(myServer)) {
		printf("Starting server failed! Exit.\n");
		IedServer_destroy(myServer);
		exit(-1);
	}

	running = 1;
        while(running){
		IedServer_lockDataModel(myServer);
		IedServer_unlockDataModel(myServer);
        Thread_sleep(2000);  //update time (milliseconds) from the file


        // To read .csv file and display
        int rows, cols;
        float value[1][5];  //values of "rows" and "columns" in .csv file
        FILE * fPtr =fopen("read.csv", "r");

            for(rows=0; rows<1; rows++){
                for(cols=0; cols<5; cols++){
                    fscanf(fPtr, "%f\n", &value[rows][cols]);
                    printf("Value: %.2f \n", value[rows][cols]); //printing the values in the terminal
                }
            printf("\n");
    }
        fclose(fPtr);


        float power1 = value[0][0];
		MmsValue* powerValue1 = MmsValue_newFloat(power1);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_MMXU1_TotW_mag_f,powerValue1);

		float energy1 = value[0][1];
		MmsValue* energyValue1 = MmsValue_newFloat(energy1);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_MMXU1_TotWh_mag_f,energyValue1);

		float current1 = value[0][2];
		MmsValue* currentValue1 = MmsValue_newFloat(current1);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_MMXU1_A_mag_f,currentValue1);

		float voltage2 = value[0][3];
		MmsValue* voltageValue2 = MmsValue_newFloat(voltage2);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_MMXU2_PhV_mag_f,voltageValue2);

		float power2 = value[0][4];
		MmsValue* powerValue2 = MmsValue_newFloat(power2);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_MMXU2_TotW_mag_f,powerValue2);

	}

	IedServer_stop(myServer); //stop MMS server - close TCP server socket and all client sockets
	IedServer_destroy(myServer); // Cleanup - free all resources

}
