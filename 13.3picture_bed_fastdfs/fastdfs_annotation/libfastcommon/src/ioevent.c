/*
 * Copyright (c) 2020 YuQing <384681@qq.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the Lesser GNU General Public License, version 3
 * or later ("LGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include "fc_memory.h"
#include "ioevent.h"

#if IOEVENT_USE_KQUEUE
/* we define these here as numbers, because for kqueue mapping them to a combination of
     * filters / flags is hard to do. */
int kqueue_ev_convert(int16_t event, uint16_t flags)
{
  int r;

  if (event == EVFILT_READ) {
    r = KPOLLIN;
  }
  else if (event == EVFILT_WRITE) {
    r = KPOLLOUT;
  }
  else {
    r = 0;
  }

  if (flags & EV_EOF) {
    r |= KPOLLHUP;
  }
  return r;
}
#endif

int ioevent_init(IOEventPoller *ioevent, const int size,
    const int timeout_ms, const int extra_events)
{
  int bytes;

  ioevent->size = size;
  ioevent->extra_events = extra_events;
  ioevent->iterator.index = 0;
  ioevent->iterator.count = 0;

#if IOEVENT_USE_EPOLL
  ioevent->poll_fd = epoll_create(ioevent->size);
  if (ioevent->poll_fd < 0) {
    return errno != 0 ? errno : ENOMEM;
  }
  bytes = sizeof(struct epoll_event) * size;
  ioevent->events = (struct epoll_event *)fc_malloc(bytes);
#elif IOEVENT_USE_KQUEUE
  ioevent->poll_fd = kqueue();
  if (ioevent->poll_fd < 0) {
    return errno != 0 ? errno : ENOMEM;
  }
  bytes = sizeof(struct kevent) * size;
  ioevent->events = (struct kevent *)fc_malloc(bytes);
#elif IOEVENT_USE_PORT
  ioevent->poll_fd = port_create();
  if (ioevent->poll_fd < 0) {
    return errno != 0 ? errno : ENOMEM;
  }
  bytes = sizeof(port_event_t) * size;
  ioevent->events = (port_event_t *)fc_malloc(bytes);
#endif

  if (ioevent->events == NULL) {
    close(ioevent->poll_fd);
    ioevent->poll_fd = -1;
    return ENOMEM;
  }
  ioevent_set_timeout(ioevent, timeout_ms);

  return 0;
}

void ioevent_destroy(IOEventPoller *ioevent)
{
  if (ioevent->events != NULL) {
    free(ioevent->events);
    ioevent->events = NULL;
  }

  if (ioevent->poll_fd >= 0) {
    close(ioevent->poll_fd);
    ioevent->poll_fd = -1;
  }
}
// 读写
int ioevent_attach(IOEventPoller *ioevent, const int fd, const int e,
    void *data)
{
#if IOEVENT_USE_EPOLL
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.events = e | ioevent->extra_events;
  ev.data.ptr = data;
  return epoll_ctl(ioevent->poll_fd, EPOLL_CTL_ADD, fd, &ev);
#elif IOEVENT_USE_KQUEUE
  struct kevent ev[2];
  int n = 0;
  if (e & IOEVENT_READ) {
    EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD | ioevent->extra_events, 0, 0, data);
  }
  if (e & IOEVENT_WRITE) {
    EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD | ioevent->extra_events, 0, 0, data);
  }
  if (n == 0) {
      return ENOENT;
  }
  return kevent(ioevent->poll_fd, ev, n, NULL, 0, NULL);
#elif IOEVENT_USE_PORT
  return port_associate(ioevent->poll_fd, PORT_SOURCE_FD, fd, e, data);
#endif
}

int ioevent_modify(IOEventPoller *ioevent, const int fd, const int e,
    void *data)
{
#if IOEVENT_USE_EPOLL
  struct epoll_event ev;
  memset(&ev, 0, sizeof(ev));
  ev.events = e | ioevent->extra_events;
  ev.data.ptr = data;
  return epoll_ctl(ioevent->poll_fd, EPOLL_CTL_MOD, fd, &ev);
#elif IOEVENT_USE_KQUEUE
  struct kevent ev[2];
  int result;
  int n = 0;

  if (e & IOEVENT_READ) {
    EV_SET(&ev[n++], fd, EVFILT_READ, EV_ADD | ioevent->extra_events, 0, 0, data);
  }
  else {
    EV_SET(&ev[n++], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  }

  if (e & IOEVENT_WRITE) {
    EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_ADD | ioevent->extra_events, 0, 0, data);
  }
  else {
    EV_SET(&ev[n++], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  }

  result = kevent(ioevent->poll_fd, ev, n, NULL, 0, NULL);
  if (result == -1) {
      result = ioevent_detach(ioevent, fd);
      if (e & (IOEVENT_READ | IOEVENT_WRITE)) {
          result = ioevent_attach(ioevent, fd, e, data);
      }
  }
  return result;
#elif IOEVENT_USE_PORT
  return port_associate(ioevent->poll_fd, PORT_SOURCE_FD, fd, e, data);
#endif
}

int ioevent_detach(IOEventPoller *ioevent, const int fd)
{
#if IOEVENT_USE_EPOLL
  return epoll_ctl(ioevent->poll_fd, EPOLL_CTL_DEL, fd, NULL);
#elif IOEVENT_USE_KQUEUE
  struct kevent ev[1];
  int r, w;

  EV_SET(&ev[0], fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
  r = kevent(ioevent->poll_fd, ev, 1, NULL, 0, NULL);

  EV_SET(&ev[0], fd, EVFILT_WRITE, EV_DELETE, 0, 0, NULL);
  w = kevent(ioevent->poll_fd, ev, 1, NULL, 0, NULL);

  return (r == 0 || w == 0) ? 0 : r;
#elif IOEVENT_USE_PORT
  return port_dissociate(ioevent->poll_fd, PORT_SOURCE_FD, fd);
#endif
}

int ioevent_poll(IOEventPoller *ioevent)
{
#if IOEVENT_USE_EPOLL
  return epoll_wait(ioevent->poll_fd, ioevent->events, ioevent->size, ioevent->timeout);
#elif IOEVENT_USE_KQUEUE
  return kevent(ioevent->poll_fd, NULL, 0, ioevent->events, ioevent->size, &ioevent->timeout);
#elif IOEVENT_USE_PORT
  int result;
  int retval;
  unsigned int nget = 1;
  if((retval = port_getn(ioevent->poll_fd, ioevent->events,
          ioevent->size, &nget, &ioevent->timeout)) == 0)
  {
    result = (int)nget;
  } else {
    switch(errno) {
      case EINTR:
      case EAGAIN:
      case ETIME:
        if (nget > 0) {
          result = (int)nget;
        }
        else {
          result = 0;
        }
        break;
      default:
        result = -1;
        break;
    }
  }
  return result;
#else
#error port me
#endif
}

