/**
   Copyright (c) 2010, Xiph.org Foundation

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:

   - Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.

   - Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

   - Neither the name of the Xiph.org Foundation nor the names of its
   contributors may be used to endorse or promote products derived from
   this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION
   OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <assert.h>
#include <stdlib.h> 
#include <string.h>

#include "skeleton_private.h"
#include "skeleton_macros.h"

static inline unsigned short extract_int16 (unsigned char *data)
{
  return data[0] | data[1] <<8;
}

static inline unsigned long extract_int32(unsigned char *data) 
{
 return data[0] | (data[1] << 8) | (data[2] << 16) | (data[3] << 24);
}

static inline ogg_int64_t extract_int64(unsigned char *data) 
{
 return ((ogg_int64_t)(extract_int32(data))) |
        (((ogg_int64_t)(extract_int32(data + 4))) << 32);
}

OggSkeleton* oggskeleton_new () 
{
  OggSkeleton* skeleton = NULL;
  
  skeleton = _ogg_calloc (1, sizeof (OggSkeleton));
  if (skeleton == NULL) 
  {
    return NULL;
  }
  /*
  skeleton->fishead.version_major         =;
  skeleton->fishead.version_minor         =; 
  skeleton->fishead.pres_num              =;
  skeleton->fishead.pres_denum            =; 
  skeleton->fishead.base_num              =;
  skeleton->fishead.base_denum            =;
  skeleton->fishead.first_sample_num      =;
  skeleton->fishead.first_sample_denum    =;
  skeleton->fishead.last_sample_num       =;
  skeleton->fishead.last_sample_denum     =;
  skeleton->fishead.segment_len           =;
  skeleton->fishead.non_header_offset     =;
  skeleton->fishead.non_header_byte_offset=;
  */
  skeleton->num_bones = 0;
  
  return skeleton;
}

OggSkeletonError oggskeleton_destroy (OggSkeleton* skeleton) 
{
  int i;
  if (skeleton == NULL) 
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  for (i = 0; i < skeleton->num_bones; ++i)
  {
    free (skeleton->fisbones[i]);
  }
  
  free (skeleton);

  return SKELETON_ERR_OK;
}


static int decode_fishead (OggSkeleton *skeleton, 
                          const ogg_packet *op) 
{
  ogg_uint32_t version;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  skeleton->fishead.ver_maj     = extract_int16 (op->packet + 8);
  skeleton->fishead.ver_min     = extract_int16 (op->packet + 10); 
  skeleton->fishead.ptime_num   = extract_int64 (op->packet + 12);
  skeleton->fishead.ptime_denum = extract_int64 (op->packet + 20); 
  skeleton->fishead.btime_num   = extract_int64 (op->packet + 28);
  skeleton->fishead.btime_denum = extract_int64 (op->packet + 36);
  
  version = SKELETON_VERSION (skeleton->fishead.ver_maj, 
                              skeleton->fishead.ver_min);
  
  if (version >= SKELETON_VERSION(3,2)) 
  {
    skeleton->fishead.first_sample_num      = extract_int64 (op->packet + 42);
    skeleton->fishead.first_sample_denum    = extract_int64 (op->packet + 50);
    skeleton->fishead.last_sample_num       = extract_int64 (op->packet + 58);
    skeleton->fishead.last_sample_denum     = extract_int64 (op->packet + 64);
    skeleton->fishead.segment_len           = extract_int64 (op->packet + 72);
    skeleton->fishead.nh_offset             = extract_int64 (op->packet + 80);
    skeleton->fishead.nh_byte_offset        = extract_int64 (op->packet + 88);
  }
  
  return 1;
}

static int decode_fisbone (OggSkeleton *skeleton, 
                           const ogg_packet *op) 
{
  FisBone * current_bone;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  /* allocate fisbone */
  skeleton->fisbones = 
    _ogg_realloc (skeleton->fisbones, sizeof (FisBone*)*(++skeleton->num_bones));
    
  if (skeleton->fisbones == NULL) 
  {
    return SKELETON_ERR_OUT_OF_MEMORY;
  }
  
  current_bone = _ogg_calloc (sizeof(FisBone), 1);
  if (current_bone == NULL)
  {
    return SKELETON_ERR_OUT_OF_MEMORY;
  }
/*
  current_bone->msg_header_offset   = extract_int32 (op->packet + 8);
  current_bone->serial_no           = extract_int32 (op->packet + 12);
  current_bone->num_headers         = extract_int32 (op->packet + 16);
  current_bone->granule_num         = extract_int64 (op->packet + 20);
  current_bone->granule_denum       = extract_int64 (op->packet + 28);
  current_bone->start_granule       = extract_int64 (op->packet + 36);
  current_bone->preroll             = extract_int32 (op->packet + 44);
*/
  
  memcpy (current_bone, (op->packet+8), sizeof(FisBone));
  
  skeleton->fisbones[skeleton->num_bones - 1] = current_bone;
  
  return 1;
}

static int decode_index (OggSkeleton* skeleton, const ogg_packet *op)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  return 1;
}

int oggskeleton_decode_header (OggSkeleton* skeleton, const ogg_packet* op)
{
  int ret = 1;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  /* identify the type of skeleton packet */
  if (memcmp (op->packet, FISHEAD_MAGIC, FISHEAD_MAGIC_LEN) == 0) 
  { 
    /* assure that the fishead packet is BOS */
    assert (op->b_o_s);
    ret = decode_fishead (skeleton, op);
  }
  else if (memcmp (op->packet, FISBONE_MAGIC, FISBONE_MAGIC_LEN) == 0)
  {
    
    ret = decode_fisbone (skeleton, op);
  }
  else if (memcmp (op->packet, INDEX_MAGIC, INDEX_MAGIC_LEN) == 0)
  {
    ret = decode_index (skeleton, op);
  }
  else
  {
    /* unknown packet type */
    ret = -1;
  }
  
  /* if we found a skeleton header and the packet marks EndOfStream return 0 */
  if ((ret == 1) && op->e_o_s) ret = 0;
  
  return ret;
}

int oggskeleton_decode_index (OggSkeleton* skeleton, const ogg_packet* op)
{
  
  return 1;
}