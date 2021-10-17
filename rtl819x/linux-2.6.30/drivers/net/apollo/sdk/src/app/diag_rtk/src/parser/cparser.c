/**
 * \file     cparser.c
 * \brief    parser top-level API
 * \version  \verbatim $Id: cparser.c 129 2009-03-29 21:12:59Z henry $ \endverbatim
 */
/*
 * Copyright (c) 2008, Henry Kwok
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the project nor the names of its contributors
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY HENRY KWOK ''AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL HENRY KWOK BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//#include <assert.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <parser/cparser.h>
#include <parser/cparser_priv.h>
#include <parser/cparser_token.h>
#include <parser/cparser_io.h>
#include <parser/cparser_fsm.h>

typedef struct cparser_desc_s
{
    char *desc;
} cparser_desc_t;

typedef struct cparser_help_buf_s
{
    int             num;    /* uesd elements */
    cparser_desc_t  *info;
    int             *order; /* record  order of the element */
} cparser_help_buf_t;

/**
 * \brief    Print the description of a node out.
 * \details  For keyword nodes,  the keyword itself is printed. For parameter
 *           nodes, a string of the form <[type]:[parameter]> is printed. For
 *           end nodes, \<LF\> is printed. This call should never be invoked with
 *           a root node.
 *
 * \param    parser Pointer to the parser structure.
 * \param    node   Pointer to the node to be printed.
 */
static void
cparser_help_print_node (cparser_t *parser, cparser_node_t *node,
    cparser_help_buf_t *buf)
{
    int             len;

    //assert(parser && node);
    if (!(parser && node && buf))
    {
        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return;
    }
    //parser->cfg.printc(parser, '\n');

    switch (node->type) {
        case CPARSER_NODE_ROOT:
            //assert(0); /* this should never happen */
            printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
            return;
        case CPARSER_NODE_END:
            //parser->cfg.prints(parser, CPARSER_END_OF_CMD_STR);
            buf->info[buf->num].desc = malloc((strlen(CPARSER_END_OF_CMD_STR)+ 1));
            strcpy(buf->info[buf->num].desc, CPARSER_END_OF_CMD_STR);
            buf->num++;
            break;
        default:
            len = atoi(CPARSER_MAX_TOKENS_DISP_SIZE) + 1;

            if (node->desc)
            {
                /* add space token (2) */
                len += (strlen(" - ") + strlen(node->desc));

                buf->info[buf->num].desc = malloc(len);
                if (!buf->info[buf->num].desc)
                {
                    printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                    return;
                }

                sprintf(buf->info[buf->num].desc, "%-"CPARSER_MAX_TOKENS_DISP_SIZE"s%s%s", (char *)node->param, " - ", node->desc);
            }
            else
            {
                buf->info[buf->num].desc = malloc(len);
                sprintf(buf->info[buf->num].desc, "%s", (char *)node->param);
            }

            buf->num++;

            #if 0
            parser->cfg.prints(parser, node->param);
            if (node->desc) {
                parser->cfg.prints(parser, " - ");
                parser->cfg.prints(parser, node->desc);
            }
            #endif
            break;
    }
}
/**
 * \brief    If the command is not complete, attempt to complete the command.
 *           If there is a complete comamnd, execute the glue (& action) 
 *           function of a command.
 *
 * \param    parser Pointer to the parser structure.
 *
 * \return   CPARSER_OK if a valid command is executed; CPARSER_NOT_OK 
 *           otherwise.
 */
