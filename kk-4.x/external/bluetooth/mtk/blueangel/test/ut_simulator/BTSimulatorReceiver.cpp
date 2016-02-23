
/*******************************************************************************
*
*   BTSimulatorReceiver used to deal with BTSimulatorSender transact method
*   Transfer data to different method
*
*******************************************************************************/

#include "BTSimulatorReceiver.h"
#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include <binder/ProcessState.h>

//#define LOG_TAG "Bluetooth.Simulator.Receiver"

/*
* transfer data to native method
*/
void onDataArrival(const android::Parcel & data)
{
	char sep = (char)data.readInt32();
	int  numOfLogs = data.readInt32();

	ALOGI("[BTSIM][BTSimulatorReceiver] : onDataArrival sep %c numOfLogs %d",sep,numOfLogs);

	for (int i = 0; i< numOfLogs ;i++)
	{
		ALOGI("[BTSIM][BTSimulatorReceiver] : onDataArrival logs %s ",data.readCString());
	}
}

namespace android {

/*
* load receiver, call BTSimulatorReceiver constructor, set call method which will be called in onTransact
* join thread pool
*/
void  BTSimulatorReceiver::launchReceiver(char *name, BTSimulatorOnFuncs *onhander, BOOL isBlock) 
{	
	sp<IServiceManager> sm = defaultServiceManager();
	ALOGI("[BTSIM][BTSimulatorReceiver] : testMain %p",sm.get());

    //call BTSimulatorReceiver constructor, set method
	int r = defaultServiceManager()->addService(String16(name), new BTSimulatorReceiver(onhander));

	if (isBlock == TRUE)
	{
		//need to run in the main thread , otherwise,  NE will happen.
		sp<ProcessState> proc(ProcessState::self());		
		ProcessState::self()->startThreadPool();
		IPCThreadState::self()->joinThreadPool();
	}
}

void testBTUTStart()
{
	//BTSimulatorReceiver::launchReceiver("Bluetooth.BTSimulatorReceiver.A2DP", ::onDataArrival,FALSE);
}

/*
* set method to public field
*/
BTSimulatorReceiver::BTSimulatorReceiver(BTSimulatorOnFuncs *onhander) 	
{
	mOnHander.dataArrival = onhander->dataArrival;
	mOnHander.dump = onhander->dump;
	mOnHander.cbPrivate = onhander->cbPrivate;
}

/*
* de-constructor
*/
BTSimulatorReceiver::~BTSimulatorReceiver()
{
	
}

/*
* according to code to switch different case.
* the param data is to store the param which transact from BTSimulatorSender(transcat)
*/
status_t BTSimulatorReceiver::onTransact(uint32_t code, const Parcel & data, Parcel * reply, uint32_t flags)
{
	ALOGD("[BTSIM][BTSimulatorReceiver] : onTransact code %d ",code);
	switch(code)
	{
	    // case 0 : data transfer.transfer the data to local process native method.
		case 0:
		{
			if (mOnHander.dataArrival != NULL )
				mOnHander.dataArrival(data);

			reply->writeInt32(2222);
			return NO_ERROR;
		}
		// case 1 : used  to dump the coverage.ec
		// this method is completed in bt_framework BtFw_dumpEmmaReport
		case 1:
		{
			if (mOnHander.dump != NULL )
				mOnHander.dump();
			return NO_ERROR;
		}
		// case 2 : call back to call private method
		// this method is completed in bt_framework BtFw_callPrivateMethod
		case 2:
		{
		    //get method param from Parcel
			int classId = data.readInt32();
			int id = data.readInt32();

			if (mOnHander.cbPrivate != NULL)
			{
				mOnHander.cbPrivate(classId, id);
			}
			return NO_ERROR;
		}
		default:
			return BBinder::onTransact(code,data,reply,flags);
	}
}

};


