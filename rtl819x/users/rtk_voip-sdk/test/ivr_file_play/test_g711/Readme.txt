�ɮ�: 
	711_raw: ���� G.711a ���n����
	Makefile: �Ψ� make test_g711.c --> test_g711.o, 
	          �ϥήɭn�� -I../../../include/ ���V VoIP-ATA/linux-2.4.18/rtk_voip/include
	test_g711.c: ���յ{��
	test_g711.o: ���յ{��������

���յ{�������G
���յ{������ rtk_SetIvrPlayG711() ���P�� 
VoIP-ATA/AP/rtk_voip/voip_manager/voip_manager.c ���� rtk_IvrStartPlayG711()
�ѼơG
	nCount: �ǳƼg�J�� frame �Ӽ�, (�C��frame  10 bytes )
	pData: data ����m

�^�ǭȡG
	�u��g�J�� frame �Ӽ� 
	�Y�^�ǭȤ����� nCount, �������@�q�ɶ� (��ĳ1��), �A�|�ձN���g�J�������g�J�C
	�ثe kernel buffer ���׬��� 2.56 ��. 

