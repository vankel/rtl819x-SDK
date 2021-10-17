#!/usr/bin/env python

import re, sys, os

SRC_TITLE='dal_apollo'
DST_TITLE='rtk'
RTU_TITLE='RTDRV'

rtkc_func_body1 = '{\n' + '    int32   ret;'
rtkc_func_body2 = '\n' + '    RTK_API_LOCK();'
rtkc_func_body3 = '\n' + '    ret = RT_MAPPER->%s('
rtkc_func_body4 = '\n' + '    RTK_API_UNLOCK();'
rtkc_func_body5 = '\n' + '    return ret;\n'



rtku_func_body1 = '{\n' + '    rtdrv_xxx_t xxx_cfg;\n\n' + '    xxx_cfg.unit = unit;\n'
rtku_get_func_body = '    GETSOCKOPT(%s, &xxx_cfg, rtdrv_xxx_t, 1);\n\n'
rtku_set_func_body = '    SETSOCKOPT(%s, &xxx_cfg, rtdrv_xxx_t, 1);\n\n'
rtku_func_body2 =  '    return RT_ERR_OK;\n'
dal_func_body1 = '{\n' + '#if 0\n' + '    int32   ret;\n' + '#endif\n' + '    RT_DBG(LOG_DEBUG, (MOD_DAL|MOD_XXX),"%s",__FUNCTION__);\n\n' + '    /* check Init status */\n' + '    RT_INIT_CHK(xxx_init);\n\n'
dal_func_body2 = '    return RT_ERR_OK;\n'+ '}'
netfilterc_body1 = 'case %s:\n    copy_from_user(&buf.xxx_cfg, user, sizeof(rtdrv_xxxCfg_t));\n'
netfilterc_set_body1 = '    ret = %s(buf.xxx_cfg.unit, buf.xxx_cfg.yyy, buf.xxx_cfg.zzz);\n    break;\n\n'
netfilterc_get_body1 = '    ret = %s(buf.xxx_cfg.unit, buf.xxx_cfg.yyy, &buf.xxx_cfg.zzz);\n'
netfilterc_get_body2 = '    copy_to_user(user, &buf.xxx_cfg, sizeof(rtdrv_xxxCfg_t));\n    break;\n\n'

#for programmer note
common_note_start_str  = '/*-----------------<<Programmer Note>>---------------------------------\n'
common_note_cstart_str = ' Please check this note before coding-- Programmer "Must" remove this Note\n\n'
common_note_end_str    = '-------------------<<End of Note>>-------------------------------*/\n'

