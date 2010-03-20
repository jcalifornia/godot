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
 
#include <stdio.h>
#include <ogg/ogg.h>
#include <skeleton/skeleton.h>

/* grab some data from the container */
int buffer_data(FILE *in,ogg_sync_state *oy){
  char *buffer  = ogg_sync_buffer (oy, 4096);
  int bytes     = fread (buffer, 1, 4096, in);
  ogg_sync_wrote (oy,bytes);
  return (bytes);
}

void print_fishead (const OggSkeleton *skeleton)
{
  ogg_int64_t t;
  char *UTC;

  printf ("Skeleton fishead header content:\n");  
  oggskeleton_get_ptime_num (skeleton, &t);
  printf ("\tPresentation time numerator: %lld\n", t);
  oggskeleton_get_ptime_denum (skeleton, &t);  
  printf ("\tPresentation time denumerator: %lld\n", t);
  oggskeleton_get_btime_num (skeleton, &t);  
  printf ("\tBase time numerator: %lld\n", t);
  oggskeleton_get_btime_denum (skeleton, &t);  
  printf ("\tBase time denumerator: %lld\n", t);
  oggskeleton_get_utc (skeleton, UTC);
  printf ("\tUTC: %s\n", UTC);
}

int
main (int argc, char **argv)
{
  ogg_sync_state    oy;
  ogg_page          og;
  ogg_stream_state  os;
  OggSkeleton     * skeleton;
  FILE            * fd;
  int               sk_headers = -1;
  
  if (argc < 2)
  {
    printf ("File name not given!\n");
    return -1;
  }
  
  fd = fopen (argv[1], "rb");
  if (fd == NULL)
  {
    perror ("opening file");
    return -1;
  }
  
  /* initialise ogg_sync_state */
  ogg_sync_init (&oy);
  
  /* create skeleton handle */
  skeleton = oggskeleton_new ();
  if (skeleton == NULL)
  {
    printf ("error while creating skeleton handle\n");
    return -1;
  }
  
  /* process all the headers of the Ogg container */
  while (sk_headers != 0)
  {
    int ret = buffer_data (fd, &oy);
    if (ret == 0) break;
    while (ogg_sync_pageout (&oy, &og) > 0)
    {
      ogg_packet        op;
      int               got_packet;

      if (sk_headers != 1)
        ogg_stream_init (&os, ogg_page_serialno (&og));
    
      ogg_stream_pagein (&os, &og);
      if (got_packet = ogg_stream_packetout (&os, &op))
      {
        sk_headers = oggskeleton_decode_header (skeleton, &op);
      
        if (sk_headers == -1)
        {
          /* packet was not a skeleton header */
          ogg_stream_clear (&os);
        }
        else
        {
          /* found a skeleton header */
        }
      }
    }
  }
  
  if (sk_headers == 0)
  {
    print_fishead (skeleton);
  }
  else
  {
    printf ("Couldn't find any skeleton headers in the ogg file\n");
  }
  
  while (1)
  {
    int ret = buffer_data (fd, &oy);
    if (ret == 0) break;
    while (ogg_sync_pageout (&oy, &og) > 0)
    {
      ogg_packet        op;
      int               got_packet;

      ogg_stream_pagein (&os, &og);
      if (got_packet = ogg_stream_packetout (&os, &op))
      {
        printf ("decoding index\n");
        oggskeleton_decode_index (skeleton, &op);
      }
    }
  }
  
  
  oggskeleton_destroy (skeleton);
  
  fclose (fd);
  
  return 0;
}