namespace muda
{
//using T                  = float;
//constexpr auto M         = 3;
//constexpr auto N         = 3;
//constexpr auto DstLayout = FieldEntryLayout::RuntimeLayout;
//constexpr auto SrcLayout = FieldEntryLayout::RuntimeLayout;

template <typename T, FieldEntryLayout DstLayout, FieldEntryLayout SrcLayout, int M, int N>
MUDA_HOST FieldEntryLaunch& FieldEntryLaunch::copy(FieldEntryView<T, DstLayout, M, N> dst,
                                                   CFieldEntryView<T, SrcLayout, M, N> src)
{
    MUDA_ASSERT(dst.size() == src.size(),
                "FieldEntry size mismatching: dst.size() = %d, src.size() = %d",
                dst.size(),
                src.size());

    ParallelFor()  //
        .apply(dst.size(),
               [dst, src] __device__(int i) mutable
               {
                   if constexpr(M == 1 && N == 1)
                   {
                       *dst.data(i) = *src.data(i);
                   }
                   else if constexpr(M > 1 && N == 1)
                   {
#pragma unroll
                       for(int j = 0; j < M; ++j)
                       {
                           *dst.data(i, j) = *src.data(i, j);
                       }
                   }
                   else if constexpr(M > 1 && N > 1)
                   {
#pragma unroll
                       for(int j = 0; j < M; ++j)
                       {
#pragma unroll
                           for(int k = 0; k < N; ++k)
                           {
                               *dst.data(i, j, k) = *src.data(i, j, k);
                           }
                       }
                   }
                   else
                   {
                       static_assert("Invalid");
                   }
               });
    return *this;
}


template <typename T, FieldEntryLayout DstLayout, int M, int N>
MUDA_HOST FieldEntryLaunch& FieldEntryLaunch::fill(
    FieldEntryView<T, DstLayout, M, N>                              dst,
    const typename FieldEntryView<T, DstLayout, M, N>::ElementType& value)
{
    ParallelFor()  //
        .apply(dst.size(),
               [dst, value] __device__(int i) mutable
               {
                   if constexpr(M == 1 && N == 1)
                   {
                       *dst.data(i) = value;
                   }
                   else if constexpr(M > 1 && N == 1)
                   {

#pragma unroll
                       for(int j = 0; j < M; ++j)
                       {
                           *dst.data(i, j) = value(j);
                       }
                   }
                   else if constexpr(M > 1 && N > 1)
                   {
#pragma unroll
                       for(int j = 0; j < M; ++j)
                       {
#pragma unroll
                           for(int k = 0; k < N; ++k)
                           {
                               *dst.data(i, j, k) = value(j, k);
                           }
                       }
                   }
                   else
                   {
                       static_assert("Invalid");
                   }
               });

    return *this;
}

template <typename T, FieldEntryLayout SrcLayout, int M, int N>
MUDA_HOST FieldEntryLaunch& FieldEntryLaunch::copy(
    BufferView<typename CFieldEntryView<T, SrcLayout, M, N>::ElementType> dst,
    CFieldEntryView<T, SrcLayout, M, N>                                   src)
{
    MUDA_ASSERT(dst.size() == src.size(),
                "FieldEntry size mismatching: dst.size() = %d, src.size() = %d",
                dst.size(),
                src.size());


    ParallelFor()  //
        .apply(dst.size(),
               [dst, src] __device__(int i) mutable
               {
                   if constexpr(M == 1 && N == 1)
                   {
                       *dst.data(i) = *src.data(i);
                   }
                   else if constexpr(M > 1 && N == 1)
                   {
#pragma unroll
                       for(int j = 0; j < M; ++j)
                       {
                           (*dst.data(i))(j) = *src.data(i, j);
                       }
                   }
                   else if constexpr(M > 1 && N > 1)
                   {
#pragma unroll
                       for(int j = 0; j < M; ++j)
                       {
#pragma unroll
                           for(int k = 0; k < N; ++k)
                           {
                               (*dst.data(i))(j, k) = *src.data(i, j, k);
                           }
                       }
                   }
                   else
                   {
                       static_assert("Invalid");
                   }
               });

    return *this;
}

template <typename T, FieldEntryLayout DstLayout, int M, int N>
MUDA_HOST FieldEntryLaunch& FieldEntryLaunch::copy(
    FieldEntryView<T, DstLayout, M, N>                                    dst,
    CBufferView<typename FieldEntryView<T, DstLayout, M, N>::ElementType> src)
{
    MUDA_ASSERT(dst.size() == src.size(),
                "FieldEntry size mismatching: dst.size() = %d, src.size() = %d",
                dst.size(),
                src.size());

    ParallelFor().apply(dst.size(),
                        [dst = dst, src = src] __device__(int i) mutable
                        {
                            if constexpr(M == 1 && N == 1)
                            {
                                *dst.data(i) = (*src.data(i));
                            }
                            else if constexpr(M > 1 && N == 1)
                            {
#pragma unroll

                                for(int j = 0; j < M; ++j)
                                {
                                    *dst.data(i, j) = (*src.data(i))(j);
                                }
                            }
                            else if constexpr(M > 1 && N > 1)
                            {
#pragma unroll
                                for(int j = 0; j < M; ++j)
                                {
#pragma unroll
                                    for(int k = 0; k < N; ++k)
                                    {
                                        *dst.data(i, j, k) = (*src.data(i))(j, k);
                                    }
                                }
                            }
                            else
                            {
                                static_assert("Invalid");
                            }
                        });

    return *this;
}
}  // namespace muda