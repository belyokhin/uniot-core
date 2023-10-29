/*
 * This is a part of the Uniot project.
 * Copyright (C) 2016-2023 Uniot <contact@uniot.io>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <Arduino.h>

#ifdef __cplusplus
#include <functional>
#endif

/**
 * std::queue requires much more resources
 */
template <typename T>
class ClearQueue {
 public:
#ifdef __cplusplus
  typedef std::function<void(const T&)> VoidCallback;
#else
  typedef void (*VoidCallback)(T);
#endif

  ClearQueue(ClearQueue const&) = delete;
  void operator=(ClearQueue const&) = delete;

  ClearQueue();
  virtual ~ClearQueue();

  void push(const T value);
  bool pushUnique(const T value);
  T hardPop();
  T hardPeek() const;
  T pop(const T errorCode);
  T peek(const T errorCode) const;
  bool removeOne(const T value);
  bool contains(const T value) const;
  bool isEmpty() const;
  void clean();
  void forEach(VoidCallback callback);

 protected:
  typedef struct node {
    T value;
    node* next;
  }* pnode;

  pnode mHead;
  pnode mTail;
};

template <typename T>
ClearQueue<T>::ClearQueue() {
  mHead = NULL;
  mTail = NULL;
}

template <typename T>
ClearQueue<T>::~ClearQueue() {
  clean();
}

template <typename T>
bool ClearQueue<T>::pushUnique(const T value) {
  if (!contains(value)) {
    push(value);
    return true;
  }
  return false;
}

template <typename T>
void ClearQueue<T>::push(const T value) {
  pnode cur = mTail;
  mTail = (pnode) new node;

  mTail->next = NULL;
  mTail->value = value;
  if (isEmpty()) {
    mHead = mTail;
  } else {
    cur->next = mTail;
  }
}

template <typename T>
T ClearQueue<T>::hardPop() {
  T value = mHead->value;
  pnode cur = mHead->next;
  delete mHead;
  mHead = cur;

  if (mHead == NULL) {
    mTail = NULL;
  }

  return value;
}

template <typename T>
T ClearQueue<T>::hardPeek() const {
  return mHead->value;
}

template <typename T>
T ClearQueue<T>::pop(const T errorCode) {
  if (!isEmpty()) {
    return hardPop();
  }
  return errorCode;
}

template <typename T>
T ClearQueue<T>::peek(const T errorCode) const {
  if (!isEmpty()) {
    return hardPeek();
  }
  return errorCode;
}

template <typename T>
bool ClearQueue<T>::removeOne(const T value) {
  if (!isEmpty()) {
    if (mHead->value == value) {
      hardPop();
      return true;
    }
    for (pnode cur = mHead; cur->next != NULL; cur = cur->next) {
      if (cur->next->value == value) {
        pnode newNext = cur->next->next;
        delete cur->next;
        cur->next = newNext;

        if (newNext == NULL) {
          mTail = cur;
        }

        return true;
      }
    }
  }
  return false;
}

template <typename T>
bool ClearQueue<T>::contains(const T value) const {
  for (pnode cur = mHead; cur != NULL; cur = cur->next) {
    if (cur->value == value) {
      return true;
    }
  }
  return false;
}

template <typename T>
bool ClearQueue<T>::isEmpty() const {
  return mHead == NULL;
}

template <typename T>
void ClearQueue<T>::clean() {
  for (pnode cur = mHead; cur != NULL; mHead = cur) {
    cur = mHead->next;
    delete mHead;
  }

  mHead = NULL;
  mTail = NULL;
}

template <typename T>
void ClearQueue<T>::forEach(VoidCallback callback) {
  for (pnode cur = mHead; cur != NULL; cur = cur->next) {
    callback(cur->value);
  }
}
