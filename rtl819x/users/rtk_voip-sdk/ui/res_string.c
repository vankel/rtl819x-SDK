#include "ui_config.h"
#include "res.h"

#ifdef LANG_BIG5	/* Traditional Chinese (BIG5) */
#define DECLARE_RES_STRING( name, szEng, szBig5, szGB2312 )		\
	const unsigned char name[] = { szBig5 };	\
	const unsigned short name ## _____len = sizeof( name ) - 1
#elif defined( LANG_GB2312 )	/* Simplified Chinese (GB2312) */
#define DECLARE_RES_STRING( name, szEng, szBig5, szGB2312 )		\
	const unsigned char name[] = { szGB2312 };	\
	const unsigned short name ## _____len = sizeof( name ) - 1
#else				/* English */
#define DECLARE_RES_STRING( name, szEng, szBig5, szGB2312 )		\
	const unsigned char name[] = { szEng };	\
	const unsigned short name ## _____len = sizeof( name ) - 1
#endif

DECLARE_RES_STRING( szOK,		"OK",		"����",		"���" );
DECLARE_RES_STRING( szFail,		"Fail",		"����",		"ʧ��" );
DECLARE_RES_STRING( szPhonebookHasNoRecord,	"No record!\nAdd one?",		"�L�O���I\n�O�_�s�W�H",		"�޼�¼��\n�Ƿ�������" );
DECLARE_RES_STRING( szPhonebookIsFull,		"Phonebook is full!",		"�q��ï�w���I",				"�绰��������" );
DECLARE_RES_STRING( szEnterName,		"Name:",		"�p���H:",		"������:" );
DECLARE_RES_STRING( szEnterNumber,		"Number:",		"�q�ܸ��X:",	"�绰����:" );
DECLARE_RES_STRING( szAdd,				"Add",			"�s�W",			"����" );
DECLARE_RES_STRING( szModify,			"Modify",		"�ק�",			"�޸�" );
DECLARE_RES_STRING( szDelete,			"Delete",		"�R��",			"ɾ��" );
DECLARE_RES_STRING( szDeleteAll,		"Delete All",	"�R������",		"ɾ��ȫ��" );
DECLARE_RES_STRING( szStatus,			"Status",	"���A",			"״̬" );
DECLARE_RES_STRING( szUsedFormat,		"Used: %d",	"�ϥΪŶ�:%d",	"ʹ�ÿռ�:%d" );
DECLARE_RES_STRING( szFreeFormat,		"Free: %d",	"�Ѿl�Ŷ�:%d",	"ʣ��ռ�:%d" );
DECLARE_RES_STRING( szStandbyPrompt,	"IP Phone",	"IP�q��",		"IP�绰" );
DECLARE_RES_STRING( szQSure,			"Sure?",	"�нT�{�H",		"��ȷ�ϣ�" );
DECLARE_RES_STRING( szEmptyIsNotAllow,			"Error!\nIt is empty!",		"���~�I���i�ťաI",		"���󣡲��ɿհף�" );
DECLARE_RES_STRING( szDialNumberPrompt,			"Dial Number:",		"����:",		"����:" );
DECLARE_RES_STRING( szDoingOutgoingCall,		"Outgoing call",	"����",			"����" );
DECLARE_RES_STRING( szDoingFxoConnecting,		"Connect FXO",		"FXO�s�u",		"FXO����" );
DECLARE_RES_STRING( szInConnection,				"In Connection",	"�q�ܤ�",		"ͨ����" );
DECLARE_RES_STRING( szDisconnection,			"Disconnection",	"�q�ܵ���",		"ͨ������" );
DECLARE_RES_STRING( szIncomingCall,				"Incoming call",	"�ӹq",			"����" );
DECLARE_RES_STRING( szDisconnecting,			"Disconnect...",	"�����q�ܤ�...",	"����ͨ����..." );
DECLARE_RES_STRING( szError,					"Error!",		"���~�I",		"����" );
DECLARE_RES_STRING( szColonLAN,					"LAN:",			"LAN:",			"LAN:" );
DECLARE_RES_STRING( szColonWAN,					"WAN:",			"WAN:",			"WAN:" );
DECLARE_RES_STRING( szZerosIP,					"0.0.0.0",		"0.0.0.0",		"0.0.0.0" );
DECLARE_RES_STRING( szVolume,					"Volume",		"���q",			"����" );
DECLARE_RES_STRING( szVolumeReceiver,			"MonoVolume",	"�歵���q",		"��������" );
DECLARE_RES_STRING( szVolumeSpeaker,			"Spk Volume",	"��z���q",		"��������" );
DECLARE_RES_STRING( szVolumeMicR,				"Mic(R) Vol.",	"Mic(R)���q",	"Mic(R)����" );
DECLARE_RES_STRING( szVolumeMicS,				"Mic(S) Vol.",	"Mic(S)���q",	"Mic(S)����" );
DECLARE_RES_STRING( szVolumeWithDigitsFormat,	"Volume:%u",	"���q:%u",		"����:%u" );
DECLARE_RES_STRING( szVolumeReceiverWithDigitsFormat,	"Mono Volume:%u",	"�歵���q:%u",		"��������:%u" );
DECLARE_RES_STRING( szVolumeSpeakerWithDigitsFormat,	"Spk Volume:%u",	"��z���q:%u",		"��������:%u" );
DECLARE_RES_STRING( szVolumeMicRWithDigitsFormat,		"Mic(R) Volume:%u",	"Mic(R)���q:%u",	"Mic(R)����:%u" );
DECLARE_RES_STRING( szVolumeMicSWithDigitsFormat,		"Mic(S) Volume:%u",	"Mic(S)���q:%u",	"Mic(S)����:%u" );
DECLARE_RES_STRING( szInputIpPrompt,			"Input IP:",		"��JIP:",		"����IP:" );
DECLARE_RES_STRING( szPing,						"Ping",				"Ping",			"Ping" );
DECLARE_RES_STRING( szGatewayPrompt,			"Gateway:",			"�w�]�h�D:",	"Ԥ������:" );
DECLARE_RES_STRING( szDnsPrompt,				"DNS:",				"DNS:",			"DNS:" );
DECLARE_RES_STRING( szCallWaiting,				"Call waiting...",	"�ӹq����...",	"����ȴ�..." );
DECLARE_RES_STRING( szCallPrompt,				"Call:",			"�ӹq:",		"����:" );
DECLARE_RES_STRING( szCallHoldStatus,			"[Hold]",			"[�w�O�d]",		"[�ѱ���]" );
DECLARE_RES_STRING( szCallHoldingStatus,		"[Hold..]",			"[�O�d��]",		"[������]" );
DECLARE_RES_STRING( szCallHeldStatus,			"[Held]",			"[�Q�O�d]",		"[������]" );
DECLARE_RES_STRING( szCallConferenceStatus,		"[Conf.]",			"[�|ĳ]",		"[����]" );
DECLARE_RES_STRING( szCallDisconnectedStatus,	"[Disc.]",			"[����]",		"[����]" );
DECLARE_RES_STRING( szNoname,					"(No name)",		"���R�W",		"δ����" );
DECLARE_RES_STRING( szNoRecord,					"No record!",		"�L���",		"������" );
DECLARE_RES_STRING( szHotLine,					"Hot Line",			"���u",			"����" );
DECLARE_RES_STRING( szHotLinePrompt,			"Hot Line:",		"���u:",		"����:" );
DECLARE_RES_STRING( szAutoDial,					"Auto Dial",		"�۰ʼ����]�w",		"�Զ���������" );
DECLARE_RES_STRING( szInputAutoDialPrompt,		"Auto Dial Time:",	"�۰ʼ����ɶ�:",	"�Զ�����ʱ��:" );
DECLARE_RES_STRING( szInputAutoDialRange,		"(3-9)",			"(3-9)",			"(3-9)" );
DECLARE_RES_STRING( szAutoAnswer,				"Auto Answer",		"�۰ʱ�ť�]�w",		"�Զ������趨" );
DECLARE_RES_STRING( szInputAutoAnswerPrompt,	"Auto Ans Time:",	"�۰ʱ�ť�ɶ�:",	"�Զ�����ʱ��:" );
DECLARE_RES_STRING( szInputAutoAnswerRange,		"(3-9)",			"(3-9)",			"(3-9)" );
DECLARE_RES_STRING( szInvalidValue,				"Invalid Value!!",	"��J�ȿ��~�I",		"����ֵ����" );
DECLARE_RES_STRING( szOffHookAlarm,				"Off-hook Alarm",	"����ĵ�i�]�w",		"�����������" );
DECLARE_RES_STRING( szInputOffHookAlarmPrompt,	"Off-hook Alarm:",	"����ĵ�i�ɶ�:",	"�������ʱ��:" );
DECLARE_RES_STRING( szInputOffHookAlarmRange,	"(10-60)",			"(10-60)",			"(10-60)" );
DECLARE_RES_STRING( szSipRegister,				"SIP registered  ",	"SIP�w���U",		"SIP��ע��" );
DECLARE_RES_STRING( szSipNotRegister,			"SIP not register",	"SIP�����U",		"SIPδע��" );
DECLARE_RES_STRING( szRebooting,				"Reboot...",		"���s�}����...",	"���¿�����..." );
DECLARE_RES_STRING( szNull,				"",				"",					"" );
DECLARE_RES_STRING( szGraphicOnly,		"Graphic Only",	"Graphic Only",		"Graphic Only" );
/* Instruction Text */
DECLARE_RES_STRING( szInsPhonebook,		"Phonebook",	"�q��ï",	"�绰��" );
DECLARE_RES_STRING( szInsMenu,			"Menu",			"���",	"�˵�" );
DECLARE_RES_STRING( szInsCancel,		"Cancel",		"����",	"ȡ��" );
DECLARE_RES_STRING( szInsOK,			"OK",			"���",	"ѡ��" );
DECLARE_RES_STRING( szInsBack,			"Back",			"�h�X",	"�˳�" );
DECLARE_RES_STRING( szInsClear,			"Clear",		"�M��",	"���" );
DECLARE_RES_STRING( szInsYes,			"Yes",			"�O",	"��" );
DECLARE_RES_STRING( szInsNo,			"No",			"�_",	"��" );
DECLARE_RES_STRING( szInsDetail,		"Detail",		"�Ա�",	"����" );
DECLARE_RES_STRING( szInsDial,			"Dial",			"����",	"����" );
DECLARE_RES_STRING( szInsReject,		"Reject",		"�ڵ�",	"�ܾ�" );
DECLARE_RES_STRING( szInsTransfer,		"Xfer",			"�౵",	"ת��" );
DECLARE_RES_STRING( szInsHold,			"Hold",			"�O�d",	"����" );
DECLARE_RES_STRING( szInsConference,	"Conf",			"�|ĳ",	"����" );
DECLARE_RES_STRING( szInsPick,			"Pick",			"��ť",	"����" );
DECLARE_RES_STRING( szInsAnswer,		"Ans",			"��ť",	"����" );
/* Menu Item Text */
DECLARE_RES_STRING( szItemView,				"View",				"�˵�",		"�鿴" );
DECLARE_RES_STRING( szItemConfiguration,	"Configuration",	"�]�w",		"����" );
DECLARE_RES_STRING( szItemPhonebook,		"Phonebook",		"�q��ï",	"�绰��" );
DECLARE_RES_STRING( szItemTestCase,			"Test Case",		"�}�o����",	"��������" );
DECLARE_RES_STRING( szItemNetworkSettings,	"Network Settings",	"�����]�w",	"��������" );
DECLARE_RES_STRING( szItemPing,				"Ping",				"Ping",		"Ping" );
DECLARE_RES_STRING( szItemSoftwareVersion,	"Software Version",	"�n�骩��",	"����汾" );
DECLARE_RES_STRING( szItemCallRecords,		"Call Records",		"�q�ܰO��",	"ͨ����¼" );
DECLARE_RES_STRING( szItemIPAddress,		"IP Address",		"IP��}",	"IP��ַ" );
DECLARE_RES_STRING( szItemMask,				"Mask",				"�l�B�n",	"������" );
DECLARE_RES_STRING( szItemGateway,			"Gateway",			"�w�]�h�D",	"Ԥ������" );
DECLARE_RES_STRING( szItemDNS,				"DNS",				"DNS",		"DNS" );
DECLARE_RES_STRING( szItemKeypressTone,		"KeypressTone",		"���䭵",	"������" );
DECLARE_RES_STRING( szItemMissedCallRecords,		"Missed Call Records",		"�����ӹq",	"δ������" );
DECLARE_RES_STRING( szItemIncomingCallRecords,		"Incoming Call Records",	"�w���ӹq",	"�ѽ�����" );
DECLARE_RES_STRING( szItemOutgoingCallRecords,		"Outgoing Call Records",	"�w���q��",	"�Ѳ��绰" );
/* Month Text */
DECLARE_RES_STRING( szMonthJan,				"Jan",				"�@��",		"һ��" );
DECLARE_RES_STRING( szMonthFeb,				"Feb",				"�G��",		"����" );
DECLARE_RES_STRING( szMonthMar,				"Mar",				"�T��",		"����" );
DECLARE_RES_STRING( szMonthApr,				"Apr",				"�|��",		"����" );
DECLARE_RES_STRING( szMonthMay,				"May",				"����",		"����" );
DECLARE_RES_STRING( szMonthJun,				"Jun",				"����",		"����" );
DECLARE_RES_STRING( szMonthJul,				"Jul",				"�C��",		"����" );
DECLARE_RES_STRING( szMonthAug,				"Aug",				"�K��",		"����" );
DECLARE_RES_STRING( szMonthSep,				"Sep",				"�E��",		"����" );
DECLARE_RES_STRING( szMonthOct,				"Oct",				"�Q��",		"ʮ��" );
DECLARE_RES_STRING( szMonthNov,				"Nov",				"11��",		"11��" );
DECLARE_RES_STRING( szMonthDec,				"Dec",				"12��",		"12��" );
