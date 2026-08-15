#ifndef HE_PLUGIN_LIBRARY_H
#define HE_PLUGIN_LIBRARY_H

class HECore;

namespace HEPluginLibrary {

// Composition root for built-in pure C++ plugins.
void registerBuiltinPlugins(HECore& core);

} // namespace HEPluginLibrary

#endif // HE_PLUGIN_LIBRARY_H
