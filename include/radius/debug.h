/* -*- buffer-read-only: t -*- vi: set ro:
   THIS FILE IS GENERATED AUTOMATICALLY.  PLEASE DO NOT EDIT.
*/
/* This file is part of GNU Radius.
   Copyright (C) 2007, 2008 Free Software Foundation, Inc.

   Written by Sergey Poznyakoff
  
   GNU Radius is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
  
   GNU Radius is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
  
   You should have received a copy of the GNU General Public License
   along with GNU Radius; if not, write to the Free Software Foundation,
   Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA. */

/* Debugging facilities */

#ifndef _gnu_radius_debug_h
#define _gnu_radius_debug_h

#ifndef GRAD_MAX_DEBUG_LEVEL
# define GRAD_MAX_DEBUG_LEVEL 100
#endif

#if RADIUS_DEBUG
# define GRAD_DEBUG_LEVEL(level) grad_debug_p(__FILE__, level)
#define __grad_debug(text)                                                    \
 do {                                                                         \
   if (grad_source_info_option)                                               \
     grad_log(GRAD_LOG_DEBUG, "%s:%lu:%s: %s",                                \
              __FILE__, __LINE__, __FUNCTION__,                               \
              text);                                                          \
   else                                                                       \
     grad_log(GRAD_LOG_DEBUG, "%s", text);                                    \
 } while (0)

#define GRAD_DEBUG(lev,text)                                                  \
 do { if (GRAD_DEBUG_LEVEL(lev)) __grad_debug(text); } while(0)
#else
# define GRAD_DEBUG_LEVEL(level) 0
# define GRAD_DEBUG(mode,str)
#endif


#if RADIUS_DEBUG
#define __grad_debug1(fmt, x1)  \
 do {                                                                         \
   if (grad_source_info_option)                                               \
    grad_log(GRAD_LOG_DEBUG, "%s:%lu:%s: " fmt, __FILE__, __LINE__, __FUNCTION__, x1); \
   else                                                                       \
    grad_log(GRAD_LOG_DEBUG, fmt,x1);\
 } while (0)                                                                  
#define GRAD_DEBUG1(lev, fmt, x1) \
 do { if (GRAD_DEBUG_LEVEL(lev)) __grad_debug1(fmt, x1); } while(0)
#else
# define GRAD_DEBUG1(lev, fmt, x1)
#endif


#if RADIUS_DEBUG
#define __grad_debug2(fmt, x1, x2)  \
 do {                                                                         \
   if (grad_source_info_option)                                               \
    grad_log(GRAD_LOG_DEBUG, "%s:%lu:%s: " fmt, __FILE__, __LINE__, __FUNCTION__, x1, x2); \
   else                                                                       \
    grad_log(GRAD_LOG_DEBUG, fmt,x1, x2);\
 } while (0)                                                                  
#define GRAD_DEBUG2(lev, fmt, x1, x2) \
 do { if (GRAD_DEBUG_LEVEL(lev)) __grad_debug2(fmt, x1, x2); } while(0)
#else
# define GRAD_DEBUG2(lev, fmt, x1, x2)
#endif


#if RADIUS_DEBUG
#define __grad_debug3(fmt, x1, x2, x3)  \
 do {                                                                         \
   if (grad_source_info_option)                                               \
    grad_log(GRAD_LOG_DEBUG, "%s:%lu:%s: " fmt, __FILE__, __LINE__, __FUNCTION__, x1, x2, x3); \
   else                                                                       \
    grad_log(GRAD_LOG_DEBUG, fmt,x1, x2, x3);\
 } while (0)                                                                  
#define GRAD_DEBUG3(lev, fmt, x1, x2, x3) \
 do { if (GRAD_DEBUG_LEVEL(lev)) __grad_debug3(fmt, x1, x2, x3); } while(0)
#else
# define GRAD_DEBUG3(lev, fmt, x1, x2, x3)
#endif


#if RADIUS_DEBUG
#define __grad_debug4(fmt, x1, x2, x3, x4)  \
 do {                                                                         \
   if (grad_source_info_option)                                               \
    grad_log(GRAD_LOG_DEBUG, "%s:%lu:%s: " fmt, __FILE__, __LINE__, __FUNCTION__, x1, x2, x3, x4); \
   else                                                                       \
    grad_log(GRAD_LOG_DEBUG, fmt,x1, x2, x3, x4);\
 } while (0)                                                                  
#define GRAD_DEBUG4(lev, fmt, x1, x2, x3, x4) \
 do { if (GRAD_DEBUG_LEVEL(lev)) __grad_debug4(fmt, x1, x2, x3, x4); } while(0)
#else
# define GRAD_DEBUG4(lev, fmt, x1, x2, x3, x4)
#endif


