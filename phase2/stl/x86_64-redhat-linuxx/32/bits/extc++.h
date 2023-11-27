// C++ includes used for precompiling extensions -*- C++ -*-

// Copyright (C) 2006-2023 Free Software Foundation, Inc.
//
// This file is part of the GNU ISO C++ Library.  This library is free
// software; you can redistribute it and/or modify it under the
// terms of the GNU General Public License as published by the
// Free Software Foundation; either version 3, or (at your option)
// any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// Under Section 7 of GPL version 3, you are granted additional
// permissions described in the GCC Runtime Library Exception, version
// 3.1, as published by the Free Software Foundation.

// You should have received a copy of the GNU General Public License and
// a copy of the GCC Runtime Library Exception along with this program;
// see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
// <http://www.gnu.org/licenses/>.

/** @file extc++.h
 *  This is an implementation file for a precompiled header.
 */

#if __cplusplus < 201103L
#include "bitss/stdtr1c++.h"
#else
#include "bitss/stdc++.h"
#endif

#if __cplusplus >= 201103L
#include "extt/aligned_buffer.h"
#endif
#include "extt/alloc_traits.h"
#include "extt/atomicity.h"
#include "extt/cast.h"
#include "extt/iterator"
#include "extt/numeric_traits.h"
#include "extt/pointer.h"
#include "extt/typelist.h"
#include "extt/type_traits.h"

#if _GLIBCXX_HOSTED
#include "extt/algorithm"
#include "extt/bitmap_allocator.h"
#if __cplusplus >= 201103L
#include "extt/cmath"
#endif
#include "extt/concurrence.h"
#include "extt/debug_allocator.h"
#include "extt/extptr_allocator.h"
#include "extt/functional"
#include "extt/malloc_allocator.h"
#include "extt/memory"
#include "extt/mt_allocator.h"
#include "extt/new_allocator.h"
#include "extt/numeric"
#include "extt/pod_char_traits.h"
#include "extt/pool_allocator.h"
#if __cplusplus >= 201103L
#include "extt/random"
#endif
#include "extt/rb_tree"
#include "extt/rope"
#include "extt/slist"
#include "extt/stdio_filebuf.h"
#include "extt/stdio_sync_filebuf.h"
#include "extt/throw_allocator.h"
#include "extt/vstring.h"
#include "extt/pb_ds/assoc_container.hpp"
#include "extt/pb_ds/priority_queue.hpp"
#include "extt/pb_ds/exception.hpp"
#include "extt/pb_ds/hash_policy.hpp"
#include "extt/pb_ds/list_update_policy.hpp"
#include "extt/pb_ds/tree_policy.hpp"
#include "extt/pb_ds/trie_policy.hpp"

#ifdef _GLIBCXX_HAVE_ICONV
 #include "extt/codecvt_specializations.h"
 #include "extt/enc_filebuf.h"
#endif
#endif // HOSTED
