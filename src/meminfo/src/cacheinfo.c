/* Copyright (c) 2014-2015, Schmidt.  All rights reserved.
 * Use of this source code is governed by a BSD-style license
 * that can be found in the LICENSE file. */


#include "meminfo.h"
#include "platform.h"


/* 
 *           Cache sizes
 */ 

/**
 * @file
 * @brief 
 * Cache Size
 *
 * @details
 * This function looks up the size of the requested cache level
 * in bytes.  Levels may range from 0 to 3; level=0 is the level 1
 * instruction cache, while level=1 is the level 1 data cache.
 *
 * @param totalcache
 * Output, passed by reference.  On successful return, the value
 * is set to the cache size (in bytes) of the requested level.
 * @param level
 * Input.  The desired cache level.
 *
 * @note
 * Requesting a bad cache will result in the return of CACHE_ERROR.
 * As usual, other internal failures will result in the return of
 * FAILURE.
 *
 * @return
 * The return value indicates the status of the function.
 */
int meminfo_cachesize(cachesize_t *totalcache, const unsigned int level)
{
  *totalcache = 0L;
  cachesize_t cache_size = 0L;
  if (level > 3 || level < 0) return CACHE_ERROR;
  
  #if OS_LINUX
  int ret = MEMINFO_OK;
  
  if (level == 0)
    cache_size = (cachesize_t) sysconf(_SC_LEVEL1_ICACHE_SIZE);
  else if (level == 1)
    cache_size = (cachesize_t) sysconf(_SC_LEVEL1_DCACHE_SIZE);
  else if (level == 2)
    cache_size = (cachesize_t) sysconf(_SC_LEVEL2_CACHE_SIZE);
  else if (level == 3)
    cache_size = (cachesize_t) sysconf(_SC_LEVEL3_CACHE_SIZE);
  else if (level == 4)
    cache_size = (cachesize_t) sysconf(_SC_LEVEL4_CACHE_SIZE);
  
  if (cache_size == 0) return FAILURE;
  
  *totalcache = cache_size;
  #elif OS_MAC
  int ret = MEMINFO_OK;
  
  size_t size = sizeof(cache_size);
  
  if (level == 0)
    ret = sysctlbyname("hw.l1icachesize", &cache_size, &size, NULL, 0);
  else if (level == 1)
    ret = sysctlbyname("hw.l1dcachesize", &cache_size, &size, NULL, 0);
  else if (level == 2)
    ret = sysctlbyname("hw.l2cachesize", &cache_size, &size, NULL, 0);
  else if (level == 3)
    ret = sysctlbyname("hw.l3cachesize", &cache_size, &size, NULL, 0);
  
  chkret(ret, CACHE_ERROR);
  
  if (cache_size == 0) return FAILURE;
  
  *totalcache = cache_size;
  #elif OS_WINDOWS
  int i, winlevel;
  BOOL winret;
  DWORD size = 0;
  SYSTEM_LOGICAL_PROCESSOR_INFORMATION *slpi;
  PROCESSOR_CACHE_TYPE cachetype;
  
  if (level == 0)
  {
    winlevel = 1;
    cachetype = CacheInstruction;
  }
  else if (level == 1)
  {
    winlevel = 1;
    cachetype= CacheData;
  }
  else
  {
    winlevel = level;
    cachetype = CacheUnified;
  }
  
  *totalcache = 0L;
  
  winret = GetLogicalProcessorInformation(0, &size);
  if (winret == TRUE)
    return FAILURE;
  
  slpi = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *) malloc(size);
  GetLogicalProcessorInformation(&slpi[0], &size);
  
  for (i=0; i != size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++)
  {
    if (slpi[i].Relationship == RelationCache && slpi[i].Cache.Level == winlevel)
    {
      *totalcache = (cachesize_t) slpi[i].Cache.Size;
      return MEMINFO_OK;
    }
  }
  
  return FAILURE;
  
  #else
  return PLATFORM_ERROR;
  #endif
  
  return MEMINFO_OK;
}



/* 
 *           Cache linesize
 */ 

/**
 * @file
 * @brief 
 * Cache Line Size
 *
 * @details
 * This function looks up the cache line size in bytes.
 *
 * @param totalswap
 * Output, passed by reference.  On successful return, the value
 * is set to the cache line size (in bytes) for the system.
 *
 * @return
 * The return value indicates the status of the function.
 */
int meminfo_cachelinesize(cachesize_t *linesize)
{
  *linesize = 0L;
  cachesize_t cache_size = 0L;
  
  #if OS_LINUX
  int ret;
  
  cache_size = (cachesize_t) sysconf(_SC_LEVEL1_DCACHE_LINESIZE);
  
  if (cache_size == 0) return FAILURE;
  
  *linesize = cache_size;
  #elif OS_MAC
  int ret;
  size_t size = sizeof(cache_size);
  
  ret = sysctlbyname("hw.cachelinesize", &cache_size, &size, 0, 0);
  
  chkret(ret, FAILURE);
  
  if (cache_size == 0)
    return FAILURE;
  
  *linesize = cache_size;
  #elif OS_WINDOWS
  int i;
  BOOL winret;
  DWORD size = 0;
  SYSTEM_LOGICAL_PROCESSOR_INFORMATION *slpi;
  
  *linesize = 0;
  
  winret = GetLogicalProcessorInformation(0, &size);
  if (winret == TRUE)
    return FAILURE;
  
  slpi = (SYSTEM_LOGICAL_PROCESSOR_INFORMATION *) malloc(size);
  GetLogicalProcessorInformation(&slpi[0], &size);
  
  for (i=0; i != size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++)
  {
    if (slpi[i].Relationship == RelationCache && slpi[i].Cache.Level == 1)
    {
      *linesize = (cachesize_t) slpi[i].Cache.LineSize;
      return MEMINFO_OK;
    }
  }
  
  return FAILURE;
  
  #else
  return PLATFORM_ERROR;
  #endif
  
  return MEMINFO_OK;
}

