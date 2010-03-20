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
 
#ifndef __OGG_SKELETON_QUERY_H__
#define __OGG_SKELETON_QUERY_H__

OggSkeletonError oggskeleton_get_ptime_num (const OggSkeleton *skeleton, ogg_int64_t *pres_num);
OggSkeletonError oggskeleton_set_ptime_num (OggSkeleton *skeleton, ogg_int64_t pres_num);

OggSkeletonError oggskeleton_get_ptime_denum (const OggSkeleton *skeleton, ogg_int64_t *pres_denum);
OggSkeletonError oggskeleton_set_ptime_denum (OggSkeleton *skeleton, ogg_int64_t pres_denum);

OggSkeletonError oggskeleton_get_btime_num (const OggSkeleton *skeleton, ogg_int64_t *btime_num);
OggSkeletonError oggskeleton_set_btime_num (OggSkeleton *skeleton, ogg_int64_t btime_num);

OggSkeletonError oggskeleton_get_btime_denum (const OggSkeleton *skeleton, ogg_int64_t *btime_denum);
OggSkeletonError oggskeleton_set_btime_denum (OggSkeleton *skeleton, ogg_int64_t btime_denum);

OggSkeletonError oggskeleton_get_utc (const OggSkeleton *skeleton, char *UTC);
OggSkeletonError oggskeleton_set_utc (OggSkeleton *skeleton, const char *UTC);

OggSkeletonError oggskeleton_get_num_headers (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_uint32_t *num_headers);
OggSkeletonError oggskeleton_set_num_headers (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_uint32_t num_headers);

OggSkeletonError oggskeleton_get_granule_num (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t *granule_num);
OggSkeletonError oggskeleton_set_granule_num (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t granule_num);

OggSkeletonError oggskeleton_get_granule_denum (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t *granule_denum);
OggSkeletonError oggskeleton_set_granule_denum (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t granule_denum);

OggSkeletonError oggskeleton_get_start_granule (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t *start_granule);
OggSkeletonError oggskeleton_set_start_granule (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_int64_t start_granule);

OggSkeletonError oggskeleton_get_preroll (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_uint32_t *preroll);
OggSkeletonError oggskeleton_set_preroll (OggSkeleton *skeleton, ogg_uint32_t serial_no, ogg_uint32_t preroll);


#endif /* __OGG_SKELETON_QUERY_H__  */
