#ifndef WEB_SNMP_DETAIL_CONFIG
#define WEB_SNMP_DETAIL_CONFIG

//#include <dbus/dbus.h>
//#include "dbus/wcpss/ACDbusDef.h"
//#include "dbus/asd/ASDDbusDef.h"

//#include "../ws_init_dbus.h"

// ap inner
#define wtpDownTrap    						"wtpDownTrap"						//0.1
#define wtpSysStartTrap 					"wtpSysStartTrap"					//0.2
#define wtpChannelModifiedTrap  			"APMtRdoChanlChgTrap"			//0.3
#define wtpIPChangeAlarmTrap				"wtpIPChangeAlarmTrap"				//0.4
#define wtpFlashWriteFailTrap				"wtpFlashWriteFailTrap"				//0.5
#define wtpColdStartTrap					"wtpColdStartTrap"					//0.6
#define wtpAuthModeChangeTrap				"wtpAuthModeChangeTrap"				//0.7
#define wtpPreSharedKeyChangeTrap			"wtpPreSharedKeyChangeTrap"			//0.8
#define wtpElectrifyRegisterCircleTrap		"wtpElectrifyRegisterCircleTrap"	//0.9
#define wtpAPUpdateTrap						"wtpAPUpdateTrap"					//0.10

#define wtpCoverholeTrap					"CoverHoleTrap"					//0.11
#define wtpWirePortExceptionTrap			"wtpWirePortExceptionTrap"			//0.12  not support now
#define CPUusageTooHighTrap					"APCPUusageTooHighTrap"				//0.13
#define CPUusageTooHighRecovTrap			"APCPUusageTooHighRecovTrap"			//0.14
#define MemUsageTooHighTrap					"APMemUsageTooHighTrap"				//0.15
#define MemUsageTooHighRecovTrap			"APMemUsageTooHighRecovTrap"			//0.16
#define TemperTooHighTrap					"APTemperatureTooHighTrap"					//0.17
#define TemperTooHighRecoverTrap			"APTemperTooHighRecovTrap"			//0.18
#define APMtWorkModeChgTrap					"APMtWorkModeChgTrap"				//0.19
#define APswitchBetweenACTrap				"APswitchBetweenACTrap"				//0.20 not support now

#define SSIDkeyConflictTrap					"SSIDkeyConflictTrap"				//0.21
#define wtpOnlineTrap						"APOnlineTrap"						//0.22
#define wtpOfflineTrap						"APOfflineTrap"					//0.23
#define wtpCoverHoleClearTrap				"CoverHoleRecovTrap"				//0.24

//ap app
#define wtpChannelObstructionTrap			"wtpChannelObstructionTrap"				//1.1
#define wtpAPInterferenceDetectedTrap		"CoChAPInterfDetectedTrap" 		//1.2
#define wtpStaInterferenceDetectedTrap		"StaInterfDetectedTrap"		//1.3
#define wtpDeviceInterferenceDetectedTrap	"wtpDeviceInterferenceDetectedTrap"		//1.4
#define wtpSubscriberDatabaseFullTrap		"APStaFullTrap"			//1.5
#define wtpDFSFreeCountBelowThresholdTrap	"wtpDFSFreeCountBelowThresholdTrap"		//1.6
#define wtpFileTransferTrap					"wtpFileTransferTrap"					//1.7
#define wtpStationOffLineTrap				"wtpStationOffLineTrap"					//1.8
#define wtpSolveLinkVarifiedTrap			"wtpSolveLinkVarifiedTrap"				//1.9
#define wtpLinkVarifyFailedTrap				"wtpLinkVarifyFailedTrap"				//1.10

