#pragma once

// Mono code also has define for GROUP_SIZE, so we need to wrap its usage here
#pragma push_macro("GROUP_SIZE")
#undef GROUP_SIZE
#include "../../external/google/sparsehash/sparse_hash_map.h"
#include "../../external/google/sparsehash/dense_hash_map.h"
#pragma pop_macro("GROUP_SIZE")

#include "KeyWrapper.h"

#include "os/FastReaderReaderWriterLock.h"
#if USE_NEW_HASHMAP
#include "../../external/hashmap/bytell_hash_map.h"
#endif

template<class Key, class T,
         class HashFcn = SPARSEHASH_HASH<Key>,
         class EqualKey = std::equal_to<Key>,
#if !USE_NEW_HASHMAP
         class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<std::pair<const KeyWrapper<Key>, T> > >
#else
         class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<std::pair<const KeyWrapperSimple<Key>, T> > >
		 //class Alloc = std::allocator<std::pair<const KeyWrapperPointerOnly<Key>, T> >
#endif
#if !USE_NEW_HASHMAP
class Il2CppHashMap : public GOOGLE_NAMESPACE::dense_hash_map<KeyWrapper<Key>, T, HashFcn, typename KeyWrapper<Key>::template EqualsComparer<EqualKey>, Alloc>
#else
class Il2CppHashMap : public ska::bytell_hash_map<KeyWrapperSimple<Key>, T, HashFcn, typename KeyWrapperSimple<Key>::template EqualsComparer<EqualKey>, Alloc>
#endif
{
private:
#if !USE_NEW_HASHMAP
typedef GOOGLE_NAMESPACE::dense_hash_map<KeyWrapper<Key>, T, HashFcn, typename KeyWrapper<Key>::template EqualsComparer<EqualKey>, Alloc> Base;
#else
    typedef ska::bytell_hash_map<KeyWrapperSimple<Key>, T, HashFcn, typename KeyWrapperSimple<Key>::template EqualsComparer<EqualKey>, Alloc> Base;
#endif

public:
    typedef typename Base::size_type size_type;
    typedef typename Base::hasher hasher;
    typedef typename Base::key_equal key_equal;
    typedef typename Base::key_type key_type;

    explicit Il2CppHashMap(size_type n = 0,
                           const hasher& hf = hasher(),
                           const EqualKey& eql = EqualKey()) :
        Base(n, hf, key_equal(eql))
    {
#if !USE_NEW_HASHMAP
        Base::set_deleted_key(key_type(key_type::KeyType_Deleted));
        Base::set_empty_key(key_type(key_type::KeyType_Empty));
#endif
    }

    template<class InputIterator>
    Il2CppHashMap(InputIterator f, InputIterator l,
                  size_type n = 0,
                  const hasher& hf = hasher(),
                  const EqualKey& eql = EqualKey()) :
        Base(f, l, n, hf, key_equal(eql))
    {
#if !USE_NEW_HASHMAP
        Base::set_deleted_key(key_type(key_type::KeyType_Deleted));
        Base::set_empty_key(key_type(key_type::KeyType_Empty));
#endif
    }

    void add(const key_type& key, const T& value)
    {
        Base::insert(std::make_pair(key, value));
    }
};

template<class Key, class T,
         class HashFcn = SPARSEHASH_HASH<Key>,
         class EqualKey = std::equal_to<Key>,
         class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<std::pair<const KeyWrapper<Key>, T> > >
class Il2CppReaderWriterLockedHashMap
{
public:
    typedef typename Il2CppHashMap<Key, T, HashFcn, EqualKey, Alloc>::key_type key_type;
    typedef typename Il2CppHashMap<Key, T, HashFcn, EqualKey, Alloc>::size_type size_type;
    typedef typename Il2CppHashMap<Key, T, HashFcn, EqualKey, Alloc>::const_iterator const_iterator;
    typedef typename Il2CppHashMap<Key, T, HashFcn, EqualKey, Alloc>::iterator iterator;
    typedef typename Il2CppHashMap<Key, T, HashFcn, EqualKey, Alloc>::hasher hasher;

