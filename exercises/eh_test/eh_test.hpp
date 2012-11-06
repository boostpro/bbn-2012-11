// Copyright David Abrahams 2010. Distributed under the Boost
// Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <memory>

namespace eh_test
{
  
long const magic[] = { 0xDEADBEEF, 0x71077345 };

std::size_t allocs = 0;
int allocs_before_exception = -1;

int disable_exceptions()
{
    int r = allocs_before_exception;
    allocs_before_exception = -1;
    return r;
}

void enable_exceptions(int a)
{
    allocs_before_exception = a;
}
}

void* operator new(std::size_t n) throw(std::bad_alloc)
{
    using namespace eh_test;
    if (allocs_before_exception-- == 0)
        throw std::bad_alloc();
    
    ++allocs;
    char* r = (char*)std::malloc( n + 2*sizeof(magic)+sizeof(n) );
    *(std::size_t*)r = n;
    
    std::copy((char*)magic, (char*)magic + sizeof(magic), r + sizeof(n));
    std::copy((char*)magic, (char*)magic + sizeof(magic), r + sizeof(n) + sizeof(magic) + n);
    
    return r + sizeof(magic) + sizeof(n);
}

void operator delete(void* p) throw()
{
    if (p == 0) return;
    using namespace eh_test;
    assert(allocs > 0);
    --allocs;
    char* const q = (char*)p - sizeof(magic) - sizeof(std::size_t);
    const char* const db0 = q + sizeof(std::size_t);
    std::size_t const n = *(std::size_t*)q;
    const char* const db1 = (char*)p + n;
        
    assert(std::equal(db0, db0 + sizeof(magic), (char*)magic));
    assert(std::equal(db1, db1 + sizeof(magic), (char*)magic));
    
    std::free(q);
}

void* operator new  [](std::size_t size) throw(std::bad_alloc)
{
    return operator new(size);
}

void operator delete[](void* ptr) throw()
{
    return operator delete(ptr);
}

namespace eh_test {

template <class F1>
void eh_test( F1 op)
{
    bool done = false;
    int count = 0;
    
    while (!done)
    {
        try
        {
            enable_exceptions(count);
            op();
            done = true;
        }
        catch(...)
        {
            ++count;
        }
        disable_exceptions();
    }
}

void setup()
{
    // Once needed this for checking that array delete dispatched to
    // the non-array version.  We handle that ourselves now.
}

}