#define wtpStationOnLineTrap				"wtpStationOnLineTrap"					//1.11				
#define APInterfClearTrap					"CoChAPInterfClearTrap"						//1.12
#define APStaInterfClearTrap				"StaInterfClearTrap"					//1.13
#define APOtherDeviceInterfDetectedTrap		"OtherDeviceInterfDetectedTrap"		//1.14 not support now
#define APOtherDevInterfClearTrap			"OtherDevInterfClearTrap"				//1.15
#define APModuleTroubleTrap					"ModuleTroubleTrap"					//1.16
#define APModuleTroubleClearTrap			"ModuleTroubleClearTrap"				//1.17
#define APRadioDownTrap						"RadioDownTrap"						//1.18
#define APRadioDownRecovTrap				"RadioDownRecovTrap"					//1.19
#define APTLAuthenticationFailedTrap		"APTLAuthenticationFailedTrap"			//1.20

#define	APStaAuthErrorTrap					"StaAuthErrorTrap"					//1.21
#define APStAssociationFailTrap				"StAssociationFailTrap"				//1.22
#define APUserWithInvalidCerInbreakNetTrap	"UserWithInvalidCerficationInbreakNetworkTrap"	//1.23
#define APStationRepititiveAttackTrap		"StationRepititiveAttackTrap"			//1.24
#define APTamperAttackTrap					"TamperAttackTrap"					//1.25
#define APLowSafeLevelAttackTrap			"LowSafeLevelAttackTrap"				//1.26
#define APAddressRedirectionAttackTrap		"AddressRedirectionAttackTrap"		//1.27
#define APMeshAuthFailureTrap				"APMeshAuthFailureTrap"					//1.28
#define APChildExcludedParentTrap			"APChildExcludedParentTrap"				//1.29
#define APParentChangeTrap					"APParentChangeTrap"					//1.30
	
#define APChildMovedTrap					"APChildMovedTrap"						//1.31
#define	APExcessiveParentChangeTrap			"APExcessiveParentChangeTrap"			//1.32
#define APMeshOnsetSNRTrap					"APMeshOnsetSNRTrap"					//1.33
#define APMeshAbateSNRTrap					"APMeshAbateSNRTrap"					//1.34
#define APConsoleLoginTrap					"APConsoleLoginTrap"					//1.35
#define APQueueOverflowTrap					"APQueueOverflowTrap"					//1.36
#define APAddUserFailClearTrap				"APStaFullRecoverTrap"				//1.37
#define APChannelTooLowClearTrap			"APChannelTooLowClearTrap"				//1.38
#define APFindUnsafeESSID					"WIDSUnauthorSSIDTrap"						//1.39
#define APFindSYNAttack						"WIDSDetectAttack"						//1.40
#define ApNeighborChannelInterfTrap			"AdjacentChAPInterfDetectedTrap"			//1.41
#define ApNeighborChannelInterfTrapClear	"AdjacentChAPInterfClearTrap"		//1.42

//ac inner
#define acSystemRebootTrap					"SystemWarmStartTrap"					//0.1
#define acAPACTimeSynchroFailureTrap		"APACTimeSyncFailureTrap"			//0.2
#define acChangedIPTrap						"IPAddChangeTrap"						//0.3
#define acTurntoBackupDeviceTrap			"ACTurnToBackupDeviceTrap"				//0.4
#define acConfigurationErrorTrap			"ConfigurationErrorTrap"				//0.5
#define acSysColdStartTrap					"SystmColdStartTrap"					//0.6
#define acHeartbeatTrap						"ACHeartbeatTrap"						//0.7

#define acAPACTimeSynchroFailureTrapClear   "acAPACTimeSynchroFailureTrapClear"		//0.22

//ac app 
#define acDiscoveryDangerAPTrap				"WIDSDetectRogueTrap"				//1.1
#define acRadiusAuthenticationServerNotReachTrap		"RadiusAuthServerUnavailableTrap"	//1.2
#define acRadiusAccountServerNotLinkTrap	"RadioAccServerUnavailableTrap"		//1.3
#define acPortalServerNotReachTrap			"PortalServerUnavaibleTrap"			//1.4
#define acAPLostNetTrap						"acAPLostNetTrap"						//1.5
#define acCPUUtilizationOverThresholdTrap	"ACCPUusageTooHighTrap"		//1.6
#define acMemUtilizationOverThresholdTrap	"ACMemUsageTooHighTrap"		//1.7
#define acBandwithOverThresholdTrap			"acBandwithOverThresholdTrap"			//1.8
#define acLostPackRateOverThresholdTrap		"acLostPackRateOverThresholdTrap"		//1.9
#define acMaxUsrNumOverThresholdTrap		"acMaxUsrNumOverThresholdTrap"			//1.10

