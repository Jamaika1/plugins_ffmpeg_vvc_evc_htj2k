#pragma once
/*
This program is free software; you can redistribute it and /or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

Helper structures for frame properties a.k.a VSMap.
Based on VapourSynth API4, copyright (c) Fredrik Mellbin

*/
#include <map>
#include <mutex>
#include <string>
#include "avisynth.h"
#include <atomic>
#include <vector>
#include <memory>
#include <cassert>

// VS node ~ Avisynth clip, VSMap-AVSMap
// See also in Avisynth.cpp

// INTRUSIVE_PTR_H 

#include <algorithm>

template<typename T>
class vs_intrusive_ptr {
private:
  T* obj;
public:
  vs_intrusive_ptr(T* ptr = nullptr, bool add_ref = false) noexcept {
    obj = ptr;
    if (add_ref && obj)
      obj->add_ref();
  }

  vs_intrusive_ptr(const vs_intrusive_ptr& ptr) noexcept {
    obj = ptr.obj;
    if (obj)
      obj->add_ref();
  }

  vs_intrusive_ptr(vs_intrusive_ptr&& ptr) noexcept {
    obj = ptr.obj;
    ptr.obj = nullptr;
  }

  ~vs_intrusive_ptr() noexcept {
    if (obj)
      obj->release();
  }

  vs_intrusive_ptr& operator=(vs_intrusive_ptr const& ptr) noexcept {
    if (obj)
      obj->release();
    obj = ptr.obj;
    if (obj)
      obj->add_ref();
    return *this;
  }

  T* operator->() const noexcept {
    return obj;
  }

  T& operator*() const noexcept {
    return *obj;
  }

  operator bool() const noexcept {
    return !!obj;
  }

  T* get() const noexcept {
    return obj;
  }

  void reset() noexcept {
    if (obj) {
      obj->release();
      obj = nullptr;
    }
  }

  void swap(vs_intrusive_ptr& ptr) noexcept {
    std::swap(obj, ptr.obj);
  }
};

#define AVS_NOEXCEPT noexcept

// enums for frame property functions

// VS: typedef enum VSPropertyType
typedef enum AVSPropertyType {
  PROPERTYTYPE_UNSET = 0, // ptUnset = 0,
  PROPERTYTYPE_INT = 1, // ptInt = 1,
  PROPERTYTYPE_FLOAT = 2, //  ptFloat = 2,
  PROPERTYTYPE_DATA = 3, // ptData = 3,
  //  ptFunction = 4, // Avisynth: functions not supported here
  PROPERTYTYPE_CLIP = 5, // ptVideoNode = 5,
  //  ptAudioNode = 6, // Avisynth: no special audio clip
  PROPERTYTYPE_FRAME = 7, //  ptVideoFrame = 7,
  //  ptAudioFrame = 8 // Avisynth: no special audio frame
} AVSPropertyType;

class VSArrayBase {
protected:
  std::atomic<long> refcount;
  AVSPropertyType ftype;
  size_t fsize = 0;
  explicit VSArrayBase(AVSPropertyType type) : refcount(1), ftype(type) {}
  virtual ~VSArrayBase() {}
public:
  AVSPropertyType type() const {
    return ftype;
  }

  size_t size() const {
    return fsize;
  }

  bool unique() const noexcept {
    return (refcount == 1);
  }

  void add_ref() noexcept {
    ++refcount;
  }

  void release() noexcept {
    assert(refcount > 0);
    if (--refcount == 0)
      delete this;
  }

  virtual VSArrayBase* copy() const noexcept = 0;
};

typedef vs_intrusive_ptr<VSArrayBase> PVSArrayBase;

template<typename T, AVSPropertyType propType>
class VSArray final : public VSArrayBase {
private:
  T singleData = {};
  std::vector<T> data;
public:
  explicit VSArray() noexcept : VSArrayBase(propType) {}

  explicit VSArray(const VSArray& other) noexcept : VSArrayBase(other.ftype) {
    fsize = other.fsize;
    if (fsize == 1)
      singleData = other.singleData;
    else if (fsize > 1)
      data = other.data;
  }

  explicit VSArray(const T* val, size_t count) noexcept : VSArrayBase(propType) { // only enable for POD types
    fsize = count;
    if (count == 1) {
      singleData = *val;
    }
    else {
      data.resize(count);
      memcpy(data.data(), val, sizeof(T) * count);
    }
  }

  virtual VSArrayBase* copy() const noexcept {
    return new VSArray(*this);
  }

  const T* getDataPointer() const noexcept { // only enable for POD types
    if (fsize == 1)
      return &singleData;
    else
      return data.data();
  }

