#pragma once

#ifndef __CONFIG_PROTOCOL__
#define __CONFIG_PROTOCOL__


// ��Ŷ �ڵ� 
enum Codes { 

	/*
		���� �⺻ ��������
	*/
	USER_PTR_UPDATE=500,// Ŭ���̾�Ʈ �������� ��û	
	CLOSE_CLIENT,		// Ŭ���̾�Ʈ�� ���� ����
	


	/*
		���� ��������
	*/
	CHK_AUTH=600,		// ȸ������
	CHK_PACKET_HAVE,	// �ܿ� ��Ŷ ��ȸ
	CHK_DUPLI_CLOSE,	// �ٸ� ��ǻ�� �α���
	UPLOAD_SIZE,		//20080122 ������ ����� ���� ���ÿ� ����� �ִ� ������ ������Ʈ, jyh

	/*
		�ٿ�ε� ���� ��������
	*/		

	LOAD_DOWNLIST=800,	// �ٿ�ε� ��� �߰� ��û	
	LOAD_DOWNPRE,		// �ٿ�ε� ���� ó������
	LOAD_DOWNLOAD,		// ���� �ٿ�ε�
	LOAD_DOWNEND,		// �ٿ�ε� ����/�Ϸ� �뺸	
	LOAD_DOWNSTATE,		// �ٿ�ε� �����Ȳ	
	DOWN_SAVE_MUREKA_DATA,


	/*
		���ε� ��Ʈ�� ���� ���� ��������
	*/
	LOAD_UPLIST=850,	// ���ε� ��� �߰� ��û
	LOAD_UPPRE,			// ���ε� ���� ó������
	LOAD_UPLOAD,		// ���� ���ε� 
	LOAD_UPEND,			// ���ε� ����/�Ϸ� �뺸
	LOAD_UPSTATE,		// ���ε�� ������(������)
	LOAD_UPHASH,		//20071210 ���ε��� �ؽð� �񱳿�û, jyh
	LOAD_UPGURAEND,		//20071211 ���� ���� �Ϸ�, jyh
	LOAD_UPCONTENTSCOMPLETE,	//20090303 ������ ���ε� �Ϸ�, jyh
	SAVE_MUREKA_DATA,			//20100914 �·�ī

	//20080506 ���� �ڷ� ���� ���� ��������
	OPEN_UPLIST=900,	//���ε� ��� �߰� ��û
	OPEN_UPLOAD,		//���� ���ε�
	OPEN_UPEND,			//���ε� ����/�Ϸ�
	OPEN_DOWNLIST,		//�ٿ�ε� ��� �߰� ��û
	OPEN_DOWNLOAD,		//���� �ٿ�ε�
	OPEN_DOWNEND		//�ٿ�ε� ����/�Ϸ�
};

#endif