#define acDisconnectNumOverThresholdTrap	"acDisconnectNumOverThresholdTrap"		//1.11
#define acDropRateOverThresholdTrap			"acDropRateOverThresholdTrap"			//1.12
#define acOfflineSucRateBelowThresholdTrap	"acOfflineSucRateBelowThresholdTrap"	//1.13
#define acAuthSucRateBelowThresholdTrap		"acAuthSucRateBelowThresholdTrap"		//1.14
#define acAveIPPoolOverThresholdTrap		"acAveIPPoolOverThresholdTrap"			//1.15
#define acMaxIPPoolOverThresholdTrap		"acMaxIPPoolOverThresholdTrap"			//1.16
#define acDHCPSucRateOverThresholdTrap		"acDHCPSucRateOverThresholdTrap"		//1.17
#define acPowerOffTrap						"PowerOffTrap"						//1.18
#define acPowerOffRecovTrap					"PowerOffRecovTrap"					//1.19
#define acCPUusageTooHighRecovTrap			"ACCPUusageTooHighRecovTrap"			//1.20

#define acMemUsageTooHighRecovTrap			"ACMemUsageTooHighRecovTrap"			//1.21
#define acTemperTooHighTrap					"ACTemperatureTooHighTrap"					//1.22
#define acTemperTooHighRecovTrap			"ACTemperTooHighRecovTrap"				//1.23
#define acDHCPAddressExhaustTrap			"ACDHCPAddressExhaustTrap"				//1.24
#define acDHCPAddressExhaustRecovTrap		"ACDHCPAddressExhaustRecovTrap"			//1.25
#define acRadiusAuthServerAvailableTrap		"RadiusAuthServerAvailableTrap"		//1.26
#define acRadiusAccServerAvailableTrap		"RadiusAccServerAvailableTrap"		//1.27
#define acRorterServerAvailableTrap			"PortalServerAvalibleTrap"			//1.28
#define acFindAttackTrap					"acFindAttackTrap"						//1.29

#define acHeartTimePackageTrap				"ACHeartbeatTrap"				//1.50


/**********************trap discr des******************************/
#define PRIVATE_TRAP_DESCR_AP_CPU_OVER_THRESHOLD 			"AP CPU�����ʹ��߸澯"
#define PRIVATE_TRAP_DESCR_AP_CPU_OVER_THRESHOLD_CLEAR 		"AP CPU�����ʹ��߸澯���"

#define PRIVATE_TRAP_DESCR_AP_MEM_OVER_THRESHOLD			"AP �ڴ������ʹ��߸澯"
#define PRIVATE_TRAP_DESCR_AP_MEM_OVER_THRESHOLD_CLEAR		"AP �ڴ������ʹ��߸澯���"

#define PRIVATE_TRAP_DESCR_AP_OFF_LINE						"AP���߸澯"
#define PRIVATE_TRAP_DESCR_AP_ON_LINE						"AP���߸澯"

#define PRIVATE_TRAP_DESCR_AP_MTWORK_CHANGE					"AP���߼��ӹ���ģʽ���ͨ��"
#define PRIVATE_TRAP_DESCR_AP_FLASH_WIRTE_FAIL				"AP�������ʧ��ͨ��"
#define PRIVATE_TRAP_DESCR_AP_SSID_KEY_CONFLICT				"SSID��Կ��ͻͨ��"
#define PRIVATE_TRAP_DESCR_AP_COVER_HOLE					"����©���澯"
#define PRIVATE_TRAP_DESCR_AP_COVER_HOLE_CLEAR				"����©���澯���"

