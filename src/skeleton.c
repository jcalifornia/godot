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
#include "skeleton_vector.h"

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

static inline void write16le (unsigned char *data_out, ogg_uint16_t i)
{
    data_out[0]=i&0xff;
    data_out[1]=(i>>8)&0xff;
}

static inline void write32le (unsigned char *data_out, ogg_uint32_t i)
{
    data_out[0]=i&0xff;
    data_out[1]=(i>>8)&0xff;
    data_out[2]=(i>>16)&0xff;
    data_out[3]=(i>>24)&0xff;
}

static inline void write64le (unsigned char *data_out, ogg_int64_t i)
{
    ogg_uint32_t hi=i>>32;
    data_out[0]=i&0xff;
    data_out[1]=(i>>8)&0xff;
    data_out[2]=(i>>16)&0xff;
    data_out[3]=(i>>24)&0xff;
    data_out[4]=hi&0xff;
    data_out[5]=(hi>>8)&0xff;
    data_out[6]=(hi>>16)&0xff;
    data_out[7]=(hi>>24)&0xff;
}


static inline unsigned char*
read_var_length (unsigned char* p, ogg_int64_t* num) {
  int shift        = 0;
  ogg_int64_t byte = 0;

  *num = 0;
  do {
    byte = (ogg_int64_t)(*p);
    *num |= ((byte & 0x7f) << shift);
    shift += 7;
    p++;
  } while ((byte & 0x80) != 0x80);
  return p;
}


OggSkeleton* oggskel_new () 
{
  OggSkeleton* skeleton = NULL;
  
  skeleton = _ogg_calloc (1, sizeof (OggSkeleton));
  if (skeleton == NULL) 
  {
    return NULL;
  }
  
  skeleton->track_vect = oggskel_vect_new ();
  if (skeleton->track_vect == NULL)
  {
    _ogg_free (skeleton);
    return NULL;
  }
  
  skeleton->indexing  = 0;
  skeleton->finished  = 0;
  
  return skeleton;
}

OggSkeletonError oggskel_destroy (OggSkeleton* skeleton) 
{
  if (skeleton == NULL) 
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  oggskel_vect_destroy (skeleton->track_vect);
  _ogg_free (skeleton);

  return SKELETON_ERR_OK;
}

static int encode_fishead (const OggSkeleton *skeleton, ogg_packet *op)
{  
  size_t      fishead_size = 0;

  if (skeleton == NULL) 
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (op == NULL)
  {
    return SKELETON_ERR_GENERIC;
  }

  if (skeleton->indexing)
  {
    /* indexing is enabled */
//    skeleton->fishead.ver_maj = SKELETON_VERSION_MAJOR;
//    skeleton->fishead.ver_min = SKELETON_VERSION_MINOR;
    fishead_size = FISHEAD_3_2_SIZE;
  }
  else
  {
    /* indexing is not enabled thus creating version 3.0 skeleton bitstream */
//    skeleton->fishead.ver_maj = SKELETON_VERSION_MAJOR;
//    skeleton->fishead.ver_min = 0;
    fishead_size = FISHEAD_3_0_SIZE;
  }

  op->packet = _ogg_calloc (fishead_size, sizeof (unsigned char));
  
  if (op->packet  == NULL)
  {
    return SKELETON_ERR_OUT_OF_MEMORY;
  }
  
  memcpy (op->packet, FISHEAD_MAGIC, FISHEAD_MAGIC_LEN);
  write16le (op->packet+FISHEAD_MAGIC_LEN, skeleton->fishead.ver_maj);
  write16le (op->packet+10, skeleton->fishead.ver_min);
  write64le (op->packet+12, skeleton->fishead.ptime_num);
  write64le (op->packet+20, skeleton->fishead.ptime_denum);
  write64le (op->packet+28, skeleton->fishead.btime_num);
  write64le (op->packet+36, skeleton->fishead.btime_denum);
  memcpy (op->packet+44, skeleton->fishead.UTC, sizeof (skeleton->fishead.UTC));

  if (skeleton->indexing)
  {
    write64le (op->packet+64, skeleton->fishead.first_sample_num);
    write64le (op->packet+72, skeleton->fishead.first_sample_denum);
    write64le (op->packet+80, skeleton->fishead.last_sample_num);
    write64le (op->packet+88, skeleton->fishead.last_sample_denum);
    write64le (op->packet+96, skeleton->fishead.segment_len);
    write64le (op->packet+104, skeleton->fishead.nh_offset);
  }

  op->b_o_s       = 1;
  op->e_o_s       = 0;
  op->bytes       = fishead_size;
  op->granulepos  = 0;
  op->packetno    = 0;
  
  return 1;
}

