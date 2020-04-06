#ifndef _SFILTER_SITECOMMON_
#define _SFILTER_SITECOMMON_


typedef struct _tagSFILTER_RET_FILEFORMAT
{
	int format_gubun;	// 0:�Ϲ�����, 1:��������, 2:����, 3:������ 4:�̹��� 5:pdf

	char format[30];
	char video_format[30];
	char video_codec[30];
	char audio_format[30];
	char audio_codec[30];

} SFILTER_RET_FILEFORMAT, *LPSFILTER_RET_FILEFORMAT;

typedef struct _tagSFILTER_RET
{
	int video_count;
	int music_count;
	int total_count;

	char sfilter_hash[54];
} SFILTER_RET, *LPSFILTER_RET;


typedef struct _tagSFILTER_RET_V2
{
	int video_count;
	int music_count;
	int cartoon_count;
	int total_count;

	char sfilter_hash[54];
} SFILTER_RET_V2, *LPSFILTER_RET_V2;




typedef struct _tagSFILTER_RET_VIDEO
{
	int rmi_return_code;		// 10:BLOCK, 11:FREE, 12:PAID, 13:UNREGISTED, 14:FAIL
	int rmi_mreturn_code;		// 10:BLOCK, 11:FREE, 12:PAID, 13:UNREGISTED, 14:FAIL (Mobile)	// 20150908

	char content_id[20];	
	char title[300];
	char seq[10];				// ȸ��
	char price[13];				// �ݾ�
	char mprice[13];			// Mobile ����	(Mobile)		// 20150908

	char site_ratio[4];			// ����Ʈ ���з�
	char ch_id[20];				// ������Ȧ�� ID
	char ch_name[100];			// ������Ȧ�� �̸�
	char ch_content_id[30];		// ������Ȧ�� content_id
	char kfpa_content_id[30];	// ������ content_id (������ �������� ���)
	char kfpa_log_seq[30];		// ������ �α� ������ ��ȣ (������ �������� ���)
	
	char release_date[9];		// ������, �濵��
	char category[50];			// ī�װ� ����
	char genre[20];				// �帣 
	char rating[3];				// �������  0:���� 1:��ü 12:12���̻� 15:15���̻� 18:18���̻� 99:������
	char country[30];			// ����

	char filtering_key[54];
	char smartblock_key[54];	// 1M HASH (Mobile)		// 20150908

	char offset01[0xFF];
	char offset02[0xFF];
	char offset03[0xFF];
} SFILTER_RET_VIDEO, *LPSFILTER_RET_VIDEO;

typedef struct _tagSFILTER_RET_MUSIC
{
	int rmi_return_code;	// 10:BLOCK, 11:FREE, 12:PAID, 13:UNREGISTED, 14:FAIL

	char content_id[20];	
	char artist[300];
	char album[300];
	char title[300];
	char price[13];				// �ݾ�
	char site_ratio[4];	
	char ch_id[20];				// ������Ȧ�� ID
	char ch_name[100];			// ������Ȧ�� �̸�
	char ch_content_id[30];		// ������Ȧ�� content_id
	char filtering_key[54];

} SFILTER_RET_MUSIC, *LPSFILTER_RET_MUSIC;



typedef struct _tagSFILTER_RET_CARTOON
{
	int rmi_return_code;	// 10:BLOCK, 11:FREE, 12:PAID, 13:UNREGISTED, 14:FAIL, 16: ��ȭ����
	char cartoon_title[500];   //��ȭ����
	char right_name[200];			// �Ǹ��� �̸�
	char filtering_key[54];

} SFILTER_RET_CARTOON, *LPSFILTER_RET_CARTOON;


struct sMemory{
	char ospid[16];
	char hash[32];
	char fil_key[128];

	sMemory()
	{
		memset(ospid,0,sizeof(ospid));
		memset(hash,0,sizeof(hash));
		memset(fil_key,0,sizeof(fil_key));
	}
};

typedef struct _tagSFILTER_SMARTBLOCK_HASH
{
	char sfilter_hash[54];	
	char smartblock_key[54];

	bool block;
}SMARTBLOCK_HASH;

typedef struct sMemory	SMEMORY;

#define	VIRUS_FINE_CODE			-5

#endif // _SFILTER_SITECOMMON_