#define PRIVATE_TRAP_DESCR_AP_FILE_TRANSFERED				"�ļ����͸澯"
#define PRIVATE_TRAP_DESCR_AP_LINK_VARIFIED_FAIL			"��·��֤ʧ�ܸ澯"
#define PRIVATE_TRAP_DESCR_AP_LINK_VARIFIED_FAIL_CLEAR		"��·��֤ʧ�ܸ澯���"

#define PRIVATE_TRAP_DESCR_AP_DOWN							"Ap�ػ��澯"
#define PRIVATE_TRAP_DESCR_AP_REBOOT						"AP�����澯"
#define PRIVATE_TRAP_DESCR_AP_COLD_REBOOT					"AP�������澯"
#define PRIVATE_TRAP_DESCR_AP_SHAREKEY_CHANGE				"Ԥ������Կ����ı�澯"
#define PRIVATE_TRAP_DESCR_AP_ELECTRIFY_REG_CIRCLE			"�ϵ�ע�����ڸ澯"
#define PRIVATE_TRAP_DESCR_AP_IP_CHANGE						"AP��IP��ַ�ı�澯"
#define PRIVATE_TRAP_DESCR_AP_AUTH_MODE_CHANGE				"AP����֤��ʽ�ı�澯"



#define PRIVATE_TRAP_DESCR_AP_INTERFERE_DETECT				"�ڽ�AP���Ÿ澯"
#define PRIVATE_TRAP_DESCR_AP_STA_INTERFERE_DETECT			"�ն˸��Ÿ澯"
#define PRIVATE_TRAP_DESCR_AP_DEVICE_INTERFERE_DETECT		"�����豸���Ÿ澯"
#define PRIVATE_TRAP_DESCR_AP_ATH_MOUDLE_ERROR				"����ģ����ϸ澯"
#define PRIVATE_TRAP_DESCR_AP_ATH_MOUDLE_ERROR_CLEAR		"����ģ����ϸ澯���"

#define PRIVATE_TRAP_DESCR_AP_LINKDOWN_ERROR				"������·�жϸ澯"
#define PRIVATE_TRAP_DESCR_AP_LINKDOWN_ERROR_CLEAR			"������·�жϸ澯���"

#define PRIVATE_TRAP_DESCR_AP_DENY_STA						"AP�޷������µ��ƶ��û��澯"
#define PRIVATE_TRAP_DESCR_AP_DENY_STA_CLEAR				"AP�޷������µ��ƶ��û��澯���"

#define PRIVATE_TRAP_DESCR_AP_CHANNEL_COUNT_MINOR			"�ɹ�ʹ�õ��ŵ������͸澯"
#define PRIVATE_TRAP_DESCR_AP_CHANNEL_COUNT_MINOR_CLEAR		"�ɹ�ʹ�õ��ŵ������͸澯���"

#define PRIVATE_TRAP_DESCR_AP_CHANNEL_CHANGE				"�����ŵ����ͨ��"



#define PRIVATE_TRAP_DESCR_AP_STATION_AUTH_FAIL				"��վ��Ȩʧ��ͨ��"
#define PRIVATE_TRAP_DESCR_AP_STATION_ASSOC_FAIL			"��վ����ʧ��ͨ��"



#define PRIVATE_TRAP_DESCR_AP_INVALID_CER_USR_ATTACK		"�Ƿ�֤���û���������ͨ��"
#define PRIVATE_TRAP_DESCR_AP_CLIENT_RE_ATTACK				"�ͻ����طŹ���ͨ��"
#define PRIVATE_TRAP_DESCR_AP_TAMPER_ATTACK					"�۸Ĺ���ͨ��"
#define PRIVATE_TRAP_DESCR_AP_LOW_SAFE_ATTACK				"��ȫ�ȼ����͹���ͨ��"
#define PRIVATE_TRAP_DESCR_AP_ADDR_REDIR_ATTACK				"��ַ�ض��򹥻�ͨ��"



