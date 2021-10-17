#!/usr/bin/env python
import re, sys, os

Apollo = True

TOKENS = [ 'STRING', 'UINT64', 'UINT',
           'INT64', 'INT', 'HEX', 'HEX64', 'FLOAT', 'MACADDR', 'IPV4ADDR',
           'FILE', 'PORT_LIST', 'IPV6ADDR', 'MASK_LIST' ]

TYPES = { 'STRING'     : 'char *',
          'UINT64'     : 'uint64_t ',
          'UINT'       : 'uint32_t ',
          'INT64'      : 'int64_t ',
          'INT'        : 'int32_t ',
          'HEX64'      : 'uint64_t ',
          'HEX'        : 'uint32_t ',
          'FLOAT'      : 'double ',
          'MACADDR'    : 'cparser_macaddr_t ',
          'IPV4ADDR'   : 'uint32_t ',
          'FILE'       : 'char *',
          'PORT_LIST'  : 'char *',
          'IPV6ADDR'   : 'char *',
          'MASK_LIST'  : 'char *'
          }

class COMMAND:
        def __init__(self, cli_str, help_str, func_str):
                self.cli = cli_str
                self.help = help_str
                self.func = func_str
        def __getitem__(self, key):
                return (self.func)

diag_func_body1 = '/*\n' + ' * %s' + ' */\n'
diag_func_body2 = 'cparser_result_t\n' + 'cparser_cmd_%s(\n' + '    cparser_context_t *context'
diag_func_body3 = '{\n' + '    uint32  unit;\n    int32   ret;\n\n' + '    DIAG_UTIL_PARAM_CHK();\n' + '    DIAG_OM_GET_CHIP_ID(unit);\n'
diag_func_get_body1 = '    DIAG_UTIL_OUTPUT_INIT();\n' + '\n    diag_util_mprintf("");\n'
diag_func_body4 = '\n    return CPARSER_OK;\n' + '}    /* end of cparser_cmd_%s */\n'
diag_func_body5 = '{\n' + '    DIAG_UTIL_PARAM_CHK();\n'


def process_input_file(filedir, filename):
    line = ''
    help = ''
    line_num = 0
    sort_list = []

    root_fname = filedir + '/' + filename

    try:
        fin = open(root_fname, 'r')
    except:
        print 'Fail to open %s.' % root_fname
        sys.exit(-1)

    # Generate dal.c, rtk.c and rtk.h file
    diag_fname = filedir + '/' + "diag.c"
    flag_fname = filedir + '/' + "flag.h"
    sort_fname = filedir + '/' + "diag_sort.cli"
    cmd_list_fname = filedir + "cmd_list.c"

    try:
        diag_fout = open(diag_fname, 'w+')
    except:
        print 'Fail to open %s.' % diag_fname
        sys.exit(-1)

    try:
        flag_fout = open(flag_fname, 'w+')
    except:
        print 'Fail to open %s.' % flag_fname
        sys.exit(-1)

    try:
        sort_fout = open(sort_fname, 'w+')
    except:
        print 'Fail to open %s.' % sort_fname
        sys.exit(-1)

    try:
        cmd_list_fout = open(cmd_list_fname, 'a')
    except:
        print 'Fail to open %s.' % cmd_list_fname
        sys.exit(-1)

    for line in fin:
        line_num = line_num + 1
        dict = {}
        para = []
        body = ''
        offset = 0

        global Apollo

        ## help line
        ## skip /* */
        comment = re.search('^\/\*.*\*\/', line)
        if comment:
            continue

        ## skip #
        comment = re.search('^\#', line)
        if comment:
            continue

        ## skip //
        comment = re.search('^\/\/', line)
        if comment:
            help = help + line
            continue

        comment = re.search('^\\n', line)
        if comment:
            help = ''
            continue;

        comment = re.search('^\\r', line)
        if comment:
            help = ''
            continue;

        #print line
        cmd_list_fout.write('%s' % line)

        parameter = line.split(" ")
        #print len(parameter)
        for k in parameter:
            k = re.sub(r'\s', '', k)
            if len(k) == 0:
                continue

            offset = offset + 1;

            k = re.sub(r'\r|\n', '', k)

            comment = re.search('\|', k)
            if comment:
                continue;

            comment = re.search('\{|\}', k)
            if comment:
                continue;

            comment = re.search('\[|\]', k)
            if comment:
                continue;

            comment = re.search('\(|\)', k)
            if comment:
                continue;

            k = re.sub(r'\<|\>', '', k)

            ## handel variable
            for token in TOKENS:
                comment = re.search(token, k)
                if comment:
                    break

            if comment:
                k = re.sub(token, '', k)
                k = re.sub(r'\:', '', k)
                dict[k] = TYPES[token]
                para.append(k)

            ## end of handel variable

            k = k.replace('-', '_')

            if offset != 1:
                body = body + "_"
            body = body + k
            #print '%d %s' % (offset, k)

        if offset == 0:
            continue

        sort_list.append(COMMAND(line, help, body))

        #print body
        flag_fout.write('#define CMD_%s\n' % body.upper());

        if not Apollo:
            diag_fout.write('#ifdef CMD_%s\n' % body.upper());

        diag_fout.write(diag_func_body1 % line);
        diag_fout.write(diag_func_body2 % body);

        ## input parameter
        for key in para:
            v = dict[key]
            diag_fout.write(',\n    %s *%s_ptr' % (v, key));

        diag_fout.write(')\n');
        # end input parameter

        if Apollo:
            diag_fout.write(diag_func_body5)
        else:
            diag_fout.write(diag_func_body3)

        if re.search('get', body) or re.search('read', body) or re.search('check', body):
            diag_fout.write(diag_func_get_body1);
        diag_fout.write(diag_func_body4 % body.lower());
        if Apollo:
            diag_fout.write('\n');
        else:
            diag_fout.write('#endif\n\n');

    sort_list.sort(key=lambda x:x[0])

    for new_line in sort_list:
        if not Apollo:
            sort_fout.write('#ifdef CMD_%s\n' % new_line.func.upper());
        sort_fout.write('%s' % new_line.help)
        sort_fout.write('%s' % new_line.cli)
        if Apollo:
            sort_fout.write('\n');
        else:
            sort_fout.write('#endif\n\n');

    fin.close()
    diag_fout.close()
    flag_fout.close()
    sort_fout.close()
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