static cparser_result_t
cparser_execute_cmd (cparser_t *parser)
{
    int n, m;
    int token_num = 0;

    //assert(VALID_PARSER(parser));
    if (!(VALID_PARSER(parser)))
    {
        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }    

#if 1 /*for support multi-line input control*/
    {
        cparser_line_t *line;
        line = &parser->cur_lines_body;
    
        if(line->buf[0] == '#') 
        {
            parser->cfg.printc(parser, '\n');
            /* Reset FSM states and advance to the next line */
            cparser_fsm_reset(parser);
            cparser_line_advance(parser);
            parser->cfg.prints(parser, parser->prompt[parser->root_level]);
        
            return CPARSER_OK;            
        }
    }
#endif


    /* 
     * Enter a command. There are three possibilites:
     * 1. If we are in WHITESPACE state, we check if there is
     *    only one child of keyword type. If yes, we recurse 
     *    into it and repeat until either: a) there is more than
     *    one choice, b) We are at a END node. If there is more
     *    than one choice, we look for an END node. In either
     *    case, if an END node is found, we execute the action
     *    function.
     *
     * 2. If we are in TOKEN state, we check if we have an unique
     *    match. If yes, re recurse into it and repeat just
     *    like WHITESPACE state until we find an END node.
     *
     * 3. If we are in ERROR state, we print out an error.
     *
     * Afterward, we reset the parser state and move to the
     * next line buffer.
     */
    if ((CPARSER_STATE_TOKEN == parser->state) ||
        (CPARSER_STATE_WHITESPACE == parser->state)) {
        cparser_node_t *child;
        cparser_result_t rc;

        if (CPARSER_STATE_TOKEN == parser->state) {
            cparser_token_t *token;
            cparser_node_t *match;
            int is_complete;

            token = CUR_TOKEN(parser);
            if ((1 <= cparser_match(token->buf, token->token_len,
                                    parser->cur_node, &match, 
                                    &is_complete)) &&
                (is_complete)) {
                token->parent = parser->cur_node;
                token->buf[token->token_len] = '\0';

                parser->token_tos++;
                //assert(CPARSER_MAX_NUM_TOKENS > parser->token_tos);
                if (!(CPARSER_MAX_NUM_TOKENS > parser->token_tos))
                {
                    printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                    return  CPARSER_NOT_OK;
                }    
                memset(CUR_TOKEN(parser), 0, sizeof(cparser_token_t));

                parser->last_good = cparser_line_current(parser);
                parser->cur_node = match;
            } else {
                parser->cfg.printc(parser, '\n');
                m = strlen(parser->prompt[parser->root_level]) + 1;
                for (n = 0; n < m+parser->last_good; n++) {
                    parser->cfg.printc(parser, ' ');
                }
                parser->cfg.printc(parser, '^');
                parser->cfg.prints(parser, "Incomplete command\n\n");
                    
                /* Reset the internal buffer, state and cur_node */
                cparser_fsm_reset(parser);
                cparser_line_advance(parser);
                parser->cfg.prints(parser, parser->prompt[parser->root_level]);

                return CPARSER_OK;
            }
        }

        /* Look for a single keyword node child */
        child = parser->cur_node->children;
        //assert(child);
        if (!(child))
        {
            printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
            return  CPARSER_NOT_OK;
        } 
        while ((CPARSER_NODE_KEYWORD == child->type) &&
               (!child->sibling)) {
            parser->cur_node = child;
            child = parser->cur_node->children;
            //assert(child);
            if (!(child))
            {
                printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                return  CPARSER_NOT_OK;
            } 
        }

        /* Look for an end node */
        child = parser->cur_node->children;
        while ((NULL != child) && (CPARSER_NODE_END != child->type)) {
            child = child->sibling;
        }
        if (child) {
            //assert(CPARSER_NODE_END == child->type);
            if (!(CPARSER_NODE_END == child->type))
            {
                printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                return  CPARSER_NOT_OK;
            } 
            /* Execute the glue function */
            parser->cur_node = child;
            parser->cfg.printc(parser, '\n');
            for (token_num = 0; token_num < CPARSER_MAX_NUM_TOKENS; token_num++)
            {
                if (parser->tokens[token_num].token_len <= 0)
                {
                    break;
                }
            }
            parser->cmd_tokens = token_num;
            rc = ((cparser_glue_fn)child->param)(parser);
        } else {
            if (parser->token_tos) {
                parser->cfg.printc(parser, '\n');
                m = strlen(parser->prompt[parser->root_level]);
                for (n = 0; n < m+parser->last_good; n++) {
                    parser->cfg.printc(parser, ' ');
                }
                parser->cfg.printc(parser, '^');
                parser->cfg.prints(parser, "Incomplete command\n\n");
            }
                    
            /* Reset FSM states and advance to the next line */
            cparser_fsm_reset(parser);
            /* Don't advance line when input empty command */
            //cparser_line_advance(parser);
            rc = cparser_line_reset(&parser->cur_lines_body);
            if (!(0 == rc))
            {
                printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                return CPARSER_NOT_OK;
            }
            
            parser->cfg.prints(parser, parser->prompt[parser->root_level]);
 
            return CPARSER_OK;
        }
    } else if (CPARSER_STATE_ERROR == parser->state) {
        parser->cfg.printc(parser, '\n');
        m = strlen(parser->prompt[parser->root_level]) + 1;
        for (n = 0; n < m+parser->last_good; n++) {
            parser->cfg.printc(parser, ' ');
        }
        parser->cfg.printc(parser, '^');
        parser->cfg.prints(parser, "Parse error\n");
    }

    /* Reset FSM states and advance to the next line */
    cparser_fsm_reset(parser);
    cparser_line_advance(parser);
    parser->cfg.prints(parser, parser->prompt[parser->root_level]);

    return CPARSER_OK;
}

