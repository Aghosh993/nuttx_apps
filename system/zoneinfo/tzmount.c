/****************************************************************************
 * apps/system/zoninfo/tzmount.c
 *
 *   Copyright (C) 2015 Gregory Nutt. All rights reserved.
 *   Author: Gregory Nutt <gnutt@nuttx.org>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/mount.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <nuttx/fs/ramdisk.h>
#include <apps/zoneinfo.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ifndef CONFIG_SYSTEM_ZONEINFO_RAMDEVNO
#  define CONFIG_SYSTEM_ZONEINFO_RAMDEVNO 1
#endif

#ifndef CONFIG_LIBC_TZDIR
#  errror CONFIG_LIBC_TZDIR is not defined
#endif

#ifdef CONFIG_DISABLE_MOUNTPOINT
#  error "Mountpoint support is disabled"
#endif

#if CONFIG_NFILE_DESCRIPTORS < 4
#  error "Not enough file descriptors"
#endif

#ifndef CONFIG_FS_ROMFS
#  error "ROMFS support not enabled"
#endif

#define SECTORSIZE  64
#define NSECTORS(b) (((b)+SECTORSIZE-1)/SECTORSIZE)

#define STR_RAMDEVNO(m)    #m
#define MKMOUNT_DEVNAME(m) "/dev/ram" STR_RAMDEVNO(m)
#define MOUNT_DEVNAME      MKMOUNT_DEVNAME(CONFIG_SYSTEM_ZONEINFO_RAMDEVNO)

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

#ifdef CONFIG_BUILD_KERNEL
int main(int argc, FAR char *argv[])
#else
int tzmount_main(int argc, char **argv)
#endif
{
   int  ret;

  /* Create a RAM disk for the test */

  ret = romdisk_register(CONFIG_SYSTEM_ZONEINFO_RAMDEVNO,
                         romfs_zoneinfo_img,
                         NSECTORS(romfs_zoneinfo_img_len),
                         SECTORSIZE);
  if (ret < 0)
    {
      printf("ERROR: Failed to create RAM disk\n");
      return EXIT_FAILURE;
    }

  /* Mount the test file system */

  printf("Mounting ROMFS filesystem at target=%s with source=%s\n",
         CONFIG_LIBC_TZDIR, MOUNT_DEVNAME);

  ret = mount(MOUNT_DEVNAME, CONFIG_LIBC_TZDIR, "romfs", MS_RDONLY, NULL);
  if (ret < 0)
    {
      printf("ERROR: Mount failed: %d\n", errno);
      return EXIT_FAILURE;
    }

  printf("TZ database mounted at %s\n", CONFIG_LIBC_TZDIR);
  return EXIT_SUCCESS;
}