1. write 
    dal_[project_name]_[module_name].h  
    ex: dal_apollo_vlan.h
    ��header file �����N���Y�g���T
    
2. copy dal_[project_name]_[module_name].h to gen_file/

3. excute
 ./drive_gen.py -fd gen_file/ -f xxx.h

4.�N�|���ͥH�Uoutput file �� gen_file/
  dal.c
  rtk.c
  rtk.h
  mapper.c
  mapper.h
  
  �٦���Lfile (netfilter_get.c ...)�ثe�S�Ψ�����z�|
  
  ���}�H�Wfiles(dal.c rtk.c ...) �N�|���۹�����SOP
  
  
