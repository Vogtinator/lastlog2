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
   Create an entry, delete that entry, and try to read entry again.
   Reading the entry should fail.
*/

#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lastlog2.h"

int
main(void)
{
  const char *db_path = "tst-delete-user.db";
  const char *user = "user";
  time_t ll_time = 0;
  char *tty = NULL;
  char *rhost = NULL;
  char *error = NULL;

  if (ll2_write_entry (db_path, user, time (NULL), "test-tty",
		       "localhost", &error) != 0)
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

  if (ll2_remove_entry (db_path, user, &error) != 0)
    {
      if (error)
        {
          fprintf (stderr, "%s\n", error);
          free (error);
        }
      else
	fprintf (stderr, "ll2_remove_entry failed\n");
      return 1;
    }

  /* this needs to fail, as the old entry shouldn't exist anymore. */
  if (ll2_read_entry (db_path, user, &ll_time, &tty, &rhost, &error) == 0)
    {
      fprintf (stderr, "Reading old user from database did not fail!\n");
      fprintf (stderr, "ll_time=%lld, tty='%s', rhost='%s'\n",
	       (long long int)ll_time, tty, rhost);
      return 1;
    }

  if (error)
    free (error);
  if (tty)
    free (tty);
  if (rhost)
    free (rhost);

  return 0;
}