#define PRIVATE_TRAP_DESCR_AC_CONFIG_ERROR					"AC���ô���澯"
#define PRIVATE_TRAP_DESCR_AC_DOWN							"AC��Դ����澯"
#define PRIVATE_TRAP_DESCR_AC_DOWN_CLEAR					"AC��Դ����澯�ָ�"

#define PRIVATE_TRAP_DESCR_AC_CPU_OVER_THRESHOLD			"AC CPU�����ʹ��߸澯"
#define PRIVATE_TRAP_DESCR_AC_CPU_OVER_THRESHOLD_CLEAR		"AC CPU�����ʹ��߸澯���"

#define PRIVATE_TRAP_DESCR_AC_MEM_OVER_THRESHOLD			"AC �ڴ������ʹ��߸澯"
#define PRIVATE_TRAP_DESCR_AC_MEM_OVER_THRESHOLD_CLEAR		"AC �ڴ������ʹ��߸澯���"

#define PRIVATE_TRAP_DESCR_AC_TURNTO_BACKUP					"AC���������л��澯"
#define PRIVATE_TRAP_DESCR_AC_DHCP_EXHAUST					"AC��DHCP�ɷ����ַ�ľ��澯"
#define PRIVATE_TRAP_DESCR_AC_DHCP_EXHAUST_CLEAR			"AC��DHCP�ɷ����ַ�ľ��澯���"

#define PRIVATE_TRAP_DESCR_AC_TIME_SYNC_FAIL				"AC��AP��ϵͳʱ��ͬ��ʧ��ͨ��"
#define PRIVATE_TRAP_DESCR_AC_TIME_SYNC_FAIL_CLEAR			"AC��AP��ϵͳʱ��ͬ��ʧ��ͨ�����"
#define PRIVATE_TRAP_DESCR_AC_COLD_REBOOT					"ACϵͳ������ͨ��"
#define PRIVATE_TRAP_DESCR_AC_REBOOT						"ACϵͳ������ͨ��"
#define PRIVATE_TRAP_DESCR_AC_IP_CHANGE						"AC IP��ַ���ͨ��"
#define PRIVATE_TRAP_DESCR_AC_HEART_TIME					"AC ��������ͨ��"



#define PRIVATE_TRAP_DESCR_AP_DISCOVER_DANGER_AP			"AP���ֿ����豸ͨ��"
#define PRIVATE_TRAP_DESCR_AP_FIND_UNSAFE_SSID				"AP����δ��ȨSSIDͨ��"
#define PRIVATE_TRAP_DESCR_AP_SYN_ATTACK					"AP���ֹ���ͨ��"



#define PRIVATE_TRAP_DESCR_AC_RADIUS_AUTH_ERROR				"Radius��֤���������ɴ�澯"
#define PRIVATE_TRAP_DESCR_AC_RADIUS_ACCOUNT_ERROR			"Radius�Ʒѷ��������ɴ�澯"
#define PRIVATE_TRAP_DESCR_AC_RADIUS_AUTH_ERROR_CLEAR		"Radius��֤���������ɴ�澯���"
#define PRIVATE_TRAP_DESCR_AC_RADIUS_ACCOUNT_ERROR_CLEAR	"Radius�Ʒѷ��������ɴ�澯���"

#define PRIVATE_TRAP_DESCR_AC_PORTAL_ERROR					"Portal���������ɴ�澯"
#define PRIVATE_TRAP_DESCR_AC_PORTAL_ERROR_CLEAR			"Portal���������ɴ�澯���"


