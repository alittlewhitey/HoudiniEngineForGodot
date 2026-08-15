#include "HEPluginLibrary.h"

#include <memory>

#include "HEAssetLifecyclePlugin.h"
#include "HECookDiagnosticsPlugin.h"
#include "HEParameterAuditPlugin.h"
#include "HECore.h"

namespace HEPluginLibrary {

void registerBuiltinPlugins(HECore& core){
    core.registerPlugin(std::make_unique<HECookDiagnosticsPlugin>());
    core.registerPlugin(std::make_unique<HEAssetLifecyclePlugin>());
    core.registerPlugin(std::make_unique<HEParameterAuditPlugin>());
}

} // namespace HEPluginLibrary
