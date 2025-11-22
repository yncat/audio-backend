#include "plugin_inspector.h"
#include "context.h"
#include "fmod/fmod_errors.h"
#include <fstream>
#include <sstream>

extern AudioBackendContext* g_context;

int corePluginInspect(const char* plugin_path, const char* output_path) {
    // Check if backend is initialized
    if (!isBackendInitialized()) {
        g_context->SetLastError("Backend not initialized");
        return -1;
    }

    if (plugin_path == nullptr || output_path == nullptr) {
        g_context->SetLastError("Invalid parameters: plugin_path or output_path is null");
        return -1;
    }

    FMOD::System* system = g_context->GetFmodSystem();
    unsigned int plugin_handle = 0;
    FMOD_RESULT result;

    // Load the plugin
    result = system->loadPlugin(plugin_path, &plugin_handle);
    if (result != FMOD_OK) {
        std::ostringstream oss;
        oss << "Failed to load plugin: " << FMOD_ErrorString(result);
        g_context->SetLastError(oss.str());
        return -1;
    }

    // Open output file
    std::ofstream outfile(output_path);
    if (!outfile.is_open()) {
        system->unloadPlugin(plugin_handle);
        g_context->SetLastError("Failed to open output file");
        return -1;
    }

    // Get number of nested plugins
    int num_nested = 0;
    result = system->getNumNestedPlugins(plugin_handle, &num_nested);
    if (result != FMOD_OK) {
        outfile.close();
        system->unloadPlugin(plugin_handle);
        std::ostringstream oss;
        oss << "Failed to get number of nested plugins: " << FMOD_ErrorString(result);
        g_context->SetLastError(oss.str());
        return -1;
    }

    outfile << "Plugin: " << plugin_path << "\n";
    outfile << "Number of nested plugins: " << num_nested << "\n\n";

    // Iterate through each nested plugin
    for (int i = 0; i < num_nested; i++) {
        unsigned int nested_handle = 0;
        result = system->getNestedPlugin(plugin_handle, i, &nested_handle);
        if (result != FMOD_OK) {
            outfile << "  [" << i << "] Error getting nested plugin: " << FMOD_ErrorString(result) << "\n";
            continue;
        }

        // Get plugin info
        FMOD_PLUGINTYPE plugin_type;
        char name[256];
        unsigned int version;
        result = system->getPluginInfo(nested_handle, &plugin_type, name, sizeof(name), &version);
        if (result != FMOD_OK) {
            outfile << "  [" << i << "] Error getting plugin info: " << FMOD_ErrorString(result) << "\n";
            continue;
        }

        outfile << "  [" << i << "] Plugin Name: " << name << "\n";
        outfile << "       Type: ";
        switch (plugin_type) {
            case FMOD_PLUGINTYPE_OUTPUT:
                outfile << "Output\n";
                break;
            case FMOD_PLUGINTYPE_CODEC:
                outfile << "Codec\n";
                break;
            case FMOD_PLUGINTYPE_DSP:
                outfile << "DSP\n";
                break;
            default:
                outfile << "Unknown\n";
                break;
        }
        outfile << "       Version: 0x" << std::hex << version << std::dec << "\n";

        // If it's a DSP plugin, try to get parameter information
        if (plugin_type == FMOD_PLUGINTYPE_DSP) {
            // Create a DSP instance to inspect parameters
            FMOD::DSP* dsp = nullptr;
            result = system->createDSPByPlugin(nested_handle, &dsp);
            if (result == FMOD_OK && dsp != nullptr) {
                int num_params = 0;
                result = dsp->getNumParameters(&num_params);
                if (result == FMOD_OK) {
                    outfile << "       Parameters: " << num_params << "\n";

                    for (int p = 0; p < num_params; p++) {
                        FMOD_DSP_PARAMETER_DESC* param_desc = nullptr;
                        result = dsp->getParameterInfo(p, &param_desc);
                        if (result == FMOD_OK && param_desc != nullptr) {
                            outfile << "         [" << p << "] " << param_desc->name;
                            outfile << " (";
                            switch (param_desc->type) {
                                case FMOD_DSP_PARAMETER_TYPE_FLOAT:
                                    outfile << "Float, range: " << param_desc->floatdesc.min
                                           << " to " << param_desc->floatdesc.max;
                                    break;
                                case FMOD_DSP_PARAMETER_TYPE_INT:
                                    outfile << "Int, range: " << param_desc->intdesc.min
                                           << " to " << param_desc->intdesc.max;
                                    break;
                                case FMOD_DSP_PARAMETER_TYPE_BOOL:
                                    outfile << "Bool";
                                    break;
                                case FMOD_DSP_PARAMETER_TYPE_DATA:
                                    outfile << "Data";
                                    break;
                                default:
                                    outfile << "Unknown type";
                                    break;
                            }
                            outfile << ")\n";

                            if (param_desc->label[0] != '\0') {
                                outfile << "             Label: " << param_desc->label << "\n";
                            }
                        }
                    }
                }
                dsp->release();
            }
        }

        outfile << "\n";
    }

    outfile.close();

    // Unload the plugin
    result = system->unloadPlugin(plugin_handle);
    if (result != FMOD_OK) {
        std::ostringstream oss;
        oss << "Failed to unload plugin: " << FMOD_ErrorString(result);
        g_context->SetLastError(oss.str());
        return -1;
    }

    return 0;
}
