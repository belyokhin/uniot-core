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

#include <Bytes.h>
#include <ClearQueue.h>
#include <DataChannels.h>
#include <IExecutor.h>

#include "IEventBusConnectionKit.h"

namespace uniot {
template <class T_topic, class T_msg, class T_data>
class EventEntity;

template <class T_topic, class T_msg, class T_data>
class EventEmitter;

template <class T_topic, class T_msg, class T_data>
class EventListener;

template <class T_topic, class T_msg, class T_data>
class EventBus : public uniot::IExecutor {
  friend class EventEntity<T_topic, T_msg, T_data>;

 public:
  EventBus(unsigned int id) : mId(id) {}
  virtual ~EventBus();

  unsigned int getId() { return mId; }

  void registerKit(IEventBusConnectionKit<T_topic, T_msg, T_data> &connection);
  void unregisterKit(IEventBusConnectionKit<T_topic, T_msg, T_data> &connection);

  bool registerEntity(EventEntity<T_topic, T_msg, T_data> *entity);
  void unregisterEntity(EventEntity<T_topic, T_msg, T_data> *entity);

  bool openDataChannel(T_topic topic, size_t limit);
  bool closeDataChannel(T_topic topic);
  bool sendDataToChannel(T_topic topic, T_data data);
  T_data receiveDataFromChannel(T_topic topic);
  bool isDataChannelEmpty(T_topic topic);

  void emitEvent(T_topic topic, T_msg msg);
  virtual void execute(short _) override;

 private:
  ClearQueue<EventEntity<T_topic, T_msg, T_data> *> mEntities; // TODO: need real set; std::set is broken into esp xtensa sdk
  ClearQueue<Pair<T_topic, T_msg>> mEvents;

  DataChannels<T_topic, T_data> mDataChannels;

  unsigned int mId;
};

using CoreEventBus = EventBus<unsigned int, int, Bytes>;
}  // namespace uniot
