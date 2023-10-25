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

#include "EventBus.h"

#include <Arduino.h>

#include "EventEmitter.h"
#include "EventListener.h"

namespace uniot {
template <class T_topic, class T_msg, class T_data>
EventBus<T_topic, T_msg, T_data>::~EventBus() {
  mListeners.forEach([this](EventListener<T_topic, T_msg, T_data> *listener) { listener->mEventBusQueue.removeOne(this); });
  mEmitters.forEach([this](EventEmitter<T_topic, T_msg, T_data> *emitter) { emitter->mEventBusQueue.removeOne(this); });
}

template <class T_topic, class T_msg, class T_data>
void EventBus<T_topic, T_msg, T_data>::connect(IEventBusKitConnection<T_topic, T_msg, T_data> *connection) {
  connection->connect(this);
}

template <class T_topic, class T_msg, class T_data>
void EventBus<T_topic, T_msg, T_data>::disconnect(IEventBusKitConnection<T_topic, T_msg, T_data> *connection) {
  connection->disconnect(this);
}

template <class T_topic, class T_msg, class T_data>
bool EventBus<T_topic, T_msg, T_data>::connect(EventEmitter<T_topic, T_msg, T_data> *emitter) {
  if (emitter && emitter->connectUnique(this)) {
    mEmitters.pushUnique(emitter);
    return true;
  }
  return false;
}

template <class T_topic, class T_msg, class T_data>
void EventBus<T_topic, T_msg, T_data>::disconnect(EventEmitter<T_topic, T_msg, T_data> *emitter) {
  if (emitter) {
    mEmitters.removeOne(emitter);
    emitter->mEventBusQueue.removeOne(this);
  }
}

template <class T_topic, class T_msg, class T_data>
bool EventBus<T_topic, T_msg, T_data>::connect(EventListener<T_topic, T_msg, T_data> *listener) {
  if (listener && listener->connectUnique(this)) {
    mListeners.pushUnique(listener);
    return true;
  }
  return false;
}

template <class T_topic, class T_msg, class T_data>
void EventBus<T_topic, T_msg, T_data>::disconnect(EventListener<T_topic, T_msg, T_data> *listener) {
  if (listener) {
    mListeners.removeOne(listener);
    listener->mEventBusQueue.removeOne(this);
  }
}

template <class T_topic, class T_msg, class T_data>
void EventBus<T_topic, T_msg, T_data>::openDataChannel(T_topic topic, size_t limit) {
  mDataChannels.open(topic, limit);
}

template <class T_topic, class T_msg, class T_data>
void EventBus<T_topic, T_msg, T_data>::closeDataChannel(T_topic topic) {
  mDataChannels.close(topic);
}

template <class T_topic, class T_msg, class T_data>
void EventBus<T_topic, T_msg, T_data>::sendDataToChannel(T_topic topic, String data) {
  mDataChannels.send(topic, data);
}

template <class T_topic, class T_msg, class T_data>
T_data EventBus<T_topic, T_msg, T_data>::receiveDataFromChannel(T_topic topic) {
  return mDataChannels.receive(topic);
}

template <class T_topic, class T_msg, class T_data>
bool EventBus<T_topic, T_msg, T_data>::isDataChannelEmpty(T_topic topic) {
  return mDataChannels.isEmpty(topic);
}

template <class T_topic, class T_msg, class T_data>
void EventBus<T_topic, T_msg, T_data>::emitEvent(T_topic topic, T_msg msg) {
  mEvents.push(std::make_pair(topic, msg));
}

template <class T_topic, class T_msg, class T_data>
uint8_t EventBus<T_topic, T_msg, T_data>::execute() {
  while (!mEvents.isEmpty()) {
    auto event = mEvents.hardPop();
    mListeners.forEach([&](EventListener<T_topic, T_msg, T_data> *listener) {
      if (listener->isListeningToEvent(event.first)) {
        listener->onEventReceived(event.first, event.second);
        yield();
      }
    });
  }
  return 0;
}
}  // namespace uniot

template class uniot::EventBus<unsigned int, int, Bytes>;
