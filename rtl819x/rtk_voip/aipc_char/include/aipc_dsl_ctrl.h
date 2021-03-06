#ifndef __AIPC_DSL_CTRL_H__
#define __AIPC_DSL_CTRL_H__

#include "aipc_shm.h"

/*****************************************************************************
*   Macro Definitions
*****************************************************************************/

/*****************************************************************************
*   Data Structure
*****************************************************************************/

typedef struct {
	unsigned char data[ DSL_CTRL_BUF_SIZE ];
} aipc_dsl_ctrl_set_buf_t;

typedef struct {
	unsigned char data[ DSL_CTRL_BUF_SIZE ];
} aipc_dsl_ctrl_get_buf_t;

/*****************************************************************************
*   Export Function
*****************************************************************************/

/*****************************************************************************
*   Function
*****************************************************************************/

/*****************************************************************************
*   External Function
*****************************************************************************/

/*****************************************************************************
*   Debug Function
*****************************************************************************/

#endif

