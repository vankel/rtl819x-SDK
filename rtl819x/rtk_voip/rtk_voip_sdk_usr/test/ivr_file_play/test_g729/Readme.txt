�ɮ�: 
	729_raw: ���� G.729 ���n����
	Makefile: �Ψ� make test_g729.c --> test_g729.o, 
	          �ϥήɭn�� -I../../../include/ ���V VoIP-ATA/linux-2.4.18/rtk_voip/include
	test_g729.c: ���յ{��
	test_g729.o: ���յ{��������

���յ{�������G
���յ{������ rtk_SetIvrPlayG729() ���P�� 
VoIP-ATA/AP/rtk_voip/voip_manager/voip_manager.c ���� rtk_IvrStartPlayG729()
�ѼơG
	nCount: �ǳƼg�J�� frame �Ӽ�, (�C��frame  10 bytes )
	pData: data ����m

�^�ǭȡG
	�u��g�J�� frame �Ӽ� 
	�Y�^�ǭȤ����� nCount, �������@�q�ɶ� (��ĳ1��), �A�|�ձN���g�J�������g�J�C
	�ثe kernel buffer ���׬��� 2.56 ��. 