#define PRIVATE_TRAP_DESCR_AC_BANDWITH_TOO_HIGH				"AC���������ʳ�����ֵ�澯"
#define PRIVATE_TRAP_DESCR_AC_LOSTPKG_TOO_HIGH				"AC�����ʳ�����ֵ�澯"
#define PRIVATE_TRAP_DESCR_AC_MAX_ONLINEUSR_TOO_HIGH		"AC��������û������߸澯"
#define PRIVATE_TRAP_DESCR_AC_RADIUS_AUTH_TOO_LOW			"AC RADIUS��֤�ɹ��ʹ��͸澯"
#define PRIVATE_TRAP_DESCR_AC_IPPOOL_AVE_RATE_TOO_HIGH		"AC IP��ַ��ƽ��ʹ���ʹ��߸澯"
#define PRIVATE_TRAP_DESCR_AC_IPPOOL_MAX_RATE_TOO_HIGH		"AC IP��ַ�����ʹ���ʹ��߸澯"
#define PRIVATE_TRAP_DESCR_AC_FIND_SYN_ATTACK				"AC SYN�����澯"


#define PRIVATE_TRAP_DESCR_AC_TEMPERATUE_TOO_HIGH			"AC�¶ȳ�����ֵ�澯"
#define PRIVATE_TRAP_DESCR_AC_TEMPERATUE_TOO_HIGH_CLEAR		"AC�¶ȳ�����ֵ�澯���"

#define PRIVATE_TRAP_DESCR_AP_LOST_NET						"AP��������澯"
#define PRIVATE_TRAP_DESCR_STA_OFFLINE						"STATION���߸澯"	
#define PRIVATE_TRAP_DESCR_STA_ONLINE						"STATION���߸澯"
#define PRIVATE_TRAP_DESCR_DEVICE_INTF_CLEAR				"�豸���Ÿ澯���"	
#define PRIVATE_TRAP_DESCR_AP_INTF_CLEAR					"AP���Ÿ澯���"
#define PRIVATE_TRAP_DESCR_STA_INTF_CLEAR					"station���Ÿ澯���"
#define PRIVATE_TRAP_DESCR_COVER_HOLE_CLEAR					"����©���澯���"
#define PRIVATE_TRAP_DESCR_AP_TEMP_TOO_HIGH					"AP�¶ȹ��߸澯"
#define PRIVATE_TRAP_DESCR_AP_TEMP_TOO_HIGH_CLEAR			"AP�¶ȹ��߸澯���"

#define PRIVATE_TRAP_DESCR_AP_ONLINE						"AP���߸澯"
#define PRIVATE_TRAP_DESCR_AP_OFFLINE						"AP���߸澯"
#define PRIVATE_TRAP_DESCR_AP_WIDS_ATTACK					"AP����WIDS�����澯"

#define PRIVATE_TRAP_DESCR_AC_POWER_EXCHANGE				"AC������Դ�л��澯"

/**************************END*********************************/

typedef struct {
	char trapName[128];
	char trapDes[128];
	char trapEDes[128];
	unsigned char  trapSwitch;
	unsigned char  trap_level;
}TRAP_DETAIL_CONFIG;


#if 0
#define TRAP_CONFIG_PATH	"/opt/www/htdocs/trap/all_trap_conf.xml"
#else
#define TRAP_CONFIG_PATH	"/opt/services/conf/trapconf_conf.conf"/*�����ļ����Զ�����*/
#define TRAP_STATUS_PATH	"/opt/services/status/trapconf_status.status"/*for srvload!*/

#endif

#define XML_ROOT_NODE_NAME	"trap_conf"

#define XML_CHAR			(const char *)

#define TRAP_RTN_OK		0
#define TRAP_RTN_ERR	-1


#define create_trap_xml()\
{\
	int ret_create=_general_trap_xml(TRAP_CONFIG_PATH);\
	if (ret_create < 0)\
	{\
		return TRAP_RTN_ERR;\
	}\
}

int init_trap_config();

TRAP_DETAIL_CONFIG * load_trap_config(int index);
int get_trap_config_num();
char *get_trapName_byindex(int num);
char *get_trapDes_byindex(int num);
char *get_trapDes_byindex(int num);
int get_trapSwitch_byindex(int num);
int mod_trap_conf(TRAP_DETAIL_CONFIG *conf, int index);
int write_conf_into_xml();
int _general_trap_xml(char *file_path);





#endif
