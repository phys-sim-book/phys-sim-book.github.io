#pragma once
#include <muda/muda_config.h>
#include <muda/tools/platform.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <cuda_runtime_api.h>

#define MUDA_HOST __host__
#define MUDA_DEVICE __device__
#define MUDA_GLOBAL __global__
#define MUDA_CONSTANT __constant__
#define MUDA_SHARED __shared__
#define MUDA_MANAGED __managed__

#ifdef __CUDA_ARCH__
#define MUDA_GENERIC MUDA_HOST MUDA_DEVICE
#else
#define MUDA_GENERIC
#endif


#ifdef MUDA_THREAD_ONLY_AS_GENERIC
#define MUDA_THREAD_ONLY MUDA_GENERIC
#else
#define MUDA_THREAD_ONLY MUDA_DEVICE
#endif

// Attributes
#define MUDA_NODISCARD [[nodiscard]]
#define MUDA_DEPRECATED [[deprecated]]
#define MUDA_FALLTHROUGH [[fallthrough]]
#define MUDA_MAYBE_UNUSED [[maybe_unused]]
#define MUDA_NORETURN [[noreturn]]

// Keywords
#define MUDA_NOEXCEPT noexcept
#define MUDA_INLINE inline
#define MUDA_CONSTEXPR constexpr