#ifndef _SFILTER_SITECOMMON_
#define _SFILTER_SITECOMMON_


typedef struct _tagSFILTER_RET_FILEFORMAT
{
	int format_gubun;	// 0:일반파일, 1:압축파일, 2:음악, 3:동영상 4:이미지 5:pdf

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
	char seq[10];				// 회차
	char price[13];				// 금액
	char mprice[13];			// Mobile 가격	(Mobile)		// 20150908

	char site_ratio[4];			// 사이트 지분률
	char ch_id[20];				// 콘텐츠홀더 ID
	char ch_name[100];			// 콘텐츠홀더 이름
	char ch_content_id[30];		// 콘텐츠홀더 content_id
	char kfpa_content_id[30];	// 영제협 content_id (영제협 콘텐츠일 경우)
	char kfpa_log_seq[30];		// 영제협 로그 시퀀스 번호 (영제협 콘텐츠일 경우)
	
	char release_date[9];		// 개봉일, 방영일
	char category[50];			// 카테고리 구분
	char genre[20];				// 장르 
	char rating[3];				// 관람등급  0:미정 1:전체 12:12세이상 15:15세이상 18:18세이상 99:음란물
	char country[30];			// 국가

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
	char price[13];				// 금액
	char site_ratio[4];	
	char ch_id[20];				// 콘텐츠홀더 ID
	char ch_name[100];			// 콘텐츠홀더 이름
	char ch_content_id[30];		// 콘텐츠홀더 content_id
	char filtering_key[54];

} SFILTER_RET_MUSIC, *LPSFILTER_RET_MUSIC;



typedef struct _tagSFILTER_RET_CARTOON
{
	int rmi_return_code;	// 10:BLOCK, 11:FREE, 12:PAID, 13:UNREGISTED, 14:FAIL, 16: 만화차단
	char cartoon_title[500];   //만화제목
	char right_name[200];			// 권리자 이름
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