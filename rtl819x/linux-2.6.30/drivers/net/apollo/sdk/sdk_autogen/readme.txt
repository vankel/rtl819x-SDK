1. write 
    dal_[project_name]_[module_name].h  
    ex: dal_apollo_vlan.h
    此header file 必須將檔頭寫正確
    
2. copy dal_[project_name]_[module_name].h to gen_file/

3. excute
 ./drive_gen.py -fd gen_file/ -f xxx.h

4.將會產生以下output file 於 gen_file/
  dal.c
  rtk.c
  rtk.h
  mapper.c
  mapper.h
  
  還有其他file (netfilter_get.c ...)目前沒用到先不理會
  
  打開以上files(dal.c rtk.c ...) 將會有相對應的SOP
  
  
