/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 *
 * MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

#ifdef __BT_MCAP__

#include <bttypes.h>
#include <btalloc.h>
#include <sys/mcapi.h>
#include <sys/debug.h>

/****************************************************************************
 *
 * RAM Data
 *
 ****************************************************************************/
#if XA_CONTEXT_PTR == XA_ENABLED
static McapContext  tempContext;
McapContext        *mcapcx = &tempContext;
#else /* XA_CONTEXT_PTR == XA_ENABLED */
McapContext         mcapcx;
#endif /* XA_CONTEXT_PTR == XA_ENABLED */


/****************************************************************************
 *
 * Function prototypes
 *
 ****************************************************************************/
/*---------------------------------------------------------------------------
 *            MCAP_Init()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Initialize MCAP. This should be the first function called for
 *            MCAP. 
 *
 * Return:    TRUE - MCAP was successfully initialized.
 *            FALSE  - MCAP failed to initialize.
 */
BOOL MCAP_Init(void)
{
	U8	i;
    OS_Report("[MCAP]MCAP_Init");
    OS_LockStack();

#if XA_CONTEXT_PTR == XA_ENABLED
    OS_MemSet((U8 *)mcapcx, 0, sizeof(McapContext));
#else /* XA_CONTEXT_PTR == XA_ENABLED */
    OS_MemSet((U8 *)&mcapcx, 0, sizeof(McapContext));
#endif /* XA_CONTEXT_PTR == XA_ENABLED */

    /* Initialize MCL state machine */
	MCAP(mclState)[0] = McapMclStateIdle;
	MCAP(mclState)[1] = McapMclStateConnected;
	MCAP(mclState)[2] = McapMclStateActive;
	MCAP(mclState)[3] = McapMclStatePending;
	MCAP(mclState)[4] = McapMclStateDisconnecting;
	
    /* Initialize MDL state machine */
    MCAP(mdlState)[0] = McapMdlStateClosed;
    MCAP(mdlState)[1] = McapMdlStateOpen;
    MCAP(mdlState)[2] = McapMdlStateConnPending;
    MCAP(mdlState)[3] = McapMdlStateDiscPending;

	/* Initialize MCL List */
    InitializeListHead(&MCAP(mcapMclList));
	
	
    OS_Report("[MCAP]Initialized");

    OS_UnlockStack();
    return TRUE;
}

/*---------------------------------------------------------------------------
 *            MCAP_DeInit()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deinitialize MCAP. This should be the last function called for
 *            MCAP. 
 *
 * Return:    BT_STATUS_SUCCESS - service was successfully removed.
 *            BT_STATUS_FAILED  - MCAP was not initialized.
 */
