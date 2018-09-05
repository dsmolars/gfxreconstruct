/*
** Copyright (c) 2018 LunarG, Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

#ifdef ENABLE_LZ4_COMPRESSION

#include "util/lz4_compressor.h"
#include "lz4.h"

BRIMSTONE_BEGIN_NAMESPACE(brimstone)
BRIMSTONE_BEGIN_NAMESPACE(util)

size_t Lz4Compressor::Compress(const size_t          uncompressed_size,
                               const uint8_t*        uncompressed_data,
                               std::vector<uint8_t>* compressed_data)
{
    size_t copy_size = 0;
    if (kLz4 == compression_type_)
    {
        try
        {
            if (nullptr == compressed_data)
            {
                return 0;
            }
            LZ4_stream_t lz4_stream;
            LZ4_resetStream(&lz4_stream);
            size_t lz4_compressed_size = LZ4_COMPRESSBOUND(uncompressed_size);
            if (lz4_compressed_size > compressed_data->size())
            {
                compressed_data->resize(lz4_compressed_size);
            }
            const size_t compressed_size_generated =
                LZ4_compress_fast_continue(&lz4_stream,
                                           reinterpret_cast<const char*>(uncompressed_data),
                                           reinterpret_cast<char*>(compressed_data->data()),
                                           static_cast<const int32_t>(uncompressed_size),
                                           static_cast<int32_t>(lz4_compressed_size),
                                           1);
            if (compressed_size_generated > 0)
            {
                copy_size = compressed_size_generated;
            }
        }
        catch (...)
        {}
    }
    return copy_size;
}

size_t Lz4Compressor::Decompress(const size_t                compressed_size,
                                 const std::vector<uint8_t>& compressed_data,
                                 const size_t                expected_uncompressed_size,
                                 std::vector<uint8_t>*       uncompressed_data)
{
    size_t copy_size = 0;
    if (kLz4 == compression_type_)
    {
        try
        {
            if (nullptr == uncompressed_data)
            {
                return 0;
            }
            LZ4_streamDecode_t lz4_stream_decode;
            LZ4_setStreamDecode(&lz4_stream_decode, NULL, 0);
            const int uncompressed_size_generated =
                LZ4_decompress_safe_continue(&lz4_stream_decode,
                                             reinterpret_cast<const char*>(compressed_data.data()),
                                             reinterpret_cast<char*>(uncompressed_data->data()),
                                             static_cast<int32_t>(compressed_size),
                                             static_cast<int32_t>(expected_uncompressed_size));
            if (uncompressed_size_generated > 0)
            {
                copy_size = uncompressed_size_generated;
            }
        }
        catch (...)
        {}
    }
    return copy_size;
}

BRIMSTONE_END_NAMESPACE(util)
BRIMSTONE_END_NAMESPACE(brimstone)

#endif // ENABLE_LZ4_COMPRESSION