static cparser_result_t
cparser_match_prefix (const char *token, const int token_len,
                      const cparser_node_t *parent, const char ch, 
                      const int offset)
{
    int local_is_complete;
    cparser_node_t *child;
    cparser_result_t rc;

    //assert(parent && ch);
    if (!(parent && ch))
    {
        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    for (child = parent->children; NULL != child; child = child->sibling) {
        rc = cparser_match_fn_tbl[child->type](token, token_len, child, 
                                               &local_is_complete);
        if (CPARSER_NOT_OK == rc) {
            continue;
        }
        if (CPARSER_NODE_KEYWORD != child->type) {
            return CPARSER_NOT_OK;
        }

        /* There is a match. Make sure that it is part of this node as well */
        if (*((char *)child->param + offset) != ch) {
            return CPARSER_NOT_OK;
        }
    }
    return CPARSER_OK;
}
static void
cparser_help_str_sort(cparser_help_buf_t *buf)
{
    int i, j;
    int cnt;        /* the order of string buffer */

    for (i = 0; i < buf->num; ++i)
    {
        cnt = 0;
        if (strcmp(buf->info[i].desc, CPARSER_END_OF_CMD_STR) == 0)
        {
            buf->order[i] = (buf->num - 1);
            continue;
        }

        for (j = 0; j < buf->num; ++j)
        {
            /* first char is '<' */
            if (buf->info[i].desc[0] == 60)
            {
                if (buf->info[j].desc[0] == 60)
                {
                    if (strcmp(buf->info[j].desc, CPARSER_END_OF_CMD_STR) == 0)
                        continue;
                    else if (strcmp(buf->info[i].desc, buf->info[j].desc) > 0)
                        ++cnt;
                }
                else
                    ++cnt;
            }
            else
            {
                if (buf->info[j].desc[0] != 60)
                {
                    if (strcmp(buf->info[i].desc, buf->info[j].desc) > 0)
                        ++cnt;
                }
            }
        }   /* for (j = 0; j < buf->num; ++j) */

        buf->order[i] = cnt;
    }   /* for (i = 0; i < buf->num; ++i) */
}   /* end of cparser_help_str_sort */

/**
 * \brief    Generate context-sensitive help.
 *
 * \param    parser Pointer to the parser structure.
 */
static cparser_result_t
cparser_help (cparser_t *parser)
{
    cparser_node_t *node, *match;
    cparser_token_t *token;
    int local_is_complete;
    int                 i, j;
    cparser_help_buf_t  buf;

    //assert(VALID_PARSER(parser));
    if (!(VALID_PARSER(parser)))
    {
        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }     

    memset(&buf, 0, sizeof(cparser_help_buf_t));

    i = 0;
    for (node = parser->cur_node->children; NULL != node; node = node->sibling)
    {
        ++i;
    }

    buf.info = malloc((sizeof(cparser_desc_t) * i));
    buf.order = malloc((sizeof(int) * i));
    if (!buf.info || !buf.order)
    {
        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
     
    if (CPARSER_STATE_WHITESPACE == parser->state) {
        /* Just print out every children */
        for (node = parser->cur_node->children; NULL != node;
             node = node->sibling) {
            cparser_help_print_node(parser, node, &buf);
        }
    } else {
        /* We have a partial match */
        node = parser->cur_node->children;
        match = NULL;
        token = CUR_TOKEN(parser);
        for (node = parser->cur_node->children; NULL != node; 
             node = node->sibling) {
            if (CPARSER_OK == 
                cparser_match_fn_tbl[node->type](token->buf, token->token_len, 
                                                 node, &local_is_complete)) {
                cparser_help_print_node(parser, node, &buf);
            }
        }
    }
    /* Print out a new line after help message */
    cparser_help_str_sort(&buf);

    parser->cfg.printc(parser, '\n');

    for (i = 0; i < buf.num; ++i)
    {
        for (j = 0; j < buf.num; ++j)
        {
            if (buf.order[j] == i)
            {
                parser->cfg.prints(parser, buf.info[j].desc);
                parser->cfg.printc(parser, '\n');
            }
        }
    }
    
    
    for (i = 0; i < buf.num; ++i)
    {
        free(buf.info[i].desc);
    }
    free(buf.order);
    free(buf.info);

    cparser_line_print(parser, 1, 1);
    return CPARSER_OK;
}

cparser_result_t
cparser_input (cparser_t *parser, char ch, cparser_char_t ch_type)
{
    int n;
    cparser_token_t *token;
    cparser_result_t rc;
#if 1 /*for support multi-line input control*/
    cparser_line_t *line;
    int need_parser;

    line = &parser->cur_lines_body;
#endif

    if (!VALID_PARSER(parser)) {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    switch (ch_type) {
        case CPARSER_CHAR_REGULAR:
        {
            if ((parser->cfg.ch_complete == ch) || 
                (parser->cfg.ch_help == ch)) {
                /* 
                 * Completion and help character do not go into the line
                 * buffer. So, do nothing.
                 */
                break;
            }
            
#if 1 /*for support multi-line input control*/
            if(ch == '\\')
            {
                line->buf[line->current] = ch;
                parser->cfg.printc(parser, ch);
                line->current++; /* update current position */
                line->last++;
                return CPARSER_OK;
            }
#endif
            if ((parser->cfg.ch_erase == ch) || (parser->cfg.ch_del == ch)) {
#if 1 /*for support multi-line input control*/
                if('\\'==line->buf[line->current-1])
                    need_parser = 0;
                else
                    need_parser = 1;
#endif
                rc = cparser_line_delete(parser);
                //assert(CPARSER_ERR_INVALID_PARAMS != rc);
                if (!(CPARSER_ERR_INVALID_PARAMS != rc))
                {
                    printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                    return  CPARSER_NOT_OK;
                }  
                if (CPARSER_ERR_NOT_EXIST == rc) {
                    return CPARSER_OK;
                }
#if 1 /*for support multi-line input control*/
                if (0 == need_parser) {
                    return CPARSER_OK;
                }
#endif                
            } else if ('\n' == ch) {
                /* Go to next new line when only press 'Enter' */


                if(0 == cparser_line_current(parser))
                    parser->cfg.printc(parser, '\n');


                /* Put the rest of the line into parser FSM */
                for (n = cparser_line_current(parser); 
                     n < cparser_line_last(parser); n++) {
                    rc = cparser_fsm_input(parser, cparser_line_char(parser, n));
                    //assert(CPARSER_OK == rc);
                    if (!(CPARSER_OK == rc))
                    {
                        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                        return  CPARSER_NOT_OK;
                    }  
                }
            } else {
                (void)cparser_line_insert(parser, ch);
            }
            break;
        }
        case CPARSER_CHAR_UP_ARROW:
        {
            rc = cparser_line_prev_line(parser);
            //assert(CPARSER_OK == rc);
            if (!(CPARSER_OK == rc))
            {
                printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                return  CPARSER_NOT_OK;
            } 
            /* Reset the token stack and re-enter the command */
            cparser_fsm_reset(parser);
            for (n = 0; n < cparser_line_current(parser); n++) {
                rc = cparser_fsm_input(parser, cparser_line_char(parser, n));
                //assert(CPARSER_OK == rc);
                if (!(CPARSER_OK == rc))
                {
                    printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                    return  CPARSER_NOT_OK;
                } 
            }
            
            return CPARSER_OK;
        }
        case CPARSER_CHAR_DOWN_ARROW:
        {
            rc = cparser_line_next_line(parser);
            //assert(CPARSER_OK == rc);
            if (!(CPARSER_OK == rc))
            {
                printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                return  CPARSER_NOT_OK;
            } 
            
            /* Reset the token stack and re-enter the command */
            cparser_fsm_reset(parser);
            for (n = 0; n < cparser_line_current(parser); n++) {
                rc = cparser_fsm_input(parser, cparser_line_char(parser, n));
                //assert(CPARSER_OK == rc);
                if (!(CPARSER_OK == rc))
                {
                    printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                    return  CPARSER_NOT_OK;
                } 
            }
            
            return CPARSER_OK;
        }
        case CPARSER_CHAR_LEFT_ARROW:
        {
            ch = cparser_line_prev_char(parser);
            if (!ch) {
                parser->cfg.printc(parser, '\a');
                return CPARSER_OK;
            }
            break;
        }
        case CPARSER_CHAR_RIGHT_ARROW:
        {
            ch = cparser_line_next_char(parser);
            if (!ch) {
                parser->cfg.printc(parser, '\a');
                return CPARSER_OK;
            }
            break;
        }
        default:
        {
            /* An unknown character. Alert and continue */
            parser->cfg.printc(parser, '\a');
            return CPARSER_NOT_OK;
        }
    } /* switch (ch_type) */

    /* Handle special characters */
    if (ch == parser->cfg.ch_complete) {
        cparser_node_t *match;
        int is_complete = 0, num_matches;
        char *ch_ptr;

        switch (parser->state) {
            case CPARSER_STATE_ERROR:
                /* If we are in ERROR, there cannot be a match. So, just quit */
                parser->cfg.printc(parser, '\a');
                break;
            case CPARSER_STATE_WHITESPACE:
                /* 
                 * If we are in WHITESPACE, just dump all children. Since there is no
                 * way any token can match to a NULL string.
                 */
                cparser_help(parser);
                break;
            case CPARSER_STATE_TOKEN:
            {
                /* Complete a command */
                token = CUR_TOKEN(parser);
                num_matches = cparser_match(token->buf, token->token_len,
                                            parser->cur_node, &match, 
                                            &is_complete);
                if ((1 == num_matches) && (is_complete)) {
                    /*
                     * If the only matched node is a keyword, we feel the rest of
                     * keyword in. Otherwise, we assume this parameter is complete
                     * and just insert a space.
                     */
                    if (CPARSER_NODE_KEYWORD == match->type) {
                        ch_ptr = match->param + token->token_len;
                        while (*ch_ptr) {
                            rc = cparser_input(parser, *ch_ptr, CPARSER_CHAR_REGULAR);
                            //assert(CPARSER_OK == rc);
                            if (!(CPARSER_OK == rc))
                            {
                                printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                                return  CPARSER_NOT_OK;
                            } 
                            ch_ptr++;
                        }
                    }
                    rc = cparser_input(parser, ' ', CPARSER_CHAR_REGULAR);
                    //assert(CPARSER_OK == rc);
                    if (!(CPARSER_OK == rc))
                    {
                        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                        return  CPARSER_NOT_OK;
                    } 
                } else {
                    int offset, orig_offset;
                    /* 
                     * If we have more than one match, we should try to complete
                     * as much as possible. To do that, we grab the node in the 
                     * (first) matched node and check that the next character
                     * from it is common among all matched nodes. If it is common
                     * to all matched nodes, we continue to feed them into the
                     * parser. However, this is only useful for keywords. If there
                     * is a parameter token in the match, we automatically abort.
                     */
                    offset = orig_offset = token->token_len;
                    ch_ptr = match->param + token->token_len;
                    while (('\0' != *ch_ptr) &&
                           (CPARSER_OK == 
                            cparser_match_prefix(token->buf, token->token_len,
                                                 parser->cur_node, *ch_ptr, 
                                                 offset))) {
                        rc = cparser_input(parser, *ch_ptr, CPARSER_CHAR_REGULAR);
                        //assert(CPARSER_OK == rc);
                        if (!(CPARSER_OK == rc))
                        {
                            printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                            return  CPARSER_NOT_OK;
                        } 
                        ch_ptr++;
                        offset++;
                    }
                    if (orig_offset == offset) {
                        /* If there is no common prefix at all, just display help */
                        cparser_help(parser);
                    }
                }
                break;
            }
            default: 
                //assert(0);
                printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
                return  CPARSER_NOT_OK;
        }
        return CPARSER_OK;
    } else if (ch == parser->cfg.ch_help) {
        /* Ask for context sensitve help */
        cparser_help(parser);
        return CPARSER_OK;
    } else if ('\n' == ch) {
#if 1 /*for support multi-line input control*/
        if(line->buf[line->current-1]=='\\')
        {
            //printf("\n get change line!!  1 0x%x",line->buf[line->current-1]);    
            line->current--;
            line->last--;
            parser->cfg.printc(parser, '\n');
            return CPARSER_OK;
        }
        else
        {
            return cparser_execute_cmd(parser);
        }
#else

#endif        
    }
    
    return cparser_fsm_input(parser, (char)ch);
}

cparser_result_t
cparser_run (cparser_t *parser)
{
    int ch;
    cparser_char_t ch_type = 0;

    if (!VALID_PARSER(parser)) return CPARSER_ERR_INVALID_PARAMS;

    parser->cfg.io_init(parser);
    parser->cfg.prints(parser, parser->prompt[parser->root_level]);


    parser->done = 0;

    while (!parser->done) {
        parser->cfg.getch(parser, &ch, &ch_type);
        cparser_input(parser, ch, ch_type);
    } /* while not done */

    parser->cfg.io_cleanup(parser);

    return CPARSER_OK;
}


cparser_result_t
cparser_run_not_start_shell (cparser_t *parser,char *cmd)
{
    int ch;
    int i=0;
    cparser_char_t ch_type = 0;

    if (!VALID_PARSER(parser)) return CPARSER_ERR_INVALID_PARAMS;

    parser->cfg.io_init(parser);

    parser->done = 0;
    
    printf("command:"); 
    while (1) {
        ch = cmd[i];
        if(ch == 0x0)
            ch = '\n';        
        cparser_input(parser, ch, CPARSER_CHAR_REGULAR);
        if(cmd[i] == 0x0)
            break;
        i++;
    } /* while not done */
    
    
    parser->cfg.io_cleanup(parser);

    return CPARSER_OK;
}


cparser_result_t 
cparser_init (cparser_cfg_t *cfg, cparser_t *parser)
{
    int n;

    if (!parser || !cfg || !cfg->root || !cfg->ch_erase) {
	return CPARSER_ERR_INVALID_PARAMS;
    }

    parser->cfg = *cfg;
    parser->cfg.prompt[CPARSER_MAX_PROMPT-1] = '\0';

    /* Initialize sub-mode states */
    parser->root_level = 0;
    parser->root[0] = parser->cfg.root;
    snprintf(parser->prompt[0], sizeof(parser->prompt[0]), "%s", 
             parser->cfg.prompt);



    for (n = 0; n < CPARSER_MAX_NESTED_LEVELS; n++) {
        parser->context.cookie[n] = NULL;
    }
    parser->context.parser = parser;

    /* Initialize line buffering states */
    parser->max_line = 0;
    parser->cur_line = 0;
    for (n = 0; n < CPARSER_MAX_LINES; n++) {
        cparser_line_reset(&parser->lines[n]);
    }
    cparser_line_reset(&parser->cur_lines_body);



    /* Initialize parser FSM state */
    cparser_fsm_reset(parser);

    return CPARSER_OK;
}

cparser_result_t
cparser_quit (cparser_t *parser)
{
    if (!parser) {
        return CPARSER_ERR_INVALID_PARAMS;
    }
    parser->done = 1;
    return CPARSER_OK;
}

cparser_result_t
cparser_submode_enter (cparser_t *parser, void *cookie, char *prompt)
{
    cparser_node_t *new_root;

    if (!parser) {
        return CPARSER_ERR_INVALID_PARAMS;
    }
    if ((CPARSER_MAX_NESTED_LEVELS-1) == parser->root_level) {
        return CPARSER_NOT_OK;
    }
    parser->root_level++;
    new_root = parser->cur_node->children;
    //assert(new_root);
    if (!(new_root))
    {
        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    //assert(CPARSER_NODE_ROOT == new_root->type);
    if (!(CPARSER_NODE_ROOT == new_root->type))
    {
        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    } 
    parser->root[parser->root_level] = new_root;
    snprintf(parser->prompt[parser->root_level], 
             sizeof(parser->prompt[parser->root_level]), "%s", prompt);
    parser->context.cookie[parser->root_level] = cookie;
    
    return CPARSER_OK;
}

cparser_result_t
cparser_submode_exit (cparser_t *parser)
{
    if (!parser) {
        return CPARSER_ERR_INVALID_PARAMS;
    }
    if (!parser->root_level) {
        return CPARSER_NOT_OK;
    }
    parser->root_level--;
    return CPARSER_OK;
}

cparser_result_t 
cparser_load_cmd (cparser_t *parser, char *filename)
{
    FILE *fp;
    char buf[128];
    size_t rsize, n;
    int fd, indent = 0, last_indent = -1, new_line = 1, m;

    if (!VALID_PARSER(parser) || !filename) {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    fd = parser->cfg.fd;
    parser->cfg.fd = -1;

    fp = fopen(filename, "r");
    if (!fp) {
        return CPARSER_NOT_OK;
    }
    
    cparser_fsm_reset(parser);
    while (!feof(fp)) {
        rsize = fread(buf, 1, sizeof(buf), fp);
        for (n = 0; n < rsize; n++) {
            /* Examine the input characters to maintain indent level */
            if ('\n' == buf[n]) {
                indent = 0;
                new_line = 1;
                (void)cparser_execute_cmd(parser);
                continue;
            } else if (' ' == buf[n]) {
                if (new_line) {
                    indent++;
                }
            } else {
                if (new_line) {
                    new_line = 0;
                    if (indent < last_indent) {
                        for (m = indent; m < last_indent; m++) {
                            if (CPARSER_OK != cparser_submode_exit(parser)) {
                                break;
                            }
                            cparser_fsm_reset(parser);
                        }
                    }
                    last_indent = indent;
                }
            }
            (void)cparser_fsm_input(parser, buf[n]);
        }
    }
    fclose(fp);

    while (parser->root_level) {
        (void)cparser_submode_exit(parser);
        cparser_fsm_reset(parser);
    }
    parser->cfg.fd = fd;
    return CPARSER_OK;
}

static cparser_result_t
cparser_walk_internal (cparser_t *parser, cparser_node_t *node, 
                       cparser_walker_fn pre_fn, cparser_walker_fn post_fn, 
                       void *cookie)
{
    cparser_result_t rc;
    cparser_node_t *cur_node;

    if (pre_fn) {
        rc = pre_fn(parser, node, cookie);
        if (CPARSER_OK != rc) {
            return rc;
        }
    }

    if (CPARSER_NODE_END != node->type) {
        cur_node = node->children;
        while (cur_node) {
            cparser_walk_internal(parser, cur_node, pre_fn, post_fn, cookie);
            cur_node = cur_node->sibling;
        }
    }
        
    if (post_fn) {
        rc = post_fn(parser, node, cookie);
        if (CPARSER_OK != rc) {
            return rc;
        }
    }

    return CPARSER_OK;
}

cparser_result_t
cparser_walk (cparser_t *parser, cparser_walker_fn pre_fn, 
              cparser_walker_fn post_fn, void *cookie)
{
    if (!VALID_PARSER(parser) || (!pre_fn && !post_fn)) {
        return CPARSER_ERR_INVALID_PARAMS;
    }

    return cparser_walk_internal(parser, parser->root[parser->root_level], 
                                 pre_fn, post_fn, cookie);
}

typedef struct help_stack_ {
    char *filter;
    int  tos;
    cparser_node_t *nodes[CPARSER_MAX_NUM_TOKENS+2];
} help_stack_t;

/**
 * \brief    Pre-order walker function used by cparser_help_cmd().
 * \details  Its main function is to push into the help stack when recurse into
 *           the next level.
 *
 * \param    parser Pointer to the parser structure.
 * \param    node   Pointer to the current parse tree node.
 * \param    cookie Pointer to the help stack.
 *
 * \return   Return CPARSER_OK always.
 */
static cparser_result_t
cparser_help_pre_walker (cparser_t *parser, cparser_node_t *node, void *cookie)
{
    help_stack_t *hs = (help_stack_t *)cookie;

    //assert(parser && node && hs);
    if (!(parser && node && hs))
    {
        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    hs->nodes[hs->tos] = node;
    hs->tos++;

    return CPARSER_OK;
}

/**
 * \brief    Post-order walker function used by cparser_help_cmd().
 * \details  Its main function is to print out a command description and to
 *           pop the help stack.
 *
 * \param    parser Pointer to the parser structure.
 * \param    node   Pointer to the current parse tree node.
 * \param    cookie Pointer to the help stack.
 *
 * \return   Return CPARSER_OK always.
 */
static cparser_result_t
cparser_help_post_walker (cparser_t *parser, cparser_node_t *node, void *cookie)
{
    help_stack_t *hs = (help_stack_t *)cookie;
    int n, do_print;

    //assert(parser && node && hs);
    if (!(parser && node && hs))
    {
        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    if ((CPARSER_NODE_END == node->type) && 
        (!(node->flags & CPARSER_NODE_FLAGS_OPT_PARTIAL))) {
        do_print = 0;
        if (hs->filter) {
            /* We have a filter string. Check if it matches any keyword */
            for (n = 0; n < hs->tos; n++) {
                if (CPARSER_NODE_KEYWORD != hs->nodes[n]->type) {
                    continue;
                }
                if (strstr(hs->nodes[n]->param, hs->filter)) {
                    do_print = 1; /* Yes, print it */
                    break;
                }
            }
        } else {
            do_print = 1;
        }
        if (do_print) {
            cparser_node_t *cur_node;

            parser->cfg.prints(parser, node->desc);
            parser->cfg.prints(parser, "\r\n  ");
            for (n = 0; n < hs->tos; n++) {
                cur_node = hs->nodes[n];
                if ((CPARSER_NODE_ROOT == cur_node->type) ||
                    (CPARSER_NODE_END == cur_node->type)) {
                    continue;
                }
                parser->cfg.prints(parser, cur_node->param);
                parser->cfg.printc(parser, ' ');
                if (cur_node->flags & CPARSER_NODE_FLAGS_OPT_START) {
                    parser->cfg.prints(parser, "{ ");
                }
                if (cur_node->flags & CPARSER_NODE_FLAGS_OPT_END) {
                    parser->cfg.prints(parser, "} ");
                }
            }
            parser->cfg.prints(parser, "\r\n\n");
        }
    }

    /* Pop the stack */
    hs->tos--;
    return CPARSER_OK;
}

cparser_result_t
cparser_help_cmd (cparser_t *parser, char *str)
{
    help_stack_t help_stack;

    //assert(parser);
    if (!(parser))
    {
        printf("Error! In file:%s line:%d function:%s\n", __FILE__, __LINE__, __FUNCTION__);
        return  CPARSER_NOT_OK;
    }
    memset(&help_stack, 0, sizeof(help_stack));
    help_stack.filter = str;
    return cparser_walk(parser, cparser_help_pre_walker, 
                        cparser_help_post_walker, &help_stack);
}

char *cparser_error_numToStr(int err_num)
{
    switch(err_num)
	{
    	case CPARSER_OK:
    	    return "OK";
    		
    	case CPARSER_NOT_OK:
    		return "Failed";

    	case CPARSER_ERR_INVALID_PARAMS:
    		return "Invalid input parameter";

    	case CPARSER_ERR_NOT_EXIST:
    		return "Requested object does not exist";

    	case CPARSER_ERR_OUT_OF_RES:
    		return "Requested resource is exhausted";

    	default:
    		return "Unknown error code";
	}
}