BtStatus MCAP_DeInit(void)
{
	McapMcl*	mcl;
	McapMdl*	mdl;
	
    OS_LockStack();
	/*
	mcl = (McapMcl*)GetHeadList(&MCAP(mcapMclList));
	while((ListEntry*)mcl != &MCAP(mcapMclList))
	{
		mdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList);
		while((ListEntry*)mdl != &mcl->mcapMdlList)
		{
			McapFreeMdl(mcl, mdl);
			mdl = (McapMdl *)GetNextNode(&mdl->node);
		}
		McapFreeMcl(mcl);
        mcl = (McapMcl *)GetNextNode(&mcl->node);
	}
	*/
    OS_Report("[MCAP]Deinitialized");

    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            MCAP_Register()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Register the MCAP callback. This should be the first function
 *            called for MCAP. 
 *
 * Return:    BT_STATUS_SUCCESS - The MCAP callback was successfully registered.
 *            BT_STATUS_FAILED  - MCAP callback registration failed.
 *            BT_STATUS_INVALID_PARM - Invalid parameter.
 *            BT_STATUS_NO_RESOURCES - A MCAP callback was already registered.
 */
BtStatus MCAP_Register(McapRegister* profileRegister)
{
    BtStatus status;

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, profileRegister != 0);
    CheckUnlockedParm(BT_STATUS_NO_RESOURCES, MCAP(callback) == 0);

    OS_LockStack();

    /* Register MCAP Control channel with L2CAP */
    L2CAP_InitDefaultPSM( &MCAP(controlPsm), L2MODE_ENHANCEDMENT_RETRANSMISSION);
    
    MCAP(controlPsm).callback = L2McapMclCallback;
    MCAP(controlPsm).psm = BT_DYNAMIC_PSM;
    MCAP(controlPsm).minRemoteMtu = MCAP_MINIMUM_MTU;
    MCAP(controlPsm).localMtu =L2CAP_MAXIMUM_MTU;
    MCAP(controlPsm).authorizedSetting = FALSE;
#ifdef __BT_2_1_ERTM__    
    MCAP(controlPsm).channelUsage = L2CAP_CHANNEL_GENERAL;
    MCAP(controlPsm).inBufferSize = (L2CAP_MAXIMUM_MTU + 7)*10;
#endif    
#if L2CAP_FLOW_CONTROL == XA_ENABLED
    MCAP(controlPsm).inLinkMode = MCAP(controlPsm).outLinkMode = L2MODE_ENHANCEDMENT_RETRANSMISSION;
    //MCAP(controlPsm).inLinkMode = MCAP(controlPsm).outLinkMode = L2MODE_BASIC;
	MCAP(controlPsm).inTransmitMax = MCAP_MAX_RETRANS_NUM;
#endif /* L2CAP_FLOW_CONTROL == XA_ENABLED */
#ifdef __BT_2_1_ERTM__
    MCAP(controlPsm).lockStepNeeded = 0;
    MCAP(controlPsm).highSpeedRequired = 0;
#endif
	OS_Report("[MCAP]MCAP(controlPsm).minRemoteMtu: %d",MCAP(controlPsm).minRemoteMtu);

    if ((status = L2CAP_RegisterPsm(&(MCAP(controlPsm)))) != BT_STATUS_SUCCESS) {
        /* Unable to register */
        OS_Report("[MCAP]Unable to register control channel PSM with L2CAP: %d",status);
        status = BT_STATUS_FAILED;
        goto fail;
    }
	OS_Report("[MCAP]Assigned PSM for control channel: %x",MCAP(controlPsm).psm);
	profileRegister->mclPsm = MCAP(controlPsm).psm;
	
	/* Register MCAP data channel with L2CAP */
	L2CAP_InitDefaultPSM( &MCAP(dataPsm), (L2MODE_ENHANCEDMENT_RETRANSMISSION | L2MODE_STREAMMING) );
    
	MCAP(dataPsm).callback = L2McapMdlCallback;
	MCAP(dataPsm).psm = BT_DYNAMIC_PSM;
	MCAP(dataPsm).minRemoteMtu = MCAP_MINIMUM_MTU;
    MCAP(dataPsm).localMtu =L2CAP_MAXIMUM_MTU;
    MCAP(dataPsm).authorizedSetting = FALSE;
#ifdef __BT_2_1_ERTM__    
    MCAP(dataPsm).channelUsage = L2CAP_CHANNEL_GENERAL;
    MCAP(dataPsm).inBufferSize = (L2CAP_MAXIMUM_MTU + 7)*10;
#endif    
#if L2CAP_FLOW_CONTROL == XA_ENABLED
	MCAP(dataPsm).inLinkMode = MCAP(dataPsm).outLinkMode = L2MODE_ENHANCEDMENT_RETRANSMISSION | L2MODE_STREAMMING;
	MCAP(dataPsm).inTransmitMax = MCAP_MAX_RETRANS_NUM;
#endif /* L2CAP_FLOW_CONTROL == XA_ENABLED */
#ifdef __BT_2_1_ERTM__
	MCAP(dataPsm).lockStepNeeded = 0;
	MCAP(dataPsm).highSpeedRequired = 0;
#endif
	
	if ((status = L2CAP_RegisterPsm(&(MCAP(dataPsm)))) != BT_STATUS_SUCCESS) {
		/* Unable to register */
		OS_Report("[MCAP]Unable to register data channel PSM with L2CAP: %d",status);
		status = BT_STATUS_FAILED;
		goto fail;
	}	
	OS_Report("[MCAP]Assigned PSM for data channel: %x",MCAP(dataPsm).psm);
	profileRegister->mdlPsm = MCAP(dataPsm).psm;


#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    kal_trace(BT_TRACE_G2_PROFILES, TRY_TO_REGISTER_SECURITY_RECORD);
	/*Register MCL security record*/
    InitializeListEntry(&(MCAP(mclSecRecord).node));
    MCAP(mclSecRecord).id = SEC_L2CAP_ID;
    MCAP(mclSecRecord).channel = MCAP(controlPsm).psm;
    MCAP(mclSecRecord).level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT | BSL_ENCRYPTION_IN | BSL_ENCRYPTION_OUT;
#ifndef __BT_2_1_SIMPLE_PAIRING__
    MCAP(mclSecRecord).level_type = BT_SEC_LOW;
#else
    MCAP(mclSecRecord).level_type = BT_SEC_MED;
#endif
    if((status = SEC_RegisterRecord(&MCAP(mclSecRecord))) != BT_STATUS_SUCCESS)
    {
		/* Unable to register */
		OS_Report("[MCAP]Unable to register control channel security record with L2CAP");
		status = BT_STATUS_FAILED;
		goto fail;
	}

	/*Register MDL security record*/
    InitializeListEntry(&(MCAP(mdlSecRecord).node));
    MCAP(mdlSecRecord).id = SEC_L2CAP_ID;
    MCAP(mdlSecRecord).channel = MCAP(dataPsm).psm;
    MCAP(mdlSecRecord).level = BSL_AUTHENTICATION_IN | BSL_AUTHENTICATION_OUT | BSL_ENCRYPTION_IN | BSL_ENCRYPTION_OUT;
#ifndef __BT_2_1_SIMPLE_PAIRING__
    MCAP(mdlSecRecord).level_type = BT_SEC_LOW;
#else
    MCAP(mdlSecRecord).level_type = BT_SEC_MED;
#endif
    if((status = SEC_RegisterRecord(&MCAP(mdlSecRecord))) != BT_STATUS_SUCCESS)
    {
		/* Unable to register */
		OS_Report("[MCAP]Unable to register data channel security record with L2CAP");
		status = BT_STATUS_FAILED;
		goto fail;
	}

#endif

    MCAP(callback) = profileRegister->callback;

    OS_Report("[MCAP]Registered");
	return status;
fail:
    OS_UnlockStack();
    return BT_STATUS_SUCCESS;
}

