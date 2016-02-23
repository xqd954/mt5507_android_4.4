
/*******************************************************************************
*
*   BTSimulatorSender used to send data to BTSimulatorReceiver
*   All data transaction through transact method.
*   BTSimulatorReceiver use onTranact to receive data
*
*******************************************************************************/

#include "BTSimulatorSender.h"
#include <binder/IPCThreadState.h>


namespace android {

void BTSimulatorSender::getReceiver(const char * serviceName)
{
	if (binder.get() != 0)
		return ;

	sp<IServiceManager> sm = defaultServiceManager();
	binder = sm->getService(String16(serviceName));
	ALOGD("[BTSIM][BTSimulatorSender] : getReceiver %p/n",sm.get());

	if (binder.get() == 0)
	{
		ALOGE("[BTSIM][BTSimulatorSender] : getService  NULL");		
	}
}

/*
* transact method used to transact data and receive data back from receiver onTransact
*/
void BTSimulatorSender::transact(int transID, Parcel &data, const char * serviceName)
{
	getReceiver(serviceName);

	Parcel reply;

	ALOGD("[BTSIM][transcat] : transID %d /n",transID);

	if (binder.get() != 0)
	{
		binder->transact(transID,data,&reply);
		int i = reply.readInt32();
		ALOGD("[BTSIM][transcat] : transcat return val %d /n",i);
	}
}

/*
* used to transact order to dump report
*/
void BTSimulatorSender::dumpEmmaReport(const char * serviceName)
{
	Parcel data;
	transact(1,data,serviceName);
}

/*
* used to transact to send logs
*/
void BTSimulatorSender::sendLogs(Parcel &data, const char * serviceName)
{
	transact(0,data,serviceName);
}

/*
* used to transact to call private method
* write int data to to Parcel.
*/
void BTSimulatorSender::callPrivateMethod(int classId, int id, const char* serviceName)
{
	Parcel data;
	if (serviceName == NULL)
	{
		ALOGD("[BTSIM][BTSimulatorSender] onSenderCallPrivate : service name is null");
		return;
	}
	data.writeInt32(classId);
	data.writeInt32(id);
	transact(2,data,serviceName);
}

};

