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

static void
connectionHandler (IedServer self, ClientConnection connection, bool connected, void* parameter)
{
    if (connected)
        printf("Connection opened\n");
	// Else print 'connection closed'
	else
        printf("Connection closed\n");
	// END TASK 1.1
}

int main(int argc, char** argv) {

    int tcpPort = 102;

    if (argc > 1) {
        tcpPort = atoi(argv[1]);
    }

	IedServer myServer = IedServer_create(&iedModel);

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

        int i, j;
        float value[1][4];
        FILE * fPtr =fopen("read.csv", "r");

            for(i=0; i<1; i++){
                for(j=0; j<4; j++){
                    fscanf(fPtr, "%f\n", &value[i][j]);
                    printf("Value: %.2f \n", value[i][j]); //printing the values in the terminal
                }
    	}
        fclose(fPtr);

		float energy= 205.f;
        	MmsValue* energyValue = MmsValue_newFloat(energy);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_DGEN1_TotWh_mag_f,energyValue);

		float power1 = value[0][0];
		MmsValue* powerValue1 = MmsValue_newFloat(power1);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_MMXU1_TotW_mag_f,powerValue1);

		float energy1 = value[0][1];
		MmsValue* energyValue1 = MmsValue_newFloat(energy1);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_MMXU1_TotWh_mag_f,energyValue1);

        	float current1 = value[0][2];
		MmsValue* currentValue1 = MmsValue_newFloat(current1);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_MMXU1_A_mag_f,currentValue1);

		float power2 = 230.f;
		MmsValue* powerValue2 = MmsValue_newFloat(power2);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_MMXU2_TotW_mag_f,powerValue2);

		float voltage2 = value[0][3];
		MmsValue* voltageValue2 = MmsValue_newFloat(voltage2);
		IedServer_updateAttributeValue(myServer,IEDMODEL_Device1_MMXU2_PhV_mag_f,voltageValue2);

	}

	IedServer_stop(myServer); //stop MMS server - close TCP server socket and all client sockets
	IedServer_destroy(myServer); // Cleanup - free all resources


}