/*---------------------------------------------------------------------------
 *            MCAP_DeRegister()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Deregister a MCAP callback. This should be the last function
 *            called for MCAP. 
 *
 * Return:    BT_STATUS_SUCCESS - callback was successfully deregistered.
 *            BT_STATUS_FAILED  - a MCAP callback is not registered.
 *            BT_STATUS_IN_PROGRESS - MCAP PANU currently up. Call 
                    MCAP_Close() first.
 */
BtStatus MCAP_DeRegister(McapUpperProfile profile)
{
    U8 i;
    BtStatus status = 0;

    CheckUnlockedParm(BT_STATUS_FAILED, MCAP(callback) != 0);

    OS_LockStack();

#if L2CAP_DEREGISTER_FUNC == XA_ENABLED
    status += L2CAP_DeregisterPsm(&(MCAP(controlPsm)));
	status += L2CAP_DeregisterPsm(&(MCAP(dataPsm)));
    if (status != BT_STATUS_SUCCESS)
    {
        return status;
    }
#endif

#if (BT_SEC_LEVEL2_SETTING_ENABLED == XA_ENABLED)
    status += SEC_Unregister(&MCAP(mclSecRecord));
	status += SEC_Unregister(&MCAP(mdlSecRecord));
    if (status != BT_STATUS_SUCCESS)
    {
        return status;
    }
#endif

    MCAP(callback) = 0;
    status = BT_STATUS_SUCCESS;

    OS_Report("[MCAP]Deregistered");

fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            MCAP_OpenMcl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open a MCAP MCL 
 *
 * Return:    BT_STATUS_PENDING - MCAP mcl open process was successfully 
 */
BtStatus MCAP_OpenMcl(BtRemoteDevice *remDev, L2capPsmValue mclPsm)
{
    BtStatus status;
	McapMcl* mcl;
    McapCallbackParms	appParms;
	
    OS_Report("[MCAP]----------Open MCL connection-----------");
	
    CheckUnlockedParm(BT_STATUS_FAILED, MCAP(callback) != 0);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, remDev != 0);

    OS_LockStack();

    mcl = McapNewMcl(remDev, MCAP_MCL_ROLE_INITIATOR, L2CID_INVALID);
	if(mcl == NULL)
		return BT_STATUS_FAILED;
	if(McapCheckMclState(mcl,MCAP_MCL_STATE_ACTIVE) || 
		McapCheckMclState(mcl,MCAP_MCL_STATE_CONNECTED) ||
		McapCheckMclState(mcl,MCAP_MCL_STATE_PENDING))
	{
		appParms.mcl = mcl; 
		appParms.remDev = remDev;
		McapAppCallBack(&appParms, BT_STATUS_SUCCESS, MCAPEVENT_MCLOPENED);
		return BT_STATUS_SUCCESS;
	}
	else if (McapCheckMclState(mcl,MCAP_MCL_STATE_DISCONNECTING))
	{
		mcl->remPsm = mclPsm;
		mcl->reopen = TRUE;
		return BT_STATUS_PENDING;
	}
	
	mcl->remPsm = mclPsm;

	
    /* Establish a new connection to the device */
    status = L2CAP_ConnectReq(&(MCAP(controlPsm)), mclPsm, 
                    remDev, 0, &mcl->l2ChannelId);
    OS_Report("[MCAP:]Establishing L2CAP connection for MCL: CID = %d", mcl->l2ChannelId);
    if (status != BT_STATUS_PENDING) {
        OS_Report("[MCAP]L2CAP connect request failed %d", status);
        McapFreeMcl(mcl);
        goto fail;
    }

#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
    if( BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status )
    {
        L2capConnSettings Setting;

        Setting.reasmBuffer = (ListEntry *)&(mcl->rxPacketsBufferList);
        Setting.receivedBuffer = (ListEntry *)&(mcl->receivedPacketsList);
        Setting.rxWindowSize = MCAP_MAX_RETRANS_NUM;  
         OS_MemSet((U8 *)(mcl->txMaxCountPerIFrame), 0, 128); /* Always 128 */
         OS_MemSet((U8 *)(mcl->txSrejList), 0, sizeof(mcl->txSrejList) ); 
        Setting.txMaxTransmitCount = (U8 *)(mcl->txMaxCountPerIFrame);
        Setting.txSrejList = (U8 *)(mcl->txSrejList);         
        Setting.maxSrejNo = RF_L2CAP_MAX_SREJ_SIZE;

        L2CAP_ConnectReqAssignFlowControlBuffer(&Setting, mcl->l2ChannelId);

    }
#endif


fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            MCAP_CloseMcl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close a MCAP MCL 
 *
 * Return:    BT_STATUS_PENDING - MCAP mcl connection closed 
 */

BtStatus MCAP_CloseMcl(McapMcl *mcl)
{
    BtStatus 			status;
	McapControlPacket*	controlPkt;
	
    OS_Report("[MCAP]--------------MCAP_CloseMcl-----------------");
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, mcl != NULL);
	
    CheckUnlockedParm(BT_STATUS_FAILED, McapCheckMclState(mcl, MCAP_MCL_STATE_ACTIVE) || McapCheckMclState(mcl, MCAP_MCL_STATE_CONNECTED));

    OS_LockStack();
		status = L2CAP_DisconnectReq(mcl->l2ChannelId);

    OS_UnlockStack();
    return status;
}


