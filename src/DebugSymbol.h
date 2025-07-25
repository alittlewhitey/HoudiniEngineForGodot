#ifndef HOUDINI_ENGINE_DEBUG_SYMBOL
#define HOUDINI_ENGINE_DEBUG_SYMBOL

// #define FORCE_DEBUG
// #define FORCE_RELEASE

#if FORCE_DEBUG && FORCE_RELEASE
    #error "Force debug and force release can't coexist"
#endif
#if defined(FORCE_DEBUG)
#define _DEBUG
#undef NDEBUG
#endif
#if defined(FORCE_RELEASE)
#define NDEBUG
#undef _DEBUG
#endif
#if defined(_DEBUG) || !defined(NDEBUG)
#define HE_DEBUG_MODE
#undef HE_RELEASE_MODE
#else
#define HE_RELEASE_MODE
#undef HE_DEBUG_MODE
#endif
#include <assert.h>
#ifndef HOT_RELOAD_ENABLED
#define HOT_RELOAD_ENABLED
#endif

#endif