/* SMUD - Slight MUD
** (c) 2000 - 2003, Karl Bastiman, Janet Hyde
**
** Title   : mccp.c
** Purpose : Mud compression v1&2 support
**
** Date     Ver  Who Why
** -------------------------------------------------------------
** 12-07-02 0.00 ksb First Version
*/

#include "smud.h"

char compress1_start[] = { IAC, SB, COMPRESS1, WILL, SE, '\0' };
char compress2_start[] = { IAC, SB, COMPRESS2, IAC, SE, '\0' };
char compress1_offer[] = { IAC, WILL, COMPRESS1, '\0' };
char compress2_offer[] = { IAC, WILL, COMPRESS2, '\0' };

short processCompressed(SU);

int  mccp_mem_usage = 0;
int  mccp_mem_freed = 0;

/* offer compression for the user */
void offerCompress(SU)
{
    write(UU.socket, compress2_offer, strlen(compress2_offer));
    write(UU.socket, compress1_offer, strlen(compress1_offer));
}
/*
 * Memory management - zlib uses these hooks to allocate and free memory
 * it needs
 */

void *zlib_alloc(void *opaque, unsigned int items, unsigned int size)
{
  /* memory tracking */
  mccp_mem_usage += size;

  /* return memory pointer */
  return calloc(items, size);
}

void zlib_free(void *opaque, void *address)
{
  /* memory tracking */
  mccp_mem_freed += sizeof(*address);

  /* free the cell */
  free(address);
}

/*
 * Begin compressing data on uid
 */
short compressStart(SU)
{
    z_stream *s;

    if (!ENABLE_MCCP) return FALSE;

    if (UU.out_compress) /* already compressing */
        return TRUE;

    /* allocate and init stream, buffer */
    s = (z_stream *)malloc(sizeof(*s));
    UU.out_compress_buf = (unsigned char *)malloc(COMPRESS_BUF_SIZE);

    s->next_in = NULL;
    s->avail_in = 0;

    s->next_out = UU.out_compress_buf;
    s->avail_out = COMPRESS_BUF_SIZE;
 
    s->zalloc = zlib_alloc;
    s->zfree  = zlib_free;
    s->opaque = NULL;

    if (deflateInit(s, COMP_LEVEL) != Z_OK) {
        /* problems with zlib, try to clean up */
        free(UU.out_compress_buf);
        free(s);
        return FALSE;
    }
    
    write(UU.socket, UU.mccp==1?compress1_start:compress2_start, strlen(compress1_start));

    /* now we're compressing */
    UU.out_compress=s;
    return TRUE;
}

/* Cleanly shut down compression on uid */
short compressEnd(SU)
{
    unsigned char dummy[1];

    if (!UU.out_compress)
        return TRUE;

    UU.out_compress->avail_in = 0;
    UU.out_compress->next_in = dummy;
    
    /* No terminating signature is needed - receiver will get Z_STREAM_END */

    if (deflate(UU.out_compress, Z_FINISH) != Z_STREAM_END)
        return FALSE;
    
    if (!processCompressed(uid)) /* try to send any residual data */
        return FALSE;

    deflateEnd(UU.out_compress); 
    free(UU.out_compress_buf);
    free(UU.out_compress);
    UU.out_compress = NULL;
    UU.out_compress_buf = NULL;

    return TRUE;
}

short compressEnd2(SU)
{
    unsigned char dummy[1];

    if (!UU.out_compress)
        return TRUE;
  
    UU.out_compress->avail_in = 0;
    UU.out_compress->next_in = dummy;
    
    /* No terminating signature is needed - receiver will get Z_STREAM_END */
    
    if (deflate(UU.out_compress, Z_FINISH) != Z_STREAM_END)
        return FALSE;

    if (!processCompressed(uid)) /* try to send any residual data */
        return FALSE;
    
    deflateEnd(UU.out_compress);
    UU.out_compress = NULL;
    UU.out_compress_buf = NULL;   

    return TRUE;
}


/* Try to send any pending compressed-but-not-sent data in `desc' */
short processCompressed(SU)
{
    int iStart, nBlock, nWrite, len;
 
    if (!UU.out_compress)
        return FALSE;
 
    /* Try to write out some data.. */
    len = UU.out_compress->next_out - UU.out_compress_buf;
    if (len > 0) {
        /* we have some data to write */

        for (iStart = 0; iStart < len; iStart += nWrite)
        {
            nBlock = len - iStart;
            if ((nWrite = write (UU.socket, UU.out_compress_buf + iStart, nBlock)) < 0)
            {
#ifdef LINUX
                if (errno == EAGAIN ||
                    errno == ENOSR)
#else
		if (errno == EAGAIN)
#endif
			break;
		
               	return FALSE; /* write error */
            }
    
            if (nWrite <= 0)
                break;
        }
    
	UU.compout+=iStart;
        if (iStart) {
            /* We wrote "iStart" bytes */
            if (iStart < len)
                memmove(UU.out_compress_buf, UU.out_compress_buf+iStart, len - iStart);

            UU.out_compress->next_out = UU.out_compress_buf + len - iStart;
        }
    }
 
    return TRUE;
}
 
/* write, the compressed case */
short writeCompressed(SU, char *txt, int length)
{
    z_stream *s = UU.out_compress;   

    s->next_in = (unsigned char *)txt;
    s->avail_in = length;
    
    while (s->avail_in) {
        s->avail_out = COMPRESS_BUF_SIZE - (s->next_out - UU.out_compress_buf);
                
        if (s->avail_out) {
            int status = deflate(s, Z_SYNC_FLUSH);
    
            if (status != Z_OK) {
                /* Boom */
                return FALSE;
            }
        }
         
        /* Try to write out some data.. */
        if (!processCompressed(uid))
            return FALSE;
    }
            
    /* Done. */
    return TRUE;
}
    