/*---------------------------------------------------------------------------
 *            MCAP_Config()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Config the L2CAP connection of a MCAP MDL 
 *
 * Return:    BT_STATUS_PENDING - MCAP config mdl command was sent successfully 
 */
BtStatus MCAP_Config(McapMcl *mcl, U8 mdepId, U8 configuration)
{
    BtStatus 	status;
	McapMdl*	mdl;
	McapCallbackParms		parms;
	
    OS_Report("[MCAP]-----------Config the MDL----------------");

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, mcl != NULL);
    CheckUnlockedParm(BT_STATUS_FAILED, !McapCheckMclState(mcl, MCAP_MCL_STATE_IDLE));
    CheckUnlockedParm(BT_STATUS_FAILED, !McapCheckMclState(mcl, MCAP_MCL_STATE_PENDING));

    OS_LockStack();	
	
	mdl = McapNewMdl(mcl, MCAP_MDLID_RESERVED, mdepId);
	status = McapConfigMdl(mcl, mdl, configuration);
	
    OS_Report("[MCAP]Config the MDL status:%d",status);
	if(status != BT_STATUS_PENDING)
	{
		parms.mcl = mcl;
		parms.mdl = mdl;
		parms.remDev = mcl->remDev;
		McapAppCallBack(&parms, BT_STATUS_FAILED, MCAPEVENT_MDLCONFIG_CNF);
		McapFreeMdl(mcl,mdl);
	}

fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            MCAP_OpenMdl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Open the L2CAP connection of a MCAP MDL 
 *
 * Return:    BT_STATUS_PENDING - MCAP mdl open process was successfully 
 */
BtStatus MCAP_OpenMdl(McapMdl *mdl, L2capPsmValue mdlPsm, BtRemoteDevice *remDev, MdlL2Configuration* l2config)
{
    BtStatus 	status;
	McapMcl*	mcl;
	
    OS_Report("[MCAP]-----------Open the MDL--------------");

    CheckUnlockedParm(BT_STATUS_INVALID_PARM, mdl != NULL);
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, remDev != 0);
    CheckUnlockedParm(BT_STATUS_FAILED, McapCheckMdlState(mdl, MCAP_MDL_STATE_CLOSED) || McapCheckMdlState(mdl, MCAP_MDL_STATE_OPEN));

    OS_LockStack();	
#ifdef __BT_2_1_ERTM__    
	MCAP(dataPsm).inLinkMode = mdl->l2config.inLinkMode = l2config->inLinkMode;
    OS_Report("[MCAP]MCAP(dataPsm).inLinkMode:%x",MCAP(dataPsm).inLinkMode);
#endif    
	mdl->mdlPsm = mdlPsm;
	mdl->connFlag = TRUE;

	if(McapCheckMdlState(mdl, MCAP_MDL_STATE_OPEN))
	{
		OS_Report("[MCAP]Reset this MDL channel");
		mdl->resetFlag = TRUE;
		status = L2CAP_DisconnectReq(mdl->l2ChannelId);
		return status;
	}
	
    status = L2CAP_ConnectReq(&(MCAP(dataPsm)), mdlPsm, 
                    remDev, 0, &mdl->l2ChannelId);
    OS_Report("[MCAP:]Establishing L2CAP connection for MDL: CID = %d", mdl->l2ChannelId);
    if (status != BT_STATUS_PENDING) {
        OS_Report("[MCAP]L2CAP connect request failed %d", status);
		mcl = McapFindMclByBTAddr(remDev->bdAddr);
        McapFreeMdl(mcl, mdl);
        goto fail;
    }
	
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED
		if( BT_STATUS_PENDING == status || BT_STATUS_SUCCESS == status )
		{
			L2capConnSettings Setting;
	
			Setting.reasmBuffer = (ListEntry *)&(mdl->rxPacketsBufferList);
			Setting.receivedBuffer = (ListEntry *)&(mdl->receivedPacketsList);
			Setting.rxWindowSize = MCAP_MAX_RETRANS_NUM;  
			 OS_MemSet((U8 *)(mdl->txMaxCountPerIFrame), 0, 128); /* Always 128 */
			 OS_MemSet((U8 *)(mdl->txSrejList), 0, sizeof(mdl->txSrejList) ); 
			Setting.txMaxTransmitCount = (U8 *)(mdl->txMaxCountPerIFrame);
			Setting.txSrejList = (U8 *)(mdl->txSrejList);		  
			Setting.maxSrejNo = RF_L2CAP_MAX_SREJ_SIZE;
	
			L2CAP_ConnectReqAssignFlowControlBuffer(&Setting, mdl->l2ChannelId);	
		}
