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

#include <cn-cbor.h>

namespace uniot {
class CBORObject;
class CBORArray {
  friend class CBORObject;

 public:
  CBORArray(CBORArray const&) = delete;
  void operator=(CBORArray const&) = delete;

  ~CBORArray() {
    mpContext = nullptr;
    mpMapNode = mpKey = mpArrayNode = nullptr;
  }

  cn_cbor_errback getLastError() {
    return mErr;
  }

  CBORArray* put(int value) {
    cn_cbor_array_append(mpArrayNode, cn_cbor_int_create(value, &mErr), &mErr);
    return this;
  }

  CBORArray* put(const char* value) {
    cn_cbor_array_append(mpArrayNode, cn_cbor_string_create(value, &mErr), &mErr);
    return this;
  }

  CBORObject& closeArray() {
    cn_cbor_map_put(mpMapNode, mpKey, mpArrayNode, &mErr);
    return *mpContext;
  }

 private:
  CBORArray(CBORObject* context, cn_cbor* mapNode, cn_cbor* key)
      : mpContext(context), mpMapNode(mapNode), mpKey(key) {
    mpArrayNode = cn_cbor_array_create(&mErr);
  }

  CBORObject* mpContext;
  cn_cbor* mpMapNode;
  cn_cbor* mpKey;
  cn_cbor* mpArrayNode;
  cn_cbor_errback mErr;
};
}  // namespace uniot