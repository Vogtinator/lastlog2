/* SPDX-License-Identifier: BSD-2-Clause

  Copyright (c) 2023, Thorsten Kukuk <kukuk@suse.com>

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice,
     this list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.
*/

/* Test case:
   Create an entry with an 3*INT32_MAX timestamp, store that,
   read that via ll2_read_all callback again and make sure the
   timestamp is correct.
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "lastlog2.h"

#define BIG_TIME_VALUE ((int64_t)3*INT32_MAX)

const char *user = "y2038";
const char *tty = "pts/test";
const char *rhost = NULL;

static int
check_y2038 (const char *res_user, time_t ll_time, const char *res_tty,
	     const char *res_rhost)
{

  if (strcmp (user, res_user) != 0)
    {
      fprintf (stderr, "write/read entry user mismatch: written: %s, got: %s\n",
	       user, res_user);
      exit (1);
    }

  if (ll_time != BIG_TIME_VALUE)
    {
      fprintf (stderr, "write/read entry time mismatch: written: %lld, got: %lld\n",
	       (long long int)BIG_TIME_VALUE, (long long int)ll_time);
      exit (1);
    }

  if (strcmp (tty, res_tty) != 0)
    {
      fprintf (stderr, "write/read entry tty mismatch: written: %s, got: %s\n",
	       tty, res_tty);
      exit (1);
    }

  if (rhost != NULL)
    {
      fprintf (stderr, "write/read entry rhost mismatch: written: NULL, got: %s\n",
	       res_rhost);
      exit (1);
    }


  return 0;
}

int
main(void)
{
  const char *db_path = "y2038-ll2_read_all.db";
  char *error = NULL;

  remove (db_path);

  printf ("Big time value is: %lld\n", (long long int)BIG_TIME_VALUE);

  if (ll2_write_entry (db_path, user, BIG_TIME_VALUE, tty, rhost, &error) != 0)
    {
      if (error)
        {
          fprintf (stderr, "%s\n", error);
          free (error);
        }
      else
	fprintf (stderr, "ll2_write_entry failed\n");
      return 1;
    }

  if (ll2_read_all (db_path, check_y2038, &error) != 0)
    {
      if (error)
        {
          fprintf (stderr, "%s\n", error);
          free (error);
        }
      else
        fprintf (stderr, "Couldn't read entries for all users\n");

      return 1;
    }

  return 0;
}