    explicit Il2CppReaderWriterLockedHashMap(size_type n = 0,
                                             const hasher& hf = hasher(),
                                             const EqualKey& eql = EqualKey()) :
        hashMap(n, hf, eql)
    {
    }

    bool TryGet(const key_type& key, T* value)
    {
        il2cpp::os::FastReaderReaderWriterAutoSharedLock readerLock(&lock);
        const_iterator iter = hashMap.find(key);
        if (iter != hashMap.end())
        {
            *value = iter->second;
            return true;
        }
        return false;
    }

    bool Add(const key_type& key, const T& value)
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        return hashMap.insert(std::make_pair(key, value)).second;
    }

    void Clear()
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        hashMap.clear();
    }

    void Remove(const key_type& key)
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock readerLock(&lock);
        hashMap.erase(key);
    }

    // This function takes no locks, some other lock must be used to protect accesses
    iterator UnlockedBegin()
    {
        return hashMap.begin();
    }

    // This function takes no locks, some other lock must be used to protect accesses
    iterator UnlockedEnd()
    {
        return hashMap.end();
    }

private:
    il2cpp::os::FastReaderReaderWriterLock lock;
    Il2CppHashMap<Key, T, HashFcn, EqualKey, Alloc> hashMap;
};


template<class Key, class T,
    class HashFcn,
    class EqualKey = std::equal_to<Key>,
#if !USE_NEW_HASHMAP
    class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<std::pair<const KeyWrapper<Key>, T> >
#else
    class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<std::pair<const KeyWrapperSimple<Key>, T> >
    //class Alloc = std::allocator<std::pair<const KeyWrapper<Key>, T> >
#endif
        >
class Il2CppHashMapKeyWrapper :
#if !USE_NEW_HASHMAP
    public GOOGLE_NAMESPACE::dense_hash_map<KeyWrapper<Key>, T, HashFcn, typename KeyWrapper<Key>::template EqualsComparer<EqualKey>, Alloc>
#else
    public ska::bytell_hash_map<KeyWrapperSimple<Key>, T, HashFcn, typename KeyWrapperSimple<Key>::template EqualsComparer<EqualKey>, Alloc>
#endif
{
private:
#if !USE_NEW_HASHMAP
    typedef GOOGLE_NAMESPACE::dense_hash_map<KeyWrapper<Key>, T, HashFcn, typename KeyWrapper<Key>::template EqualsComparer<EqualKey>, Alloc> Base;
#else
    typedef ska::bytell_hash_map<KeyWrapperSimple<Key>, T, HashFcn, typename KeyWrapperSimple<Key>::template EqualsComparer<EqualKey>, Alloc> Base;
#endif

public:
    typedef typename Base::size_type size_type;
    typedef typename Base::hasher hasher;
    typedef typename Base::key_equal key_equal;
    typedef typename Base::key_type key_type;

    explicit Il2CppHashMapKeyWrapper(size_type n = 0,
        const hasher& hf = hasher(),
        const EqualKey& eql = EqualKey()) :
        Base(n, hf, key_equal(eql))
    {
#if !USE_NEW_HASHMAP
        Base::set_empty_key(key_type(key_type::KeyType_Empty));
        Base::set_deleted_key(key_type(key_type::KeyType_Deleted));
#else
        //Base::set_deleted_key(key_type(key_type::KeyType_Deleted));
#endif
    }

    template<class InputIterator>
    Il2CppHashMapKeyWrapper(InputIterator f, InputIterator l,
        size_type n = 0,
        const hasher& hf = hasher(),
        const EqualKey& eql = EqualKey()) :
        Base(f, l, n, hf, key_equal(eql))
    {
#if !USE_NEW_HASHMAP
        Base::set_empty_key(key_type(key_type::KeyType_Empty));
        Base::set_deleted_key(key_type(key_type::KeyType_Deleted));
#else
        //Base::set_deleted_key(key_type(key_type::KeyType_Deleted));
#endif
    }

    void add(const key_type& key, const T& value)
    {
        Base::insert(std::make_pair(key, value));
    }
};