def process_input_file(filedir, filename):
    dict = {}
    para = []
    proto_type = []
    line = ''
    root_fname = filedir + '/' + filename

    try:
        fin = open(root_fname, 'r')
    except:
        print 'Fail to open %s.' % root_fname
        sys.exit(-1)

    # Generate dal.c, rtk.c and rtk.h file
    dalc_fname = filedir + '/' + "dal.c"
    rtkc_fname = filedir + '/' + "rtk.c"
    rtkh_fname = filedir + '/' + "rtk.h"
    export_fname = filedir + '/' + "export.h"
    rtku_fname = filedir + '/' + "rtku.c"
    netfilterset_fname = filedir + '/' + "netfilter_set.h"
    netfilterget_fname = filedir + '/' + "netfilter_get.h"
    netfiltersetc_fname = filedir + '/' + "netfilter_set.c"
    netfiltergetc_fname = filedir + '/' + "netfilter_get.c"
    mapperc_fname = filedir + '/' + "mapper.c"
    mapperh_fname = filedir + '/' + "mapper.h"

    try:
        dalc_fout = open(dalc_fname, 'w+')
    except:
        print 'Fail to open %s.' % dalc_fname
        sys.exit(-1)

    try:
        rtkc_fout = open(rtkc_fname, 'w+')
    except:
        print 'Fail to open %s.' % rtkc_fname
        sys.exit(-1)

    try:
        rtkh_fout = open(rtkh_fname, 'w+')
    except:
        print 'Fail to open %s.' % rtkh_fname
        sys.exit(-1)

    try:
        export_fout = open(export_fname, 'w+')
    except:
        print 'Fail to open %s.' % export_fname
        sys.exit(-1)

    try:
        rtku_fout = open(rtku_fname, 'w+')
    except:
        print 'Fail to open %s.' % rtku_fname
        sys.exit(-1)

    try:
        netfilter_get_fout = open(netfilterget_fname, 'w+')
    except:
        print 'Fail to open %s.' % netfilterget_fname
        sys.exit(-1)

    try:
        netfilter_set_fout = open(netfilterset_fname, 'w+')
    except:
        print 'Fail to open %s.' % netfilterset_fname
        sys.exit(-1)

    try:
        netfilterc_get_fout = open(netfiltergetc_fname, 'w+')
    except:
        print 'Fail to open %s.' % netfiltergetc_fname
        sys.exit(-1)

    try:
        netfilterc_set_fout = open(netfiltersetc_fname, 'w+')
    except:
        print 'Fail to open %s.' % netfiltersetc_fname
        sys.exit(-1)

    try:
        mapperc_fout = open(mapperc_fname, 'w+')
    except:
        print 'Fail to open %s.' % mapperc_fname
        sys.exit(-1)

    try:
        mapperh_fout = open(mapperh_fname, 'w+')
    except:
        print 'Fail to open %s.' % mapperh_fname
        sys.exit(-1)

    line_num        = 0
    comment_flag    = 0
    extern_flag     = 0
    extern_line     = 0
    brace_flag      = 0
    brace_dal_flag  = 0
    header_str      = ''
    c_skip          = 0

    export_fout.write('path:sdk/system/linux/rtk/module.c\n')

	#add common note
	#for dal.c file
    dalc_fout.write(common_note_start_str)
    dalc_fout.write(common_note_cstart_str)
    dalc_fout.write('	1. declear [your_module]_init \n' + '		ex:\n          static uint32    l2_init = {INIT_NOT_COMPLETED}; \n')
    dalc_fout.write('	2. replace all xxx_init to [your_module]_init \n')
    dalc_fout.write('	3. replace all MOD_XXX to MOD_[your_module] \n')
    dalc_fout.write('	4. chage all RTK_RANGE_DEFINED_XXX to correct defination \n')
    dalc_fout.write('	5. implement function body \n')
    dalc_fout.write('	6. rename dal.c to dal_[project_name]_[your_module].c \n')
    dalc_fout.write('	7. copy this file to sdk/src/dal/[project_name]/ \n\n')
    dalc_fout.write(common_note_end_str);
    dalc_fout.write('#include <dal/apollo/dal_apollo.h>\n');
    dalc_fout.write('#include <rtk/[your_module].h>\n');
    dalc_fout.write('#include <dal/apollo/dal_apollo_[your_module].h>\n');

	#for rtk.c file
    rtkc_fout.write(common_note_start_str)
    rtkc_fout.write(common_note_cstart_str)
    rtkc_fout.write('	1. rename rtk.c to [your_module].c \n')
    rtkc_fout.write('	2. correct #include <rtk/[your_module].h>  \n')
    rtkc_fout.write('	3. move include header to correct position. \n')
    rtkc_fout.write(common_note_end_str);
    rtkc_fout.write('#include <rtk/init.h> \n');
    rtkc_fout.write('#include <rtk/default.h> \n');
    rtkc_fout.write('#include <rtk/[your_module].h> \n');
    rtkc_fout.write('#include <dal/dal_mgmt.h> \n');

    #for rtk.h file
    rtkh_fout.write(common_note_start_str)
    rtkh_fout.write(common_note_cstart_str)
    rtkh_fout.write('	1. rename rtk.h to [your_module].h \n')
    rtkh_fout.write('	2. copy this file to sdk/include/rtk/ \n\n')
    rtkh_fout.write(common_note_end_str);

    #for mapper.c file
    mapperc_fout.write(common_note_start_str)
    mapperc_fout.write('	1.copy the content to  sdk/src/dal/[project_name]/dal_[project_name]_mapper.c\n')
    mapperc_fout.write('	 add in dal_mapper_t\n\n')
    mapperc_fout.write('	2.add #include <dal/[project_name]/dal_[project_name]_[module_name].h> to dal_[project_name]_mapper.c\n')

    mapperc_fout.write(common_note_end_str);


    #for mapper.h file
    mapperh_fout.write(common_note_start_str)
    mapperh_fout.write('	copy the content to  sdk/include/dal/dal_mapper.h\n')
    mapperh_fout.write('	  add in dal_mapper_t\n')

    mapperh_fout.write(common_note_end_str);

	
    for line in fin:
        line_num = line_num + 1

        ## Comment line
        #comment = re.search('^\/\*.*\*\/', line)
        #if comment:
        #    dalc_fout.write(line)
        #    rtkc_fout.write(line)
        #    rtkh_fout.write(line)
        #    continue
        #
        ## Comment start
        #comment = re.search('^\/\*', line)
        #if comment:
        #    comment_flag = comment_flag + 1
        #    dalc_fout.write(line)
        #    rtkc_fout.write(line)
        #    rtkh_fout.write(line)
        #    continue
        #
        ## Comment end
        #comment = re.search('\*\/', line)
        #if comment:
        #    comment_flag = comment_flag - 1
        #    dalc_fout.write(line)
        #    rtkc_fout.write(line)
        #    rtkh_fout.write(line)
        #    continue
        #
        ## Comment body
        #if comment_flag != 0:
        #    dalc_fout.write(line)
        #    rtkc_fout.write(line)
        #    rtkh_fout.write(line)
        #    continue

        # remove extern keyword for c file
        extern_flag = re.search('^extern', line)
        if extern_flag:
            extern_line = line_num

        # skip header file define
        if len(header_str) == 0:
            header_flag = re.search('^#ifndef (.+)', line)
            if header_flag:
                header_str = header_flag.group(1)
                #sys.stdout.write('find %s\n' % header_str)
                c_skip = 1
        elif re.search(header_str + '.*', line):
            #sys.stdout.write(line)
            c_skip = 1
        elif re.search('^#endif', line):
                #sys.stdout.write(line)
                c_skip = 1
        else:
            c_skip = 0

        ## [rtk]
        # replace dal_cypress keyword to rtk
        #r = re.sub(SRC_TITLE.upper(), DST_TITLE.upper(), line)
        #r = re.sub(SRC_TITLE.lower(), DST_TITLE.lower(), r)
        r = line.replace(SRC_TITLE.upper(), DST_TITLE.upper())
        r = r.replace(SRC_TITLE.lower(), DST_TITLE.lower())

        rm = re.sub('^extern\s', '', line)

        ## function body
        if extern_line != 0:
            ## left brace
            brace = re.search('\(', r)
            if brace:
                brace_flag = line_num
                parameter = r.split("(")
                rtk_func_name = parameter[0]
                parameter = parameter[1]
                export_fout.write('EXPORT_SYMBOL(%s);\n' % rtk_func_name)
                #sys.stdout.write('A)%s %d %s\n' % (parameter, line_num, rtk_func_name))

                ## record the new distance
                brace_len = len(rtk_func_name)

            if brace_flag != 0:
                ## right brace
                brace = re.search('\)', r)
                if brace:
                    ## left & right brace in the same line
                    if brace_flag == line_num:
                        rtkh_fout.write(r)
                    ## left & right brace in the different line
                    ## remove lead whitespace & add new distance whitespace
                    else:
                        #remove lead whitespace
                        #new_line = r.lstrip()

                        #add new distance whitespace
                        #i = 0
                        #while i <= brace_len:
                        #    rtkh_fout.write(' ')
                        #    i = i + 1

                        #rtkh_fout.write(new_line)
                        rtkh_fout.write(r)

                    brace_flag = 0
                ## else for if brace:
                else:
                    rtkh_fout.write(r)
            ## else for if brace_flag != 0:
            else:
                rtkh_fout.write(line)
        ## else for if extern_line != 0:
        else:
            rtkh_fout.write(r)

        ## [dal]
        if c_skip == 1:
            continue

        ## left brace
        if extern_line != 0 and extern_line != line_num:
            extern_flag = re.search('^extern', rm)
            if extern_flag:
                print 'extern line %d %d' % (extern_line, line_num)
            body = re.search(';', rm)
            rm = re.sub(';', '', rm)

            brace = re.search('\(', rm)
            if brace:
                brace_dal_flag = line_num
                parameter = rm.split("(")
                dal_func_name = parameter[0]
                parameter = parameter[1]
                #sys.stdout.write('A)%s %d %s\n' % (parameter, line_num, dal_func_name))
            else:
                parameter = rm;

            parameter = re.sub(r'\r|\n|\)', '', parameter)
            parameter = parameter.lstrip()
            parameter = parameter.split(",")

            for k in parameter:
                k = re.sub(r'\r|\n|\)', '', k)
                #sys.stdout.write('p:%s\n' % (k))
                key = k.split(" ")

                #if key[-1] != 'unit':
                for proto in key:
                    proto = re.sub(r'\s', '', proto)
                    if len(proto) != 0:
                        if proto != key[-1]:
                            if re.search('\*', key[-1]):
                                proto = proto + " *"
                            #print '2. %s' % proto
                            proto_type.append(proto)

                rkey = key[-1].lstrip()
                if len(rkey) != 0:
                    #sys.stdout.write('pC2) %s\n' % (key[-1]))
                    if re.search('\*', key[-1]):
                        key[-1] = re.sub(r'\*', '', key[-1])
                        dict[key[-1]] = 1
                    else:
                        dict[key[-1]] = 0

                    para.append(key[-1])

            ## brace_dal_flag: record for left and right brace in diff line.
            if brace_dal_flag != 0:
                ## right brace
                brace = re.search('\)', rm)
                #sys.stdout.write('rm:%s' % (rm))

                new_line = rm.lstrip()
                new_line = re.sub(r'\r|\n|\)', '', new_line)

                if brace:
                    if brace_dal_flag != line_num:
                        new_line = rm.lstrip()
                        new_line = re.sub(r'\r|\n|\)', '', new_line)
                        parameter = new_line.split(",")

                    brace_dal_flag = 0
                ## end of if brace:

            if body:
                extern_line = 0

                dalc_fout.write(rm)

                rtc = rm.replace(SRC_TITLE.upper(), DST_TITLE.upper())
                rtc = rtc.replace(SRC_TITLE.lower(), DST_TITLE.lower())
                rtkc_fout.write(rtc)
                rtku_fout.write(rtc)

                map_func_name = re.sub(SRC_TITLE, '', dal_func_name)
                map_func_name = re.sub(r'^_', '', map_func_name)
                mapperc_fout.write('.%s = %s,\n' % (map_func_name , dal_func_name))
                mapperh_fout.write('int32   (*%s)(' % map_func_name);

                var_idx = 0
                for proto in proto_type:
                    if var_idx != 0:
                        mapperh_fout.write(', ')
                    mapperh_fout.write('%s' % proto)
                    var_idx = var_idx + 1
                mapperh_fout.write(');\n');

                ## add dal & rtkc function body
                dalc_fout.write(dal_func_body1)

                rtkc_fout.write(rtkc_func_body1)
                rtkc_fout.write(rtkc_func_body2)
                rtkc_fout.write(rtkc_func_body3 % map_func_name);

                rtku_fout.write(rtku_func_body1);

                rtdrv_title = re.search('\(', rtc)
                if rtdrv_title:
                    rtdrv = rtc.split("(")
                    netfilter = rtdrv[0]
                    rtku = rtdrv[0].replace(DST_TITLE, RTU_TITLE)

                dalc_fout.write('    /* parameter check */\n')

                var_idx = 0
                for k in para:
 
                    #sys.stdout.write('Line%d %s \n' % (line_num, k))
                    v = dict[k]

                #@for k, v in dict.iteritems():
                	#sys.stdout.write('%s %d\n' % (k, v))
                    # constant
                    if v == 0 and k != 'unit' and k != 'void':
                        dalc_fout.write('    RT_PARAM_CHK((RTK_RANGE_DEFINED_XXX <= %s), RT_ERR_INPUT);\n' % k)
                    # pointer
                    elif v == 1:
                        dalc_fout.write('    RT_PARAM_CHK((NULL == %s), RT_ERR_NULL_POINTER);\n' % k)

                    if var_idx != 0:
                    	rtkc_fout.write(',')
                    rtkc_fout.write(' %s' % k)
                    rtku_fout.write('    xxx_cfg.%s = %s;\n' % (k, k))
                    var_idx = var_idx + 1

                dalc_fout.write('\n')
                dalc_fout.write(dal_func_body2)
                dalc_fout.write(' /* end of %s */' % dal_func_name)
                dalc_fout.write('\n')


                if re.search('get', rtku) or re.search('read', rtku) or re.search('check', rtku):
                    rtku_fout.write(rtku_get_func_body % rtku.upper());
                    netfilter_get_fout.write('    %s,\n' % rtku.upper());
                    netfilterc_get_fout.write(netfilterc_body1 % rtku.upper());
                    netfilterc_get_fout.write(netfilterc_get_body1 % netfilter);
                    netfilterc_get_fout.write(netfilterc_get_body2);
                else :
                    rtku_fout.write(rtku_set_func_body % rtku.upper());
                    netfilter_set_fout.write('    %s,\n' % rtku.upper());
                    netfilterc_set_fout.write(netfilterc_body1 % rtku.upper());
                    netfilterc_set_fout.write(netfilterc_set_body1 % netfilter);

                rtkc_fout.write(');')
                
                rtkc_fout.write(rtkc_func_body4)
                rtkc_fout.write(rtkc_func_body5)
                
                rtkc_fout.write('} /* end of %s */\n' % rtk_func_name)

                rtku_fout.write(rtku_func_body2);
                rtku_fout.write('} /* end of %s */\n\n' % rtk_func_name)

                para = []
                proto_type = []

            ## end of if body:
            else:
                dalc_fout.write(rm)
                rtc = rm.replace(SRC_TITLE.upper(), DST_TITLE.upper())
                rtc = rtc.replace(SRC_TITLE.lower(), DST_TITLE.lower())
                rtkc_fout.write(rtc)
                rtku_fout.write(rtc)

                rtdrv_title = re.search('\(', rtc)
                if rtdrv_title:
                    rtdrv = rtc.split("(")
                    netfilter = rtdrv[0]
                    rtku = rtdrv[0].replace(DST_TITLE, RTU_TITLE)

        ## end of if extern_line != 0:
        else:
            dalc_fout.write(rm)
            rtc = rm.replace(SRC_TITLE.upper(), DST_TITLE.upper())
            rtc = rtc.replace(SRC_TITLE.lower(), DST_TITLE.lower())
            rtkc_fout.write(rtc)
            ## rtkusr needn't function header
            if extern_line == line_num:
                rtku_fout.write(rm)
        #sys.stdout.write(line)
        
    fin.close()
    dalc_fout.close()
    rtkc_fout.close()
    rtkh_fout.close()
    export_fout.close()
    rtku_fout.close()
    mapperc_fout.close()
    mapperh_fout.close()
    return

## Process the command-line argument
def main():
    f_dir   = '.'
    f_fname = ''
    # Parse input arguments
    sys.argv.pop(0) # remove mk_parser.py itself

    if len(sys.argv) == 0:
        sys.stdout.write("argv is null")
        return

    while (len(sys.argv) > 0):
        item = sys.argv.pop(0)
        if '-fd' == item:
            f_dir = sys.argv.pop(0)
        elif '-f' == item:
            f_fname = sys.argv.pop(0)
        else:
            sys.stdout.write("parameter incorrect\n")
            return

    process_input_file(f_dir, f_fname)

    return

# Entry point of the script
if __name__ == '__main__':
    main()

