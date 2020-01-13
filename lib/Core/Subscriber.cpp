/*
 * This is a part of the Uniot project.
 * Copyright (C) 2016-2020 Uniot <info.uniot@gmail.com>
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

#include <Arduino.h>
#include "Subscriber.h"
#include "Broker.h"

template<class T_topic, class T_msg>
Subscriber<T_topic, T_msg>::~Subscriber() {
  mBrokerQueue.forEach( [this](Broker<T_topic, T_msg>* broker){ broker->mSubscribers.removeOne(this); } );
}

template<class T_topic, class T_msg>
Subscriber<T_topic, T_msg>* Subscriber<T_topic, T_msg>::subscribe(T_topic topic) {
  mTopics.pushUnique(topic);
  return this;
}

template<class T_topic, class T_msg>
Subscriber<T_topic, T_msg>* Subscriber<T_topic, T_msg>::unsubscribe(T_topic topic) {
  mTopics.removeOne(topic);
  return this;
}

template<class T_topic, class T_msg>
bool Subscriber<T_topic, T_msg>::isSubscribed(T_topic topic) {
  return mTopics.contains(topic);
}

template<class T_topic, class T_msg>
void Subscriber<T_topic, T_msg>::connect(Broker<T_topic, T_msg>* broker) {
  if(broker) {
    broker->connect(this);
  }
}

template<class T_topic, class T_msg>
void Subscriber<T_topic, T_msg>::disconnect(Broker<T_topic, T_msg>* broker) {
  if(broker) {
    broker->disconnect(this);
  }
}

template class Subscriber<int, int>;