static int encode_fisbone (const FisBone *fisbone, ogg_packet *op)
{
  size_t bone_size = FISBONE_MAGIC_LEN;
  
  if (fisbone == NULL) 
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (op == NULL)
  {
    return SKELETON_ERR_GENERIC;
  }
  
  bone_size += sizeof (*fisbone);
  if (bone_size < FISBONE_SIZE)
  {
    return -1;
  }
  
  op->packet = _ogg_calloc (1, bone_size);
  
  memcpy (op->packet, FISBONE_MAGIC, FISBONE_MAGIC_LEN);
  write32le (op->packet + FISBONE_MAGIC_LEN, fisbone->msg_header_offset);
  write32le (op->packet + 12, fisbone->serial_no);
  write32le (op->packet + 16, fisbone->num_headers);
  write64le (op->packet + 20, fisbone->granule_num);
  write64le (op->packet + 28, fisbone->granule_denum);
  write64le (op->packet + 36, fisbone->start_granule);
  write32le (op->packet + 44, fisbone->preroll);
  *(op->packet + 48) = fisbone->granule_shift;
  memcpy (op->packet+FISBONE_SIZE, fisbone->msg_fields, bone_size - FISBONE_SIZE);
  
  op->b_o_s       = 0;
  op->e_o_s       = 0;
  op->bytes       = bone_size;
  op->granulepos  = 0;
  op->packetno    = 0;

  return 1;
}

static int encode_skeleton_eos (ogg_packet *op)
{
  if (op == NULL)
  {
    return SKELETON_ERR_GENERIC;
  }

  op->b_o_s       = 0;
  op->e_o_s       = 1;
  op->bytes       = 0;
  op->granulepos  = 0;
  op->packetno    = 0;
  
  return 1;
}


int oggskel_encode_header (OggSkeleton* skeleton, ogg_packet *op)
{
  int ret = SKELETON_ERR_GENERIC;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;    
  }
  
  if (op == NULL)
  {
    return SKELETON_ERR_GENERIC;
  }
  
  ret = encode_fishead (skeleton, op);
//  ret = encode_fisbone (, op);
  /*
  switch (skeleton_enc_state)
  {
    case :
    
    default:
    
  }
  */
  
  /* no packet was produced + no more packets remain */
  return ret;
}


static int decode_fishead (OggSkeleton *skeleton, 
                           const ogg_packet *op) 
{
  ogg_uint32_t version;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (op == NULL)
  {
    return SKELETON_ERR_BAD_PACKET;
  }
  
  skeleton->fishead.ver_maj     = extract_int16 (op->packet + 8);
  skeleton->fishead.ver_min     = extract_int16 (op->packet + 10); 

  version = SKELETON_VERSION (skeleton->fishead.ver_maj, 
                              skeleton->fishead.ver_min);

  /* check whether we can decode this skeleton bitstream */
  if (version > SKELETON_VERSION(SKELETON_VERSION_MAJOR, SKELETON_VERSION_MINOR))
  {
    return SKELETON_ERR_UNSUPPORTED_VERSION;
  }

  /* ensure that the ogg packet's size is either 64 or 112 bytes! */
  if ((version >= SKELETON_VERSION(3,2)) && (op->bytes != FISHEAD_3_2_SIZE))
  {
    return -1;
  }
  else if ((version == SKELETON_VERSION(3,0)) && (op->bytes != FISHEAD_3_0_SIZE))
  {
    return -1;
  }
    
  skeleton->fishead.ptime_num   = extract_int64 (op->packet + 12);
  skeleton->fishead.ptime_denum = extract_int64 (op->packet + 20); 
  skeleton->fishead.btime_num   = extract_int64 (op->packet + 28);
  skeleton->fishead.btime_denum = extract_int64 (op->packet + 36);
  memcpy (skeleton->fishead.UTC, op->packet+44, sizeof (skeleton->fishead.UTC));
    
  if (version >= SKELETON_VERSION(3,2)) 
  {
    skeleton->fishead.first_sample_num      = extract_int64 (op->packet + 64);
    skeleton->fishead.first_sample_denum    = extract_int64 (op->packet + 72);
    skeleton->fishead.last_sample_num       = extract_int64 (op->packet + 80);
    skeleton->fishead.last_sample_denum     = extract_int64 (op->packet + 88);
    skeleton->fishead.segment_len           = extract_int64 (op->packet + 96);
    skeleton->fishead.nh_offset             = extract_int64 (op->packet + 104);
  }

  return 1;
}

static int decode_fisbone (OggSkeleton *skeleton, 
                           const ogg_packet *op) 
{
  FisBone * current_bone;
  int       ret = -1;    
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (op == NULL)
  {
    return SKELETON_ERR_BAD_PACKET;
  }
  
  if (op->bytes < FISBONE_SIZE)
  {
    return SKELETON_ERR_BAD_FISBONE;
  }
  
  current_bone = _ogg_calloc (1, op->bytes);
  if (current_bone == NULL)
  {
    return SKELETON_ERR_OUT_OF_MEMORY;
  }
  
  current_bone->msg_header_offset   = extract_int32 (op->packet + 8);
  current_bone->serial_no           = extract_int32 (op->packet + 12);
  current_bone->num_headers         = extract_int32 (op->packet + 16);
  current_bone->granule_num         = extract_int64 (op->packet + 20);
  current_bone->granule_denum       = extract_int64 (op->packet + 28);
  current_bone->start_granule       = extract_int64 (op->packet + 36);
  current_bone->preroll             = extract_int32 (op->packet + 44);
  current_bone->granule_shift       = *(op->packet + 48);
  memcpy (current_bone->msg_fields, op->packet + FISBONE_SIZE, (op->bytes - FISBONE_SIZE));

  ret = 
    oggskel_vect_add_bone (skeleton->track_vect, current_bone, 
                           current_bone->serial_no);
  
  if (ret < 0)
    _ogg_free (current_bone);
    
  return ret;
}

