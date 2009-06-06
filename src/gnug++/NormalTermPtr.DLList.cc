// This may look like C code, but it is really -*- C++ -*-
// WARNING: This file is obsolete.  Use ../DLList.cc, if you can.
/* 
Copyright (C) 1988 Free Software Foundation
    written by Doug Lea (dl@rocky.oswego.edu)

This file is part of the GNU C++ Library.  This library is free
software; you can redistribute it and/or modify it under the terms of
the GNU Library General Public License as published by the Free
Software Foundation; either version 2 of the License, or (at your
option) any later version.  This library is distributed in the hope
that it will be useful, but WITHOUT ANY WARRANTY; without even the
implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU Library General Public License for more details.
You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#ifdef __GNUG__
#pragma implementation
#endif

#include "config.h"
#include <cstring>
#include <climits>
#include "NormalTermPtr.DLList.h"
#include <builtin.h>

// error handling



void NormalTermPtrDLList::error(const char* msg)
{
  (*lib_error_handler)("DLList", msg);
}

int NormalTermPtrDLList::length()
{
  int l = 0;
  NormalTermPtrDLListNode* t = h;
  if (t != 0) do { ++l; t = t->fd; } while (t != h);
  return l;
}

NormalTermPtrDLList::NormalTermPtrDLList(const NormalTermPtrDLList& a)
{
  if (a.h == 0)
    h = 0;
  else
  {
    NormalTermPtrDLListNode* p = a.h;
    NormalTermPtrDLListNode* t = new NormalTermPtrDLListNode(p->hd);
    h = t;
    p = p->fd;
    while (p != a.h)
    {
      NormalTermPtrDLListNode* n = new NormalTermPtrDLListNode(p->hd);
      t->fd = n;
      n->bk = t;
      t = n;
        p = p->fd;
    }
    t->fd = h;
    h->bk = t;
    return;
  }
}

NormalTermPtrDLList& NormalTermPtrDLList::operator = (const NormalTermPtrDLList& a)
{
  if (h != a.h)
  {
    clear();
    if (a.h != 0)
    {
      NormalTermPtrDLListNode* p = a.h;
      NormalTermPtrDLListNode* t = new NormalTermPtrDLListNode(p->hd);
      h = t;
      p = p->fd;
      while (p != a.h)
      {
        NormalTermPtrDLListNode* n = new NormalTermPtrDLListNode(p->hd);
        t->fd = n;
        n->bk = t;
        t = n;
        p = p->fd;
      }
      t->fd = h;
      h->bk = t;
    }
  }
  return *this;
}

void NormalTermPtrDLList::clear()
{
  if (h == 0)
    return;

  NormalTermPtrDLListNode* p = h->fd;
  h->fd = 0;
  h = 0;

  while (p != 0)
  {
    NormalTermPtrDLListNode* nxt = p->fd;
    // cengiz
    delete p->hd;
    delete(p);
    p = nxt;
  }
}


Pix NormalTermPtrDLList::prepend(NormalTermPtr  item)
{
  NormalTermPtrDLListNode* t = new NormalTermPtrDLListNode(item);
  if (h == 0)
    t->fd = t->bk = h = t;
  else
  {
    t->fd = h;
    t->bk = h->bk;
    h->bk->fd = t;
    h->bk = t;
    h = t;
  }
  return Pix(t);
}

Pix NormalTermPtrDLList::append(NormalTermPtr  item)
{
  NormalTermPtrDLListNode* t = new NormalTermPtrDLListNode(item);
  if (h == 0)
    t->fd = t->bk = h = t;
  else
  {
    t->bk = h->bk;
    t->bk->fd = t;
    t->fd = h;
    h->bk = t;
  }
  return Pix(t);
}

void NormalTermPtrDLList::join(NormalTermPtrDLList& b)
{
  NormalTermPtrDLListNode* t = b.h;
  b.h = 0;
  if (h == 0)
    h = t;
  else if (t != 0)
  {
    NormalTermPtrDLListNode* l = t->bk;
    h->bk->fd = t;
    t->bk = h->bk;
    h->bk = l;
    l->fd = h;
  }
}

int NormalTermPtrDLList::owns(Pix p)
{
  NormalTermPtrDLListNode* t = h;
  if (t != 0 && p != 0)
  {
    do
    {
      if (Pix(t) == p) return 1;
      t = t->fd;
    } while (t != h);
  }
  return 0;
}

void NormalTermPtrDLList::del(Pix& p, int dir)
{
  if (p == 0) error("null Pix");
  NormalTermPtrDLListNode* t = (NormalTermPtrDLListNode*) p;
  if (t->fd == t)
  {
    h = 0;
    p = 0;
  }
  else
  {
    if (dir < 0)
    {
      if (t == h)
        p = 0;
      else
        p = Pix(t->bk);
    }
    else
    {
      if (t == h->bk)
        p = 0;
      else
        p = Pix(t->fd);
    }
    t->bk->fd = t->fd;
    t->fd->bk = t->bk;
    if (t == h) h = t->fd;
  }
  delete t;
}

void NormalTermPtrDLList::del_front()
{
  if (h == 0)
    error("del_front of empty list");
  NormalTermPtrDLListNode* t = h;
  if (h->fd == h)
    h = 0;
  else
  {
    h->fd->bk = h->bk;
    h->bk->fd = h->fd;
    h = h->fd;
  }
  delete t;
}

int NormalTermPtrDLList::OK()
{
  int v = 1;
  if (h != 0)
  {
    NormalTermPtrDLListNode* t = h;
    long count = LONG_MAX;      // Lots of chances to find h!
    do
    {
      count--;
      v &= t->bk->fd == t;
      v &= t->fd->bk == t;
      t = t->fd;
    } while (v && count > 0 && t != h);
    v &= count > 0;
  }
  if (!v) error("invariant failure");
  return v;
}
