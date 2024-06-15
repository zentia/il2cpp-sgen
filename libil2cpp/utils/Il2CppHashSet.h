#pragma once

// Mono code also has define for GROUP_SIZE, so we need to wrap its usage here
#pragma push_macro("GROUP_SIZE")
#undef GROUP_SIZE
#include "../../external/google/sparsehash/sparse_hash_set.h"
#include "../../external/google/sparsehash/dense_hash_set.h"
#pragma pop_macro("GROUP_SIZE")

#include "KeyWrapper.h"
#include "os/FastReaderReaderWriterLock.h"
#if USE_NEW_HASHMAP
#include "../../external/hashmap/bytell_hash_map.h"
#endif

template<class Value,
         class HashFcn = SPARSEHASH_HASH<Value>,
         class EqualKey = std::equal_to<Value>,
#if !USE_NEW_HASHMAP
         class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<KeyWrapper<Value> > >
#else
         class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<KeyWrapperSimple<Value> > >
#endif
#if !USE_NEW_HASHMAP
class Il2CppHashSet : public GOOGLE_NAMESPACE::dense_hash_set<KeyWrapper<Value>, HashFcn, typename KeyWrapper<Value>::template EqualsComparer<EqualKey>, Alloc>
#else
class Il2CppHashSet : public ska::bytell_hash_set<KeyWrapperSimple<Value>, HashFcn, typename KeyWrapperSimple<Value>::template EqualsComparer<EqualKey>, Alloc>
#endif
{
private:
#if !USE_NEW_HASHMAP
typedef GOOGLE_NAMESPACE::dense_hash_set<KeyWrapper<Value>, HashFcn, typename KeyWrapper<Value>::template EqualsComparer<EqualKey>, Alloc> Base;
#else
    typedef ska::bytell_hash_set<KeyWrapperSimple<Value>, HashFcn, typename KeyWrapperSimple<Value>::template EqualsComparer<EqualKey>, Alloc> Base;
#endif

public:
    typedef typename Base::size_type size_type;
    typedef typename Base::hasher hasher;
    typedef typename Base::key_equal key_equal;
    typedef typename Base::key_type key_type;

    explicit Il2CppHashSet(size_type n = 0,
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
    Il2CppHashSet(InputIterator f, InputIterator l,
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
};

template<class Value,
         class HashFcn = SPARSEHASH_HASH<Value>,
         class EqualKey = std::equal_to<Value>,
         class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<KeyWrapper<Value>> >
class Il2CppReaderWriterLockedHashSet
{
public:
    typedef typename Il2CppHashSet<Value, HashFcn, EqualKey, Alloc>::key_type key_type;
    typedef typename Il2CppHashSet<Value, HashFcn, EqualKey, Alloc>::size_type size_type;
    typedef typename Il2CppHashSet<Value, HashFcn, EqualKey, Alloc>::const_iterator const_iterator;
    typedef typename Il2CppHashSet<Value, HashFcn, EqualKey, Alloc>::iterator iterator;
    typedef typename Il2CppHashSet<Value, HashFcn, EqualKey, Alloc>::hasher hasher;

    explicit Il2CppReaderWriterLockedHashSet(size_type n = 0,
                                             const hasher& hf = hasher(),
                                             const EqualKey& eql = EqualKey()) :
        hashSet(n, hf, eql)
    {
    }

    bool TryGet(const Value& findValue, Value* value)
    {
        il2cpp::os::FastReaderReaderWriterAutoSharedLock readerLock(&lock);
        const_iterator iter = hashSet.find(findValue);
        if (iter != hashSet.end())
        {
            *value = *iter;
            return true;
        }
        return false;
    }

    bool Add(const Value& value)
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        return hashSet.insert(value).second;
    }

    Value GetOrAdd(const Value& value)
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        auto inserted = hashSet.insert(value);
        if (inserted.second)
            return value;
        return *(inserted.first);
    }

    void Clear()
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        hashSet.clear();
    }

    void Resize(size_t size)
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        hashSet.resize(size);
    }