#endif

	McapMdlStateTrans(mdl,MCAP_MDL_STATE_CONN_PENDING);
fail:
    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            MCAP_CloseMdl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close a MCAP mdl 
 *
 * Return:    BT_STATUS_PENDING - Delete mdl command was sent successfully 
 */
BtStatus MCAP_CloseMdl(McapMcl *mcl, McapMdl *mdl)
{
    BtStatus 			status;
	McapControlPacket*	controlPkt;
	McapMdl*	tmpMdl;
	
    OS_Report("[MCAP]--------------MCAP_CloseMdl-----------------");
    CheckUnlockedParm(BT_STATUS_INVALID_PARM, mcl != NULL);
	CheckUnlockedParm(BT_STATUS_INVALID_PARM, mdl != NULL);
	
    CheckUnlockedParm(BT_STATUS_FAILED, McapCheckMclState(mcl, MCAP_MCL_STATE_ACTIVE) || McapCheckMclState(mcl, MCAP_MCL_STATE_CONNECTED));
    //CheckUnlockedParm(BT_STATUS_FAILED, McapCheckMdlState(mdl, MCAP_MDL_STATE_OPEN));

    OS_LockStack();
	if(mdl->mdlId == MCAP_ALL_MDLIDS)
	{
		tmpMdl = (McapMdl*)GetHeadList(&mcl->mcapMdlList); 
		while((ListEntry*)tmpMdl != &mcl->mcapMdlList)
		{
			if(McapCheckMdlState(tmpMdl,MCAP_MDL_STATE_OPEN))
			{
				tmpMdl->delFlag = TRUE;
				//L2CAP_DisconnectReq(mdl->l2ChannelId);
				McapMdlStateTrans(tmpMdl,MCAP_MDL_STATE_DISC_PENDING);
			}
			tmpMdl = (McapMdl *)GetNextNode(&tmpMdl->node);
		}
	}
	else 
	{
		if(McapCheckMdlState(mdl,MCAP_MDL_STATE_OPEN))
		{
			mdl->delFlag = TRUE;
			McapMdlStateTrans(mdl,MCAP_MDL_STATE_DISC_PENDING);
		}
	}
	
	controlPkt = (McapControlPacket*)mcap_malloc(sizeof(McapControlPacket));
	controlPkt->opCode = MCAP_MD_DELETE_MDL_REQ;
	controlPkt->mdlId = mdl->mdlId;
	status = McapSendControl(mcl, controlPkt);

    OS_UnlockStack();
    return status;
}

/*---------------------------------------------------------------------------
 *            MCAP_AcceptMcl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accept the MCAP MCL connection.  
 *
 * Return:    BT_STATUS_PENDING - MCAP mcl connection accepted.
 */
