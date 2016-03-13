
/*!
 ************************************************************************
 * \file  nalucommon.c
 *
 * \brief
 *    Common NALU support functions
 *
 * \author
 *    Main contributors (see contributors.h for copyright, address and affiliation details)
 *    - Stephan Wenger   <stewe@cs.tu-berlin.de>
 ************************************************************************
 */

#include <stdio.h>
#include <assert.h>
#include <malloc.h>

#include "global.h"
#include "memory.h"
#include "nalu.h"
#include "memalloc.h"


/*! 
 *************************************************************************************
 * \brief
 *    Allocates memory for a NALU
 *
 * \param buffersize
 *     size of NALU buffer 
 *
 * \return
 *    pointer to a NALU
 *************************************************************************************
 */
 

NALU_t *AllocNALU(int buffersize)
{
  NALU_t *n;
  //����һ������ָ��NALU�ĵ�Ԫ
  if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL) no_mem_exit ("AllocNALU: n");
  //��Ҫ����buffersize���ֽ���
  n->max_size=buffersize;
  //����NALU_t�ڵ�buf��Ӧ��buffersize��С�ֽ�
  if ((n->buf = (byte*)calloc (buffersize, sizeof (byte))) == NULL) no_mem_exit ("AllocNALU: n->buf");
  
  return n;
}


/*! 
 *************************************************************************************
 * \brief
 *    Frees a NALU
 *
 * \param n 
 *    NALU to be freed
 *
 *************************************************************************************
 */

void FreeNALU(NALU_t *n)
{
  if (n)
  {
    if (n->buf)
    {
      free(n->buf);
      n->buf=NULL;
    }
    free (n);
  }
}

