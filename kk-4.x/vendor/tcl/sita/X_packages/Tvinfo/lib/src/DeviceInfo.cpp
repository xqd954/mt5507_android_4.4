#include "DeviceInfo.h"
#include "DatabaseCtrl.h"


char strSQL[QUERY_STRING_LENGTH];

pthread_mutex_t m_DBmutex;

DeviceInfo* DeviceInfo::m_pInstance = NULL;

DeviceInfo::DeviceInfo()
{
	PTH_RET_CHK(pthread_mutex_init(&m_DBmutex, NULL));
}

DeviceInfo::~DeviceInfo(void)
{
	if(DeviceInfo::m_pInstance )
	{
		delete DeviceInfo::m_pInstance;
	}
}

DeviceInfo* DeviceInfo::getInstance()
{
	if (m_pInstance == NULL)
	{
		m_pInstance = new DeviceInfo();
	}
	return m_pInstance;
}

void DeviceInfo::device_info_checkDatabase()
{
#if 0
	sqlite3 *pSqldb;
	sqlite3_stmt *pSqlstmt;
	SQL_Check Check(SQL_DB_TCL_DEVICEINFO_PATH, &pSqldb, &pSqlstmt, &m_DBmutex);
#endif
}

int getIntValue(const char * Title, const int projectID) {
	char m_Condition[QUERY_CONDITION_LENGTH]={0};
	sqlite3 *pSqldb;
	sqlite3_stmt *pSqlstmt;
	int m_ColNum;
	int mRet = -1;
	
    memset(strSQL, '\0', QUERY_STRING_LENGTH);
	snprintf(m_Condition, QUERY_CONDITION_LENGTH, "ProjectID=%d",projectID);

	SQL_Connect Connect(SQL_DB_TCL_DEVICEINFO_PATH, &pSqldb, &m_DBmutex);
	SQL_LookupPre  prepear(pSqldb, &pSqlstmt, TABLE_DeviceInfo, strSQL, m_ColNum,m_Condition);
	if(SQLITE_ROW == SQL_Step(pSqlstmt))
	{
		mRet = SQL_GetU32(pSqlstmt,Title);
	}

	return mRet;
}

const char* getTextValue(const char * Title, const int projectID) {
	char m_Condition[QUERY_CONDITION_LENGTH]={0};
	sqlite3 *pSqldb;
	sqlite3_stmt *pSqlstmt;
	int m_ColNum;

	const char *textValue[50];
    memset(textValue, '\0', 50);
    memset(strSQL, '\0', QUERY_STRING_LENGTH);
	snprintf(m_Condition, QUERY_CONDITION_LENGTH, "ProjectID=%d",projectID);
	
	SQL_Connect Connect(SQL_DB_TCL_DEVICEINFO_PATH, &pSqldb, &m_DBmutex);
	SQL_LookupPre  prepear(pSqldb, &pSqlstmt, TABLE_DeviceInfo, strSQL, m_ColNum,m_Condition);
	if(SQLITE_ROW == SQL_Step(pSqlstmt))
	{
		strcpy((char *)&textValue[0], (const char *)SQL_GetText(pSqlstmt,Title));
	}

	return ((char *)textValue);
}

void DeviceInfo::device_info_getProjectList(int* prjList, int &len)
{
	sqlite3 *pSqldb;
	sqlite3_stmt *pSqlstmt;
	int m_ColNum, m_Step = 0;
	memset(strSQL, '\0', QUERY_STRING_LENGTH);
	SQL_Connect Connect(SQL_DB_TCL_DEVICEINFO_PATH, &pSqldb, &m_DBmutex);
	SQL_LookupPre  prepear(pSqldb, &pSqlstmt, TABLE_DeviceInfo, strSQL, m_ColNum, NULL);
	while(SQLITE_ROW == SQL_Step(pSqlstmt)) {
		prjList[m_Step] = SQL_GetU32(pSqlstmt,"ProjectID");
		m_Step++;
	}
	
	len = m_Step;
}

void DeviceInfo::device_info_getClientType(const int projectID, char* clientType)
{
	memset(clientType, '\0', 50);
	strcpy((char *)&clientType[0], getTextValue("ClientType", projectID));
	
}

void DeviceInfo::device_info_getModelName(const int projectID, char* modelName)
{
	memset(modelName, '\0', 50);
	strcpy((char *)&modelName[0], getTextValue("ModelName", projectID));
}

void DeviceInfo::device_info_getPanelName(const int projectID, char* panelName)
{
	memset(panelName, '\0', 50);
	strcpy((char *)&panelName[0], getTextValue("PanelName", projectID));
}

int DeviceInfo::device_info_getPanelType(const int projectID)
{
	return getIntValue("PanelType", projectID);
}

void DeviceInfo::device_info_getPSUName(const int projectID, char* psuName)
{
	memset(psuName, '\0', 50);
	strcpy((char *)&psuName[0], getTextValue("PSUName", projectID));
}

void DeviceInfo::device_info_getRCUName(const int projectID, char* rcuName)
{
	memset(rcuName, '\0', 50);
	strcpy((char *)&rcuName[0], getTextValue("RCUName", projectID));
}

int DeviceInfo::device_info_get3DGlassesType(const int projectID)
{
	return getIntValue("3DGlassesType", projectID);
}

int DeviceInfo::device_info_get3DStatus(const int projectID)
{
	return getIntValue("3DStatus", projectID);
}

int DeviceInfo::device_info_getWifiType(const int projectID)
{
	return getIntValue("WifiType", projectID);
}

int DeviceInfo::device_info_getWifiStatus(const int projectID)
{
	return getIntValue("WifiStatus", projectID);
}

int DeviceInfo::device_info_getMemc(const int projectID)
{
	return getIntValue("Memc", projectID);
}