BtStatus MCAP_AcceptMcl(McapMcl *mcl, BOOL result)
{
    BtStatus status;

    OS_Report("[MCAP]--------------MCAP_AcceptMcl-----------------");

	CheckUnlockedParm(BT_STATUS_INVALID_PARM, mcl != NULL);

    OS_LockStack();
	if(result)
	{		
#if L2CAP_FLOW_NON_BASIC_MODE == XA_ENABLED	
//#if 0
		L2capConnSettings Setting;
	
		Setting.reasmBuffer = (ListEntry *)&(mcl->rxPacketsBufferList);
		Setting.receivedBuffer = (ListEntry *)&(mcl->receivedPacketsList);
		Setting.rxWindowSize = MCAP_MAX_RETRANS_NUM;  
		OS_MemSet((U8 *)(mcl->txMaxCountPerIFrame), 0, 128); /* Always 128 */
		OS_MemSet((U8 *)(mcl->txSrejList), 0, sizeof(mcl->txSrejList) ); 
		Setting.txMaxTransmitCount = (U8 *)(mcl->txMaxCountPerIFrame);
		Setting.txSrejList = (U8 *)(mcl->txSrejList);		  
		Setting.maxSrejNo = RF_L2CAP_MAX_SREJ_SIZE;	
			
		status = L2CAP_ConnectRsp(mcl->l2ChannelId, L2CONN_ACCEPTED, &Setting);
#else
		status = L2CAP_ConnectRsp(mcl->l2ChannelId, L2CONN_ACCEPTED, NULL);
#endif
	}
	else
	{
        status = L2CAP_ConnectRsp(mcl->l2ChannelId, L2CONN_REJECT_SECURITY_BLOCK, 0);
	}

    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            MCAP_AcceptMdl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Accept the MCAP mdl config.  
 *
 * Return:    BT_STATUS_PENDING - MCAP mdl config accepted.
 */
BtStatus MCAP_AcceptMdl(McapMcl *mcl, McapMdl *mdl, BOOL result, U8 configuration)
{
    BtStatus 			status;
	McapControlPacket*	controlPkt;

    OS_Report("[MCAP]------------MCAP_AcceptMdl-------------");

	CheckUnlockedParm(BT_STATUS_INVALID_PARM, mcl != NULL);
	CheckUnlockedParm(BT_STATUS_INVALID_PARM, mdl != NULL);
	
    CheckUnlockedParm(BT_STATUS_FAILED, McapCheckMclState(mcl, MCAP_MCL_STATE_ACTIVE) || McapCheckMclState(mcl, MCAP_MCL_STATE_CONNECTED));

    OS_LockStack();

    switch(configuration){
        case 0x01:
	        mdl->l2config.inLinkMode = L2MODE_ENHANCEDMENT_RETRANSMISSION;
	        break;
        case 0x02:
	        mdl->l2config.inLinkMode = L2MODE_STREAMMING;
	        break;
        default:
	        mdl->l2config.inLinkMode = L2MODE_ENHANCEDMENT_RETRANSMISSION | L2MODE_STREAMMING ;
	        break;
    }
#ifdef __BT_2_1_ERTM__
    // change the psm to accept 
    MCAP(dataPsm).inLinkMode = mdl->l2config.inLinkMode;	
    OS_Report("[MCAP] mdl configed mode:0x%x", MCAP(dataPsm).inLinkMode);
#endif	
	controlPkt = (McapControlPacket*)mcap_malloc(sizeof(McapControlPacket));
	controlPkt->opCode = MCAP_MD_CREATE_MDL_RSP;
	if(result)
	{
		controlPkt->responseCode = MCAP_SUCCESS;
	}
	else
	{
		controlPkt->responseCode = MCAP_CONFIGURATION_REJECTED;
	}
	controlPkt->mdlId = mdl->mdlId;
	controlPkt->configuration = configuration;

	/*Delete the MDL once reject the create MDL request*/
	
	status = McapSendControl(mcl, controlPkt);
	if(status != BT_STATUS_PENDING)
	{
		if(result)
			McapFreeMdl(mcl, mdl);
		return status;
	}
	
	if(!result)
	{
		McapFreeMdl(mcl, mdl);
		return BT_STATUS_SUCCESS;
	}

	if(McapCheckMdlState(mdl, MCAP_MDL_STATE_OPEN))
	{
		OS_Report("[MCAP]Reset this mdl channel");
		mdl->resetFlag = TRUE;
	}
	
	McapMclStateTrans(mcl,MCAP_MCL_STATE_PENDING);
	mcl->pendingMdl = mdl;

	if(mdl->resetFlag != TRUE)
		McapMdlStateTrans(mdl,MCAP_MDL_STATE_CONN_PENDING);
	
    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            MCAP_AbortMdl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Close the MCAP mdl.  
 *
 * Return:    BT_STATUS_SUCCESS - MCAP mdl closed.
 *            BT_STATUS_PENDING - MCAP mdl close started 
 */
BtStatus MCAP_AbortMdl(McapMcl *mcl, McapMdl *mdl)
{
    BtStatus 			status;
	McapControlPacket*	controlPkt;

    OS_Report("[MCAP]-------------MCAP_AbortMdl------------");

	CheckUnlockedParm(BT_STATUS_INVALID_PARM, mcl != NULL);
	CheckUnlockedParm(BT_STATUS_INVALID_PARM, mdl != NULL);
	
    CheckUnlockedParm(BT_STATUS_FAILED, McapCheckMclState(mcl, MCAP_MCL_STATE_ACTIVE));
    CheckUnlockedParm(BT_STATUS_FAILED, McapCheckMdlState(mdl, MCAP_MDL_STATE_CONN_PENDING) || McapCheckMdlState(mdl, MCAP_MDL_STATE_CLOSED));

    OS_LockStack();
	controlPkt = (McapControlPacket*)mcap_malloc(sizeof(McapControlPacket));
	controlPkt->opCode = MCAP_MD_ABORT_MDL_REQ;
	controlPkt->mdlId = mdl->mdlId;
	
	status = McapSendControl(mcl, controlPkt);
    OS_UnlockStack();

    return status;
}

/*---------------------------------------------------------------------------
 *            MCAP_ReconnectMdl()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Reconnect mdl.  
 *
 * Return:    BT_STATUS_PENDING - send reconnect command ok.
 */
BtStatus MCAP_ReconnectMdl(McapMcl *mcl, McapMdl *mdl)
{
    BtStatus 			status;
	McapControlPacket*	controlPkt;

    OS_Report("[MCAP]------------MCAP_ReconnectMdl----------------");

	CheckUnlockedParm(BT_STATUS_INVALID_PARM, mcl != NULL);
	CheckUnlockedParm(BT_STATUS_INVALID_PARM, mdl != NULL);
	
    CheckUnlockedParm(BT_STATUS_FAILED, McapCheckMclState(mcl, MCAP_MCL_STATE_ACTIVE) || McapCheckMclState(mcl, MCAP_MCL_STATE_CONNECTED));
    CheckUnlockedParm(BT_STATUS_FAILED, McapCheckMdlState(mdl, MCAP_MDL_STATE_CLOSED));

    OS_LockStack();
	mdl->connFlag = TRUE;
	controlPkt = (McapControlPacket*)mcap_malloc(sizeof(McapControlPacket));
	controlPkt->opCode = MCAP_MD_RECONNECT_MDL_REQ;
	controlPkt->mdlId = mdl->mdlId;
	
	status = McapSendControl(mcl, controlPkt);
    OS_UnlockStack();

    return status;
}


/*---------------------------------------------------------------------------
 *            MCAP_SendData()
 *---------------------------------------------------------------------------
 *
 * Synopsis:  Send an  packet to the remote device.
 *
 * Return:    BT_STATUS_PENDING the send packet process has been 
 *            successfully started.
 */
BtStatus MCAP_SendData(McapMdl *mdl,
							   BtPacket *btPacket,
							   McapDataPacket *txPkt)
{
    BtStatus        status;
    OS_Report("[MCAP]--------------MCAP_SendData---------------");

	CheckUnlockedParm(BT_STATUS_INVALID_PARM, btPacket != NULL);
	CheckUnlockedParm(BT_STATUS_INVALID_PARM, mdl != NULL);
	
    CheckUnlockedParm(BT_STATUS_FAILED, McapCheckMdlState(mdl, MCAP_MDL_STATE_OPEN));

    OS_LockStack();
	status = McapSendData(mdl, btPacket, txPkt);
    OS_UnlockStack();
    return status;
}

#endif /* __BT_MCAP__ */

