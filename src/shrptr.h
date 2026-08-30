#ifndef SHRPTR_H
#define SHRPTR_H

/*
 * Minimal, C++98-compatible replacement for std::shared_ptr, covering only
 * the operations actually exercised elsewhere in this project.
 */

namespace mrtp {

namespace detail {

class RefCount
{
public:
    RefCount() : count_(1) {}

    void add_ref()
    {
#ifdef _OPENMP
        #pragma omp atomic
        ++count_;
#else
        ++count_;
#endif
    }

    // Decrements the count and returns the value after decrementing.
    long release()
    {
        long result;
#ifdef _OPENMP
        #pragma omp atomic capture
        result = --count_;
#else
        result = --count_;
#endif
        return result;
    }

private:
    long count_;

    // Exactly one RefCount per managed object - never copied.
    RefCount(const RefCount&);
    RefCount& operator=(const RefCount&);
};

} // namespace detail


template <typename T>
class SharedPtr
{
public:
    SharedPtr() : ptr_(0), refcount_(0) {}

    explicit SharedPtr(T* ptr) : ptr_(ptr), refcount_(ptr ? new detail::RefCount() : 0) {}

    SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), refcount_(other.refcount_)
    {
        if (refcount_)
            refcount_->add_ref();
    }

    ~SharedPtr()
    {
        if (refcount_ && refcount_->release() == 0)
        {
            delete ptr_;
            delete refcount_;
        }
    }

    SharedPtr& operator=(const SharedPtr& other)
    {
        SharedPtr temp(other);
        swap(temp);
        return *this;
    }

    T* get() const { return ptr_; }
    T* operator->() const { return ptr_; }

    // Safe-bool idiom: supports `if (ptr)` / `if (!ptr)` without allowing
    // ptr to silently convert to int or to an unrelated pointer type.
    typedef T* SharedPtr::*unspecified_bool_type;

    operator unspecified_bool_type() const
    {
        return ptr_ ? &SharedPtr::ptr_ : 0;
    }

private:
    void swap(SharedPtr& other)
    {
        T* tmp_ptr = ptr_;
        ptr_ = other.ptr_;
        other.ptr_ = tmp_ptr;

        detail::RefCount* tmp_refcount = refcount_;
        refcount_ = other.refcount_;
        other.refcount_ = tmp_refcount;
    }

    T* ptr_;
    detail::RefCount* refcount_;
};

// C++98 equivalent for `typedef SharedPtr shared_ptr`
template <typename T>
class shared_ptr : public SharedPtr<T>
{
public:
    shared_ptr() {}
    explicit shared_ptr(T* ptr) : SharedPtr<T>(ptr) {}
    shared_ptr(const SharedPtr<T>& other) : SharedPtr<T>(other) {}
};

} // namespace mrtp

#endif // SHRPTR_H
