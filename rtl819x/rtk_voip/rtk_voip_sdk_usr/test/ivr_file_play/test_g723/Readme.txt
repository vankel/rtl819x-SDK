�ɮ�: 
	723_raw: ���� G.723 6.3k ���n����
	Makefile: �Ψ� make test_g723.c --> test_g723.o, 
	          �ϥήɭn�� -I../../../include/ ���V VoIP-ATA/linux-2.4.18/rtk_voip/include
	test_g723.c: ���յ{��
	test_g723.o: ���յ{��������

���յ{�������G
���յ{������ rtk_SetIvrPlayG72363() ���P�� 
VoIP-ATA/AP/rtk_voip/voip_manager/voip_manager.c ���� rtk_IvrStartPlayG72363()
�ѼơG
	nCount: �ǳƼg�J�� frame �Ӽ�, (�C��frame  24 bytes )
	pData: data ����m

�^�ǭȡG
	�u��g�J�� frame �Ӽ� 
	�Y�^�ǭȤ����� nCount, �������@�q�ɶ� (��ĳ1��), �A�|�ձN���g�J�������g�J�C
	�ثe kernel buffer ���׬��� 3.84 ��. 

