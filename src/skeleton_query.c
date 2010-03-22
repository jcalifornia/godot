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
 
#include <stdlib.h>
#include <string.h>

#include "skeleton_private.h"
#include "skeleton_vector.h"

OggSkeletonError 
oggskel_get_ver_maj (const OggSkeleton *skeleton, ogg_uint16_t *ver_maj)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (skeleton->finished == 0)
  {
    return SKELETON_ERR_EOS_AWAITING;
  }
  
  *ver_maj = skeleton->fishead.ver_maj;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_ver_min (const OggSkeleton *skeleton, ogg_uint16_t *ver_min)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (skeleton->finished == 0)
  {
    return SKELETON_ERR_EOS_AWAITING;
  }
  
  *ver_min = skeleton->fishead.ver_min;
  
  return SKELETON_ERR_OK;  
}



OggSkeletonError 
oggskel_get_ptime_num (const OggSkeleton *skeleton, ogg_int64_t *pres_num)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (skeleton->finished == 0)
  {
    return SKELETON_ERR_EOS_AWAITING;
  }
  
  *pres_num = skeleton->fishead.ptime_num;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_set_ptime_num (OggSkeleton *skeleton, ogg_int64_t pres_num)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  skeleton->fishead.ptime_num = pres_num;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_ptime_denum (const OggSkeleton *skeleton, ogg_int64_t *pres_denum)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (skeleton->finished == 0)
  {
    return SKELETON_ERR_EOS_AWAITING;
  }

  *pres_denum = skeleton->fishead.ptime_denum;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_set_ptime_denum (OggSkeleton *skeleton, ogg_int64_t pres_denum)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  skeleton->fishead.ptime_denum = pres_denum;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_btime_num (const OggSkeleton *skeleton, ogg_int64_t *btime_num)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  if (skeleton->finished == 0)
  {
    return SKELETON_ERR_EOS_AWAITING;
  }

  *btime_num = skeleton->fishead.btime_num;

  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_set_btime_num (OggSkeleton *skeleton, ogg_int64_t btime_num)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  skeleton->fishead.btime_num = btime_num;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_btime_denum (const OggSkeleton *skeleton, ogg_int64_t *btime_denum)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (skeleton->finished == 0)
  {
    return SKELETON_ERR_EOS_AWAITING;
  }
  
  *btime_denum = skeleton->fishead.btime_denum;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_set_btime_denum (OggSkeleton *skeleton, ogg_int64_t btime_denum)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  skeleton->fishead.btime_denum = btime_denum;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_utc (const OggSkeleton *skeleton, char **UTC)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  if (skeleton->finished == 0)
  {
    return SKELETON_ERR_EOS_AWAITING;
  }

  *UTC = _ogg_calloc (20, sizeof (char));
  if (*UTC == NULL)
  {
    return SKELETON_ERR_OUT_OF_MEMORY;
  }
  
  memcpy (*UTC, skeleton->fishead.UTC, 20);

  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_set_utc (OggSkeleton *skeleton, const char *UTC)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (strlen (UTC) != 20) 
  {
    return -1;
  }
  
  memcpy (skeleton->fishead.UTC, UTC, 20);
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_granule_shift (const OggSkeleton *skeleton, ogg_uint32_t serial_no, unsigned char *granule_shift)
{
  FisBone *bone = NULL;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if ((bone = oggskel_vect_get_bone (skeleton->track_vect, serial_no)) == NULL)
  {
    return SKELETON_ERR_BAD_SERIAL_NO;
  }
  
  *granule_shift = bone->granule_shift;
  return SKELETON_ERR_OK;  
}

OggSkeletonError 
oggskel_set_granule_shift (OggSkeleton *skeleton, ogg_uint32_t serial_no, unsigned char granule_shift)
{
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_num_headers (const OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_uint32_t *num_headers)
{
  FisBone *bone = NULL;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if ((bone = oggskel_vect_get_bone (skeleton->track_vect, serial_no)) == NULL)
  {
    return SKELETON_ERR_BAD_SERIAL_NO;
  }
  
  *num_headers = bone->num_headers;
  
  return SKELETON_ERR_OK; 
}

OggSkeletonError 
oggskel_set_num_headers (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_uint32_t num_headers)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_granule_num (const OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t *granule_num)
{
  FisBone *bone = NULL;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if ((bone = oggskel_vect_get_bone (skeleton->track_vect, serial_no)) == NULL)
  {
    return SKELETON_ERR_BAD_SERIAL_NO;
  }
  
  *granule_num = bone->granule_num;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_set_granule_num (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t granule_num)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_granule_denum (const OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t *granule_denum)
{
  FisBone *bone;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if ((bone = oggskel_vect_get_bone (skeleton->track_vect, serial_no)) == NULL)
  {
    return SKELETON_ERR_BAD_SERIAL_NO;
  }
  
  *granule_denum = bone->granule_denum;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_set_granule_denum (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t granule_denum)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_start_granule (const OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t *start_granule)
{
  FisBone *bone = NULL;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if ((bone = oggskel_vect_get_bone (skeleton->track_vect, serial_no)) == NULL)
  {
    return SKELETON_ERR_BAD_SERIAL_NO;
  }
  
  *start_granule = bone->start_granule;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_set_start_granule (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t start_granule)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_preroll (const OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_uint32_t *preroll)
{
  FisBone *bone = NULL;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if ((bone = oggskel_vect_get_bone (skeleton->track_vect, serial_no)) == NULL)
  {
    return SKELETON_ERR_BAD_SERIAL_NO;
  }
  
  *preroll = bone->preroll;
  
  return SKELETON_ERR_OK;  
}

OggSkeletonError 
oggskel_set_preroll (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_uint32_t preroll)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  return SKELETON_ERR_OK;
}

OggSkeletonError 
oggskel_get_msg_header (const OggSkeleton *skeleton, ogg_uint32_t serial_no, char **msg_header)
{
  FisBone *bone        = NULL;
  size_t   fields_size = 0;
  
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if ((bone = oggskel_vect_get_bone (skeleton->track_vect, serial_no)) == NULL)
  {
    return SKELETON_ERR_BAD_SERIAL_NO;
  }
  
  fields_size = strlen (bone->msg_fields);
  *msg_header = _ogg_calloc (fields_size, sizeof (char));
  if (*msg_header == NULL)
    return SKELETON_ERR_OUT_OF_MEMORY;
  
  memcpy (*msg_header, bone->msg_fields, fields_size);
  
  return SKELETON_ERR_OK; 
}

OggSkeletonError 
oggskel_set_msg_header (OggSkeleton *skeleton, ogg_uint32_t serial_no, char *msg_header)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  return SKELETON_ERR_OK;
}

