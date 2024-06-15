#pragma once

#define USE_NEW_HASHMAP (IL2CPP_TARGET_IOS || (IL2CPP_TARGET_ANDROID && IL2CPP_SIZEOF_VOID_P == 4))


template<class T>
struct KeyWrapper
{
    typedef T wrapped_type;
    typedef KeyWrapper<T> self_type;

    enum KeyTypeEnum { KeyType_Normal, KeyType_Empty, KeyType_Deleted };

    KeyTypeEnum type;
    T key;

    KeyWrapper() : type(KeyType_Normal), key(T()) {}
    KeyWrapper(KeyTypeEnum type_) : type(type_), key(T()) {}
    KeyWrapper(const T& key_) : key(key_), type(KeyType_Normal) {}
    KeyWrapper(const self_type& other) : type(other.type), key(other.key) {}

    operator const T&() const { return key; }
    bool isNormal() const { return (type == KeyType_Normal); }

    template<typename KeyComparer>
    struct EqualsComparer
    {
        EqualsComparer(KeyComparer keyComparer) :
            m_KeyComparer(keyComparer)
        {
        }

        bool operator()(const KeyWrapper<T>& left, const KeyWrapper<T>& right) const
        {
            if (left.type != right.type)
                return false;

            if (!left.isNormal())
                return true;

            return m_KeyComparer(left.key, right.key);
        }

    private:
        KeyComparer m_KeyComparer;
    };
};



template<class T>
struct KeyWrapperSimple
{
    typedef T wrapped_type;
    typedef KeyWrapperSimple<T> self_type;

    T key;

    KeyWrapperSimple() : key(T()) {}
    KeyWrapperSimple(const T& key_) : key(key_) {}
    KeyWrapperSimple(const self_type& other) : key(other.key) {}

    operator const T&() const { return key; }

    template<typename KeyComparer>
    struct EqualsComparer
    {
        EqualsComparer(KeyComparer keyComparer) :
            m_KeyComparer(keyComparer)
        {
        }

        bool operator()(const KeyWrapperSimple<T>& left, const KeyWrapperSimple<T>& right) const
        {
            return m_KeyComparer(left.key, right.key);
        }

    private:
        KeyComparer m_KeyComparer;
    };
};


template<class T>
struct KeyWrapperPointerOnly
{
    typedef T wrapped_type;
    typedef KeyWrapperPointerOnly<T> self_type;

    enum KeyTypeEnum
    {
        KeyType_Normal = 0,
        KeyType_Empty = 1,
        KeyType_Deleted = 3
    };

    T key;

    KeyWrapperPointerOnly() : key(T()) {}
    KeyWrapperPointerOnly(KeyTypeEnum type_) : key((T)type_) {}
    KeyWrapperPointerOnly(const T& key_) : key(key_) {}
    KeyWrapperPointerOnly(const self_type& other) : key(other.key) {}

    operator const T&() const { return key; }

    bool isNormal() const
    {
        return (GetType() == KeyType_Normal);
    }

    KeyTypeEnum GetType() const
    {
        if (key == (T)KeyType_Empty)
            return KeyType_Empty;
        else if (key == (T)KeyType_Deleted)
            return KeyType_Deleted;
        else
            return KeyType_Normal;
    }

    template<typename KeyComparer>
    struct EqualsComparer
    {
        EqualsComparer(KeyComparer keyComparer) :
            m_KeyComparer(keyComparer)
        {
        }

        bool operator()(const KeyWrapperPointerOnly<T>& left, const KeyWrapperPointerOnly<T>& right) const
        {
            if (left.GetType() != right.GetType())
                return false;

            if (!left.isNormal())
                return true;

            return m_KeyComparer(left.key, right.key);
        }

    private:
        KeyComparer m_KeyComparer;
    };
};


