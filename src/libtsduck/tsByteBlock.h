//----------------------------------------------------------------------------
//
// TSDuck - The MPEG Transport Stream Toolkit
// Copyright (c) 2005-2017, Thierry Lelegard
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
// THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------
//!
//!  @file
//!  Definitions of a generic block of bytes.
//!
//----------------------------------------------------------------------------

#pragma once
#include "tsPlatform.h"
#include "tsSafePtr.h"

namespace ts {

    class TSDUCKDLL ByteBlock : public std::vector<uint8_t>
    {
    public:
        typedef std::vector<uint8_t> ByteVector;

        // Default constructor
        explicit ByteBlock (size_type size = 0);

        // Constructor, initialized with size bytes of specified value
        ByteBlock (size_type size, uint8_t value);

        // Constructor from a data block
        ByteBlock (const void* data, size_type size);

        // Constructor from a C string
        ByteBlock (const char* str);

        // Return a pointer to the beginning of the raw data.
        // May be invalidated when the vector changes.
        const uint8_t* data() const {return size() == 0 ? 0 : &(*this)[0];}
        uint8_t* data() {return size() == 0 ? 0 : &(*this)[0];}

        // Replace the content of a byte block.
        void copy (const void* data, size_type size);

        // Remove 'size' elements at index 'first'.
        // (the STL equivalent uses iterators, not indices).
        void erase (size_type first, size_type size);

        // Increase size by n and return pointer to new n-byte area at end of block.
        void* enlarge (size_type n);

        // Append raw data to a byte block.
        void append (const void* data, size_type size) {if (size > 0 && data != 0) ::memcpy (enlarge (size), data, size);}

        // Append a byte block to a byte block.
        void append (const ByteBlock& bb) {append (bb.data(), bb.size());}

        // Append string to a byte block.
        void append (const std::string& s) {append (s.c_str(), s.length());}

        // Append n bytes with value i
        void append (uint8_t i, size_type size) {if (size > 0) ::memset (enlarge (size), i, size);}

        // Add a integer in big endian representation at the end.
        void appendUInt8  (uint8_t  i) {push_back(i);}
        void appendUInt16 (uint16_t i) {appendUInt16BE(i);}
        void appendUInt24 (uint32_t i) {appendUInt24BE(i);}
        void appendUInt32 (uint32_t i) {appendUInt32BE(i);}
        void appendUInt64 (uint64_t i) {appendUInt64BE(i);}

        void appendInt8  (int8_t  i) {push_back(uint8_t(i));}
        void appendInt16 (int16_t i) {appendInt16BE(i);}
        void appendInt24 (int32_t i) {appendInt24BE(i);}
        void appendInt32 (int32_t i) {appendInt32BE(i);}
        void appendInt64 (int64_t i) {appendInt64BE(i);}

        // Add a integer in big endian representation at the end.
        void appendUInt16BE (uint16_t i) {PutUInt16BE (enlarge (2), i);}
        void appendUInt24BE (uint32_t i) {PutUInt24BE (enlarge (3), i);}
        void appendUInt32BE (uint32_t i) {PutUInt32BE (enlarge (4), i);}
        void appendUInt64BE (uint64_t i) {PutUInt64BE (enlarge (8), i);}

        void appendInt16BE (int16_t i) {PutInt16BE (enlarge (2), i);}
        void appendInt24BE (int32_t i) {PutInt24BE (enlarge (3), i);}
        void appendInt32BE (int32_t i) {PutInt32BE (enlarge (4), i);}
        void appendInt64BE (int64_t i) {PutInt64BE (enlarge (8), i);}

        // Add a integer in little endian representation at the end.
        void appendUInt16LE (uint16_t i) {PutUInt16LE (enlarge (2), i);}
        void appendUInt24LE (uint32_t i) {PutUInt24LE (enlarge (3), i);}
        void appendUInt32LE (uint32_t i) {PutUInt32LE (enlarge (4), i);}
        void appendUInt64LE (uint64_t i) {PutUInt64LE (enlarge (8), i);}

        void appendInt16LE (int16_t i) {PutInt16LE (enlarge (2), i);}
        void appendInt24LE (int32_t i) {PutInt24LE (enlarge (3), i);}
        void appendInt32LE (int32_t i) {PutInt32LE (enlarge (4), i);}
        void appendInt64LE (int64_t i) {PutInt64LE (enlarge (8), i);}

        // Template variant
        template<typename INT> void append (INT i) {PutInt<INT> (enlarge (sizeof(INT)), i);}
    };

    // Template specializations for performance
    template<> inline void ByteBlock::append (uint8_t i) {push_back (i);}
    template<> inline void ByteBlock::append (int8_t i) {push_back (uint8_t (i));}

    // Safe pointer for ByteBlock

    // Not thread-safe
    typedef SafePtr <ByteBlock, NullMutex> ByteBlockPtr;

    // Thread-safe (MT = multi-thread):
    typedef SafePtr <ByteBlock, Mutex> ByteBlockPtrMT;
}