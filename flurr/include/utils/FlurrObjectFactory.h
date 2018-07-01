#pragma once

#include "FlurrDefines.h"

#include <cassert>
#include <functional>
#include <map>
#include <string>

namespace flurr {

template <class BaseT, typename TypeIdT = uint32_t, TypeIdT InvalidTypeId = -1, typename ...Args>
class ObjectFactory {

public:

  using CreateCallbackType = std::function<BaseT*(Args...)>;

  static constexpr TypeIdT kInvalidTypeId = InvalidTypeId;

  void RegisterClass(TypeIdT type_id, const std::string& type_name, CreateCallbackType create_callback) {
    assert(InvalidTypeId != type_id);
    assert(type_ids_by_name_.count(type_name) <= 0);

    create_callbacks_[type_id] = create_callback;
    type_ids_by_name_[type_name] = type_id;
  }

  void UnregisterClass(TypeIdT type_id) {
    for (auto&& type_id_name : type_ids_by_name_) {
      if (type_id_name.second == type_id) {
        type_ids_by_name.erase(type_id_name.first);
        break;
      }
    }

    create_callbacks_.erase(type_id);
  }

  void UnregisterClass(const std::string& type_name) {
    auto&& type_id_name_it = type_ids_by_name_.find(type_name);
    if (type_ids_by_name.end() == type_id_name_it)
      return;

    create_callbacks_.erase(type_id_name_it->second);
    type_ids_by_name_.erase(type_id_name_it);
  }

  bool IsClassRegistered(TypeIdT type_id) const {
    return create_callbacks_.count(type_id) > 0;
  }

  bool IsClassRegistered(const std::string& type_name) const {
    return type_ids_by_name_.count(type_name) > 0;
  }

  TypeIdT GetTypeIdByName(const std::string& type_name) const {
    auto&& type_id_name_it = type_ids_by_name_.find(type_name);
    return type_ids_by_name_.end() != type_id_name_it ? type_id_name_it->second : kInvalidTypeId;
  }

  const std::map<std::string, TypeIdT>& GetRegisteredClassTypes() const {
    return type_ids_by_name_;
  }

  BaseT* CreateObject(TypeIdT type_id, Args... args) {
    auto&& create_callback_it = create_callbacks_.find(type_id);
    return create_callbacks_.end() != create_callback_it ?
      create_callback_it->second(args...) : nullptr;
  }

  BaseT* CreateObject(const std::string& type_name, Args... args) {
    TypeIdT type_id = GetTypeIdByName(type_name);
    return kInvalidTypeId != type_id ? CreateObject(type_id, args...) : nullptr;
  }

private:

  std::map<TypeIdT, CreateCallbackType> create_callbacks_;
  std::map<std::string, TypeIdT> type_ids_by_name_;
};

} // namespace flurr