template<class Key, class T,
    class HashFcn,
    class EqualKey = std::equal_to<Key>,
    class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<std::pair<const KeyWrapperPointerOnly<Key>, T> >
        >
class Il2CppHashMapMemory :
    public GOOGLE_NAMESPACE::sparse_hash_map<KeyWrapperPointerOnly<Key>, T, HashFcn, typename KeyWrapperPointerOnly<Key>::template EqualsComparer<EqualKey>, Alloc>
{
private:
    typedef GOOGLE_NAMESPACE::sparse_hash_map<KeyWrapperPointerOnly<Key>, T, HashFcn, typename KeyWrapperPointerOnly<Key>::template EqualsComparer<EqualKey>, Alloc> Base;
public:
    typedef typename Base::size_type size_type;
    typedef typename Base::hasher hasher;
    typedef typename Base::key_equal key_equal;
    typedef typename Base::key_type key_type;

    explicit Il2CppHashMapMemory(size_type n = 0,
        const hasher& hf = hasher(),
        const EqualKey& eql = EqualKey()) :
        Base(n, hf, key_equal(eql))
    {
        //Base::set_empty_key(key_type(key_type::KeyType_Empty));
        Base::set_deleted_key(key_type(key_type::KeyType_Deleted));
    }

    template<class InputIterator>
    Il2CppHashMapMemory(InputIterator f, InputIterator l,
        size_type n = 0,
        const hasher& hf = hasher(),
        const EqualKey& eql = EqualKey()) :
        Base(f, l, n, hf, key_equal(eql))
    {
        //Base::set_empty_key(key_type(key_type::KeyType_Empty));
        Base::set_deleted_key(key_type(key_type::KeyType_Deleted));
    }

    void add(const key_type& key, const T& value)
    {
        Base::insert(std::make_pair(key, value));
    }
};


template<class Key, class T,
         class HashFcn = SPARSEHASH_HASH<Key>,
         class EqualKey = std::equal_to<Key>,
         class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<std::pair<const KeyWrapperPointerOnly<Key>, T>> >
class Il2CppReaderWriterLockedHashMapMemory
{
public:
    typedef typename Il2CppHashMapMemory<Key, T, HashFcn, EqualKey, Alloc>::key_type key_type;
    typedef typename Il2CppHashMapMemory<Key, T, HashFcn, EqualKey, Alloc>::size_type size_type;
    typedef typename Il2CppHashMapMemory<Key, T, HashFcn, EqualKey, Alloc>::const_iterator const_iterator;
    typedef typename Il2CppHashMapMemory<Key, T, HashFcn, EqualKey, Alloc>::iterator iterator;
    typedef typename Il2CppHashMapMemory<Key, T, HashFcn, EqualKey, Alloc>::hasher hasher;

    explicit Il2CppReaderWriterLockedHashMapMemory(size_type n = 0,
                                             const hasher& hf = hasher(),
                                             const EqualKey& eql = EqualKey()) :
        hashMap(n, hf, eql)
    {
    }

    bool TryGet(const key_type& key, T* value)
    {
        il2cpp::os::FastReaderReaderWriterAutoSharedLock readerLock(&lock);
        const_iterator iter = hashMap.find(key);
        if (iter != hashMap.end())
        {
            *value = iter->second;
            return true;
        }
        return false;
    }

    bool Add(const key_type& key, const T& value)
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        return hashMap.insert(std::make_pair(key, value)).second;
    }

    void Clear()
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        hashMap.clear();
    }

    void Remove(const key_type& key)
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock readerLock(&lock);
        hashMap.erase(key);
    }

    // This function takes no locks, some other lock must be used to protect accesses
    iterator UnlockedBegin()
    {
        return hashMap.begin();
    }

    // This function takes no locks, some other lock must be used to protect accesses
    iterator UnlockedEnd()
    {
        return hashMap.end();
    }

private:
    il2cpp::os::FastReaderReaderWriterLock lock;
    Il2CppHashMapMemory<Key, T, HashFcn, EqualKey, Alloc> hashMap;
};

