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
 
#include <string.h>

#include "skeleton_private.h"

OggSkeletonError 
oggskeleton_get_ptime_num (const OggSkeleton *skeleton, ogg_int64_t *pres_num)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  *pres_num = skeleton->fishead.ptime_num;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError oggskeleton_set_ptime_num (OggSkeleton *skeleton, ogg_int64_t pres_num)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  skeleton->fishead.ptime_num = pres_num;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError oggskeleton_get_ptime_denum (const OggSkeleton *skeleton, ogg_int64_t *pres_denum)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  *pres_denum = skeleton->fishead.ptime_denum;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError oggskeleton_set_ptime_denum (OggSkeleton *skeleton, ogg_int64_t pres_denum)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  skeleton->fishead.ptime_denum = pres_denum;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError oggskeleton_get_btime_num (const OggSkeleton *skeleton, ogg_int64_t *btime_num)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  *btime_num = skeleton->fishead.btime_num;

  return SKELETON_ERR_OK;
}

OggSkeletonError oggskeleton_set_btime_num (OggSkeleton *skeleton, ogg_int64_t btime_num)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  skeleton->fishead.btime_num = btime_num;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError oggskeleton_get_btime_denum (const OggSkeleton *skeleton, ogg_int64_t *btime_denum)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  *btime_denum = skeleton->fishead.btime_denum;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError oggskeleton_set_btime_denum (OggSkeleton *skeleton, ogg_int64_t btime_denum)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  skeleton->fishead.btime_denum = btime_denum;
  
  return SKELETON_ERR_OK;
}

OggSkeletonError oggskeleton_get_utc (const OggSkeleton *skeleton, char *UTC)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }

  if ((UTC = strdup (skeleton->fishead.UTC)) == NULL)
  {
    return SKELETON_ERR_OUT_OF_MEMORY;
  }

  return SKELETON_ERR_OK;
}

OggSkeletonError oggskeleton_set_utc (OggSkeleton *skeleton, const char *UTC)
{
  if (skeleton == NULL)
  {
    return SKELETON_ERR_BAD_SKELETON;
  }
  
  if (strlen (UTC) < 20) 
  {
    return -1;
  }
  memcpy (skeleton->fishead.UTC, UTC, 20);
  return SKELETON_ERR_OK;
}