static int decode_index (OggSkeleton* skeleton, const ogg_packet *op)
{
  ogg_int64_t i = 0, offset = 0, time = 0, time_mult = 1000;
  Index * current_index       = NULL;
  ogg_int64_t min_packet_size = -1;
  ogg_int64_t max_packet_size = -1;
  unsigned char *p            = NULL;
  int ret = -1;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  if (op == NULL)
  {
    return SKELETON_ERR_BAD_PACKET;
  }
  
  current_index = _ogg_calloc (1, sizeof (Index));
  if (current_index == NULL)
  {
    return SKELETON_ERR_OUT_OF_MEMORY;
  }
  
  current_index->serial_no   = extract_int32 (op->packet + 6);
  current_index->num_keys    = extract_int64 (op->packet + 10);
  current_index->ptime_denum = extract_int64 (op->packet + 18);  
  
  /* check whether presentation time denumerator is 0 */
  if (current_index->ptime_denum == 0)
  {
    ret = SKELETON_ERR_DENUM_ZERO;
    goto fin;
  }
  
  min_packet_size = 
    INDEX_KEYPOINT_OFFSET + current_index->num_keys * MIN_KEY_POINT_SIZE;
  if (min_packet_size > op->bytes)
  {
    ret = SKELETON_ERR_MALICIOUS_INDEX;
    goto fin;
  }

/*
  max_packet_size = 
    INDEX_KEYPOINT_OFFSET + current_index->num_keys * MAX_KEY_POINT_SIZE;
  if (max_packet_size > )
  {
    return SKELETON_ERR_;
  }
*/
  current_index->keypoints = 
    _ogg_calloc (current_index->num_keys, sizeof (KeyFrameInfo));

  if (current_index->keypoints == NULL)
  {
    ret = SKELETON_ERR_OUT_OF_MEMORY;
    goto fin;
  }
  
  p = op->packet + INDEX_KEYPOINT_OFFSET;
  /* read in keypoints */
  for (i = 0; i < current_index->num_keys; ++i)
  {
    ogg_int64_t offset_delta = 0, time_delta = 0;
    
    p = read_var_length (p, &offset_delta);
    offset += offset_delta;

    p = read_var_length (p, &time_delta);
    time += time_delta;
    
    /* store (offset, time_ms) pair in keypoints array */
    current_index->keypoints[i].offset = offset;
    current_index->keypoints[i].time_ms = 
        (time * time_mult) / current_index->ptime_denum;
  }
  
  /* add index structure to the trackInfo struct */
  ret = 
    oggskel_vect_add_index (skeleton->track_vect, current_index, 
                            current_index->serial_no);

fin:  
  if (ret < 0)
    _ogg_free (current_index);
  
  return ret;
}

int oggskel_decode_header (OggSkeleton* skeleton, const ogg_packet* op)
{
  int ret = -1;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  if (op == NULL)
  {
    return SKELETON_ERR_BAD_PACKET;
  }
  
  /* we've got the skeleton EOS packet - presumably */
  if (op->e_o_s)
  {
    skeleton->finished = 1;
   
    /* 
     * EOS packet does not comply with the Skeleton specification:
     * "The skeleton eos page MUST contain one packet of length zero"
     */
    if (op->bytes != 0)
    {
      return SKELETON_WARN_EOS_NOT_EMTPY;
    }
    
    return SKELETON_ERR_OK;
  }
  
  /* identify the type of skeleton packet */
  if (memcmp (op->packet, FISHEAD_MAGIC, FISHEAD_MAGIC_LEN) == 0) 
  { 
    /* assure that the fishead packet is BOS */
    ret = decode_fishead (skeleton, op);
    if (ret == 1 && !op->b_o_s)
    {
      ret = SKELETON_WARN_FISHEAD_NOT_BOS;
    }
  }
  else if (memcmp (op->packet, FISBONE_MAGIC, FISBONE_MAGIC_LEN) == 0)
  {
    ret = decode_fisbone (skeleton, op);
  }
  else if (memcmp (op->packet, INDEX_MAGIC, INDEX_MAGIC_LEN) == 0)
  {
    skeleton->indexing = 1;
    ret = decode_index (skeleton, op);
  }
  else
  {
    /* unknown packet type */
    ret = -1;
  }

  return ret;
}