private:
    il2cpp::os::FastReaderReaderWriterLock lock;
    Il2CppHashSet<Value, HashFcn, EqualKey, Alloc> hashSet;
    
};


template<class Value,
class HashFcn,
class EqualKey = std::equal_to<Value>,
class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<KeyWrapperPointerOnly<Value> >
        >
class Il2CppHashSetMemory :
    public GOOGLE_NAMESPACE::sparse_hash_set<KeyWrapperPointerOnly<Value>, HashFcn, typename KeyWrapperPointerOnly<Value>::template EqualsComparer<EqualKey>, Alloc>
{
private:
	typedef GOOGLE_NAMESPACE::sparse_hash_set<KeyWrapperPointerOnly<Value>, HashFcn, typename KeyWrapperPointerOnly<Value>::template EqualsComparer<EqualKey>, Alloc> Base;
public:
	typedef typename Base::size_type size_type;
	typedef typename Base::hasher hasher;
	typedef typename Base::key_equal key_equal;
	typedef typename Base::key_type key_type;

	explicit Il2CppHashSetMemory(size_type n = 0,
		const hasher& hf = hasher(),
		const EqualKey& eql = EqualKey()) :
	Base(n, hf, key_equal(eql))
	{
		//Base::set_empty_key(key_type(key_type::KeyType_Empty));
		Base::set_deleted_key(key_type(key_type::KeyType_Deleted));
	}

	template<class InputIterator>
	Il2CppHashSetMemory(InputIterator f, InputIterator l,
		size_type n = 0,
		const hasher& hf = hasher(),
		const EqualKey& eql = EqualKey()) :
	Base(f, l, n, hf, key_equal(eql))
	{
		//Base::set_empty_key(key_type(key_type::KeyType_Empty));
		Base::set_deleted_key(key_type(key_type::KeyType_Deleted));
	}
};



template<class Value,
         class HashFcn = SPARSEHASH_HASH<Value>,
         class EqualKey = std::equal_to<Value>,
         class Alloc = GOOGLE_NAMESPACE::libc_allocator_with_realloc<KeyWrapperPointerOnly<Value>> >
class Il2CppReaderWriterLockedHashSetMemory
{
public:
    typedef typename Il2CppHashSetMemory<Value, HashFcn, EqualKey, Alloc>::key_type key_type;
    typedef typename Il2CppHashSetMemory<Value, HashFcn, EqualKey, Alloc>::size_type size_type;
    typedef typename Il2CppHashSetMemory<Value, HashFcn, EqualKey, Alloc>::const_iterator const_iterator;
    typedef typename Il2CppHashSetMemory<Value, HashFcn, EqualKey, Alloc>::iterator iterator;
    typedef typename Il2CppHashSetMemory<Value, HashFcn, EqualKey, Alloc>::hasher hasher;

    explicit Il2CppReaderWriterLockedHashSetMemory(size_type n = 0,
                                             const hasher& hf = hasher(),
                                             const EqualKey& eql = EqualKey()) :
        hashSet(n, hf, eql)
    {
    }

    bool TryGet(const Value& findValue, Value* value)
    {
        il2cpp::os::FastReaderReaderWriterAutoSharedLock readerLock(&lock);
        const_iterator iter = hashSet.find(findValue);
        if (iter != hashSet.end())
        {
            *value = *iter;
            return true;
        }
        return false;
    }

    bool Add(const Value& value)
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        return hashSet.insert(value).second;
    }

    Value GetOrAdd(const Value& value)
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        auto inserted = hashSet.insert(value);
        if (inserted.second)
            return value;
        return *(inserted.first);
    }

    void Clear()
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        hashSet.clear();
    }

    void Resize(size_t size)
    {
        il2cpp::os::FastReaderReaderWriterAutoExclusiveLock writerLock(&lock);
        hashSet.resize(size);
    }

private:
    il2cpp::os::FastReaderReaderWriterLock lock;
    Il2CppHashSetMemory<Value, HashFcn, EqualKey, Alloc> hashSet;
    
};