  void push_back(const T& val) noexcept {
    if (fsize == 0) {
      singleData = val;
    }
    else if (fsize == 1) {
      data.reserve(8);
      data.push_back(std::move(singleData));
      data.push_back(val);
    }
    else {
      if (data.capacity() == data.size())
        data.reserve(data.capacity() * 2);
      data.push_back(val);
    }
    fsize++;
  }

  const T& at(size_t pos) const noexcept {
    assert(pos < fsize);
    if (fsize == 1)
      return singleData;
    else
      return data.at(pos);
  }
};

// variant types
class VSMapData {
public:
  AVSPropDataTypeHint typeHint;
  std::string data;
};

typedef VSArray<int64_t, AVSPropertyType::PROPERTYTYPE_INT> VSIntArray; // ptInt
typedef VSArray<double, AVSPropertyType::PROPERTYTYPE_FLOAT> VSFloatArray; // ptFloat
typedef VSArray<VSMapData, AVSPropertyType::PROPERTYTYPE_DATA> VSDataArray; // ptData
typedef VSArray<PClip, AVSPropertyType::PROPERTYTYPE_CLIP> VSVideoNodeArray; // ptVideoNode
typedef VSArray<PVideoFrame, AVSPropertyType::PROPERTYTYPE_FRAME> VSVideoFrameArray; // ptVideoFrame
//typedef VSArray<PFunction, ptFunction> VSFunctionArray;


typedef std::vector<int64_t> IntList;
typedef std::vector<double> FloatList;
typedef std::vector<VSMapData> DataList;
typedef std::vector<PClip> ClipList;
typedef std::vector<PVideoFrame> FrameList;
//typedef std::vector<PFunction> FuncList;


class VSMapStorage {
private:
  std::atomic<long> refcount;
public:
  std::map<std::string, PVSArrayBase> data;
  bool error;

  explicit VSMapStorage() : refcount(1), error(false) {}

  explicit VSMapStorage(const VSMapStorage& s) : refcount(1), data(s.data), error(s.error) {
  }

  void clear() noexcept {
    data.clear();
    error = false;
  }

  bool unique() noexcept {
    return (refcount == 1);
  };

  void add_ref() noexcept {
    ++refcount;
  }

  void release() noexcept {
    assert(refcount > 0);
    if (--refcount == 0)
      delete this;
  }
};

typedef vs_intrusive_ptr<VSMapStorage> PVSMapStorage;

// This one is referenced in avisynth.h.
// For avoiding dual plugin name collisions, renamed VSMap->AVSMap
struct AVSMap {
private:
  PVSMapStorage data;
public:
  AVSMap(const AVSMap* map = nullptr) : data(map ? map->data : new VSMapStorage()) {
  }

  AVSMap& operator=(const AVSMap& map) {
    data = map.data;
    return *this;
  }

  bool detach() {
    if (!data->unique()) {
      data = new VSMapStorage(*data);
      return true;
    }
    return false;
  }

  VSArrayBase* find(const std::string& key) const {
    auto it = data->data.find(key);
    return (it == data->data.end()) ? nullptr : it->second.get();
  }

  VSArrayBase* detach(const std::string& key) {
    detach();
    auto it = data->data.find(key);
    if (it != data->data.end()) {
      if (!it->second->unique())
        it->second = it->second->copy();
      return it->second.get();
    }
    return nullptr;
  }

  bool erase(const std::string& key) {
    auto it = data->data.find(key);
    if (it != data->data.end()) {
      if (detach())
        it = data->data.find(key);
      data->data.erase(it);
      return true;
    }
    return false;
  }

  void insert(const std::string& key, VSArrayBase* val) {
    detach();
    auto it = data->data.find(key);
    if (it != data->data.end()) {
      it->second = val;
    }
    else {
      data->data.insert(std::make_pair(key, val));
    }
  }

  void copy(const AVSMap* src) {
    if (src == this)
      return;

    detach();
    for (auto& iter : src->data->data)
      data->data[iter.first] = iter.second;
  }

  size_t size() const {
    return data->data.size();
  }

  void clear() {
    if (data->unique())
      data->clear();
    else
      data = new VSMapStorage();
  }

  const char* key(size_t n) const {
    if (n >= size())
      return nullptr;
    auto iter = data->data.cbegin();
    std::advance(iter, n);
    return iter->first.c_str();
  }

  void setError(const std::string& errMsg) {
    clear();
    VSDataArray* arr = new VSDataArray();
    arr->push_back({ AVSPropDataTypeHint::PROPDATATYPEHINT_UTF8, errMsg }); // dtUtf8
    data->data.insert(std::make_pair("_Error", arr));
    data->error = true;
  }

  bool hasError() const {
    return data->error;
  }

  const char* getErrorMessage() const {
    if (data->error) {
      return reinterpret_cast<VSDataArray*>(data->data.at("_Error").get())->at(0).data.c_str();
    }
    else {
      return nullptr;
    }
  }

  //bool isV3Compatible() const noexcept; // VS special
};