#if RADIUS_DEBUG
#define __grad_debug5(fmt, x1, x2, x3, x4, x5)  \
 do {                                                                         \
   if (grad_source_info_option)                                               \
    grad_log(GRAD_LOG_DEBUG, "%s:%lu:%s: " fmt, __FILE__, __LINE__, __FUNCTION__, x1, x2, x3, x4, x5); \
   else                                                                       \
    grad_log(GRAD_LOG_DEBUG, fmt,x1, x2, x3, x4, x5);\
 } while (0)                                                                  
#define GRAD_DEBUG5(lev, fmt, x1, x2, x3, x4, x5) \
 do { if (GRAD_DEBUG_LEVEL(lev)) __grad_debug5(fmt, x1, x2, x3, x4, x5); } while(0)
#else
# define GRAD_DEBUG5(lev, fmt, x1, x2, x3, x4, x5)
#endif


#if RADIUS_DEBUG
#define __grad_debug6(fmt, x1, x2, x3, x4, x5, x6)  \
 do {                                                                         \
   if (grad_source_info_option)                                               \
    grad_log(GRAD_LOG_DEBUG, "%s:%lu:%s: " fmt, __FILE__, __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6); \
   else                                                                       \
    grad_log(GRAD_LOG_DEBUG, fmt,x1, x2, x3, x4, x5, x6);\
 } while (0)                                                                  
#define GRAD_DEBUG6(lev, fmt, x1, x2, x3, x4, x5, x6) \
 do { if (GRAD_DEBUG_LEVEL(lev)) __grad_debug6(fmt, x1, x2, x3, x4, x5, x6); } while(0)
#else
# define GRAD_DEBUG6(lev, fmt, x1, x2, x3, x4, x5, x6)
#endif


#if RADIUS_DEBUG
#define __grad_debug7(fmt, x1, x2, x3, x4, x5, x6, x7)  \
 do {                                                                         \
   if (grad_source_info_option)                                               \
    grad_log(GRAD_LOG_DEBUG, "%s:%lu:%s: " fmt, __FILE__, __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7); \
   else                                                                       \
    grad_log(GRAD_LOG_DEBUG, fmt,x1, x2, x3, x4, x5, x6, x7);\
 } while (0)                                                                  
#define GRAD_DEBUG7(lev, fmt, x1, x2, x3, x4, x5, x6, x7) \
 do { if (GRAD_DEBUG_LEVEL(lev)) __grad_debug7(fmt, x1, x2, x3, x4, x5, x6, x7); } while(0)
#else
# define GRAD_DEBUG7(lev, fmt, x1, x2, x3, x4, x5, x6, x7)
#endif


#if RADIUS_DEBUG
#define __grad_debug8(fmt, x1, x2, x3, x4, x5, x6, x7, x8)  \
 do {                                                                         \
   if (grad_source_info_option)                                               \
    grad_log(GRAD_LOG_DEBUG, "%s:%lu:%s: " fmt, __FILE__, __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8); \
   else                                                                       \
    grad_log(GRAD_LOG_DEBUG, fmt,x1, x2, x3, x4, x5, x6, x7, x8);\
 } while (0)                                                                  
#define GRAD_DEBUG8(lev, fmt, x1, x2, x3, x4, x5, x6, x7, x8) \
 do { if (GRAD_DEBUG_LEVEL(lev)) __grad_debug8(fmt, x1, x2, x3, x4, x5, x6, x7, x8); } while(0)
#else
# define GRAD_DEBUG8(lev, fmt, x1, x2, x3, x4, x5, x6, x7, x8)
#endif


#if RADIUS_DEBUG
#define __grad_debug9(fmt, x1, x2, x3, x4, x5, x6, x7, x8, x9)  \
 do {                                                                         \
   if (grad_source_info_option)                                               \
    grad_log(GRAD_LOG_DEBUG, "%s:%lu:%s: " fmt, __FILE__, __LINE__, __FUNCTION__, x1, x2, x3, x4, x5, x6, x7, x8, x9); \
   else                                                                       \
    grad_log(GRAD_LOG_DEBUG, fmt,x1, x2, x3, x4, x5, x6, x7, x8, x9);\
 } while (0)                                                                  
#define GRAD_DEBUG9(lev, fmt, x1, x2, x3, x4, x5, x6, x7, x8, x9) \
 do { if (GRAD_DEBUG_LEVEL(lev)) __grad_debug9(fmt, x1, x2, x3, x4, x5, x6, x7, x8, x9); } while(0)
#else
# define GRAD_DEBUG9(lev, fmt, x1, x2, x3, x4, x5, x6, x7, x8, x9)
#endif


int grad_debug_p(char *name, int level);
const char *grad_request_code_to_name(int code);
int grad_request_name_to_code(const char *);
void grad_set_debug_levels(char *str);
int grad_set_module_debug_level(char *name, int level);
void grad_clear_debug();

const char *grad_next_matching_code_name(void *data);
const char *grad_first_matching_code_name(const char *name, void **ptr);

#endif
