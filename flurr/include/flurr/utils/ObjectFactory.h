#pragma once

#include "flurr/FlurrDefines.h"

#include <cassert>
#include <functional>
#include <map>
#include <string>

namespace flurr
{

template <class BaseT, typename TypeIdT = uint32_t, TypeIdT InvalidTypeId = -1, typename ...Args>
class ObjectFactory
{

public:

  using CreateCallbackType = std::function<BaseT*(Args...)>;

  static constexpr TypeIdT kInvalidTypeId = InvalidTypeId;

  void registerClass(TypeIdT a_typeId, const std::string& a_typeName, CreateCallbackType a_createCallback)
  {
    assert(InvalidTypeId != a_typeId);
    assert(m_typeIdsByName.count(a_typeName) <= 0);

    m_createCallbacks[a_typeId] = a_createCallback;
    m_typeIdsByName[a_typeName] = a_typeId;
  }

  void unregisterClass(TypeIdT a_typeId)
  {
    for (auto&& typeIdName : m_typeIdsByName)
    {
      if (typeIdName.second == a_typeId)
      {
        m_typeIdsByName.erase(typeIdName.first);
        break;
      }
    }

    m_createCallbacks.erase(a_typeId);
  }

  void unregisterClass(const std::string& a_typeName)
  {
    auto&& typeIdNameIt = m_typeIdsByName.find(a_typeName);
    if (type_ids_by_name.end() == typeIdNameIt)
      return;

    m_createCallbacks.erase(typeIdNameIt->second);
    m_typeIdsByName.erase(typeIdNameIt);
  }

  bool isClassRegistered(TypeIdT a_typeId) const
  {
    return m_createCallbacks.count(a_typeId) > 0;
  }

  bool isClassRegistered(const std::string& a_typeName) const
  {
    return m_typeIdsByName.count(a_typeName) > 0;
  }

  TypeIdT getTypeIdByName(const std::string& a_typeName) const
  {
    auto&& typeIdNameIt = m_typeIdsByName.find(a_typeName);
    return m_typeIdsByName.end() != typeIdNameIt ? typeIdNameIt->second : kInvalidTypeId;
  }

  const std::map<std::string, TypeIdT>& getRegisteredClassTypes() const
  {
    return m_typeIdsByName;
  }

  BaseT* createObject(TypeIdT a_typeId, Args... args)
 {
    auto&& createCallbackIt = m_createCallbacks.find(a_typeId);
    return m_createCallbacks.end() != createCallbackIt ?
      createCallbackIt->second(args...) : nullptr;
  }

  BaseT* createObject(const std::string& a_typeName, Args... args)
  {
    TypeIdT typeId = getTypeIdByName(a_typeName);
    return kInvalidTypeId != typeId ? createObject(typeId, args...) : nullptr;
  }

private:

  std::map<TypeIdT, CreateCallbackType> m_createCallbacks;
  std::map<std::string, TypeIdT> m_typeIdsByName;
};

} // namespace flurr
