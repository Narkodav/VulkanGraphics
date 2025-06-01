#pragma once
#include <optional>
#include <functional>
#include <any>
#include <map>
#include <string>

enum class FeatureType : size_t {
    GEOMETRY_SHADER = 0,                        //geometry shader
    TESSELLATION_SHADER,                        //tessellation shader

    SAMPLER_ANISOTROPY,                         //sample anisatropy
    FILL_MODE_NON_SOLID,
    TEXTURE_COMPRESSION_BC,
    TEXTURE_COMPRESSION_ETC2,

    SHADER_INT64,
    SHADER_FLOAT64,
    SHADER_INT16,

    MULTI_DRAW_INDIRECT,
    WIDE_LINES,
    LARGE_POINTS,
    MULTI_VIEWPORT,

    DEPTH_CLAMP,
    DEPTH_BIAS_CLAMP,
    DUAL_SRC_BLEND,

    VERTEX_PIPELINE_STORES_AND_ATOMICS,
    FRAGMENT_STORES_AND_ATOMICS,

    FEATURES_NUM
};

class FeatureCaching
{
public:

    using Features = vk::PhysicalDeviceFeatures2;

    struct FeatureBundle
    {
        std::function<vk::Bool32(const Features&)> existsCheck; //no values, so only checks
        std::string name;
        int score;
    };

    static inline const std::unordered_map<const FeatureType, const FeatureBundle> featureBundleTable{
    {FeatureType::GEOMETRY_SHADER, {
        [](const Features& features) { return features.features.geometryShader; },
        "Geometry Shader",
        100
    }},
    {FeatureType::TESSELLATION_SHADER, {
        [](const Features& features) { return features.features.tessellationShader; },
        "Tessellation Shader",
        100
    }},
    {FeatureType::SAMPLER_ANISOTROPY, {
        [](const Features& features) { return features.features.samplerAnisotropy; },
        "Sampler Anisotropy",
        100
    }},
    {FeatureType::FILL_MODE_NON_SOLID, {
        [](const Features& features) { return features.features.fillModeNonSolid; },
        "Fill Mode Non Solid",
        100
    }},
    {FeatureType::TEXTURE_COMPRESSION_BC, {
        [](const Features& features) { return features.features.textureCompressionBC; },
        "BC Texture Compression",
        50
    }},
    {FeatureType::TEXTURE_COMPRESSION_ETC2, {
        [](const Features& features) { return features.features.textureCompressionETC2; },
        "ETC2 Texture Compression",
        50
    }},
    {FeatureType::SHADER_INT64, {
        [](const Features& features) { return features.features.shaderInt64; },
        "64-bit Integer Shader Operations",
        25
    }},
    {FeatureType::SHADER_FLOAT64, {
        [](const Features& features) { return features.features.shaderFloat64; },
        "64-bit Float Shader Operations",
        25
    }},
    {FeatureType::SHADER_INT16, {
        [](const Features& features) { return features.features.shaderInt16; },
        "16-bit Integer Shader Operations",
        25
    }},
    {FeatureType::MULTI_DRAW_INDIRECT, {
        [](const Features& features) { return features.features.multiDrawIndirect; },
        "Multi Draw Indirect",
        50
    }},
    {FeatureType::WIDE_LINES, {
        [](const Features& features) { return features.features.wideLines; },
        "Wide Lines",
        25
    }},
    {FeatureType::LARGE_POINTS, {
        [](const Features& features) { return features.features.largePoints; },
        "Large Points",
        25
    }},
    {FeatureType::MULTI_VIEWPORT, {
        [](const Features& features) { return features.features.multiViewport; },
        "Multi Viewport",
        100
    }},
    {FeatureType::DEPTH_CLAMP, {
        [](const Features& features) { return features.features.depthClamp; },
        "Depth Clamp",
        75
    }},
    {FeatureType::DEPTH_BIAS_CLAMP, {
        [](const Features& features) { return features.features.depthBiasClamp; },
        "Depth Bias Clamp",
        75
    }},
    {FeatureType::DUAL_SRC_BLEND, {
        [](const Features& features) { return features.features.dualSrcBlend; },
        "Dual Source Blend",
        50
    }},
    {FeatureType::VERTEX_PIPELINE_STORES_AND_ATOMICS, {
        [](const Features& features) { return features.features.vertexPipelineStoresAndAtomics; },
        "Vertex Pipeline Stores and Atomics",
        50
    }},
    {FeatureType::FRAGMENT_STORES_AND_ATOMICS, {
        [](const Features& features) { return features.features.fragmentStoresAndAtomics; },
        "Fragment Stores and Atomics",
        50
    }}
    };

    static std::map<FeatureType, bool> getFeatures(const Features& features)
    {

        std::map<FeatureType, bool> featureMap;

        for (int i = 0; i < static_cast<int>(FeatureType::FEATURES_NUM); i++)
            featureMap[static_cast<FeatureType>(i)] =
            featureBundleTable.at(static_cast<FeatureType>(i)).existsCheck(features);

        return featureMap;
    }

    static bool compare(Rating& rating,
        const std::vector<FeatureType>& required, 
        const std::map<FeatureType, bool>& available)
    {
        

        for (auto& feature : required)
        {
            auto it = available.find(feature);
            if (it != available.end() && it->second)
            {
                rating.suitable = false;
                rating.reasons.push_back(
                    "Feature " + featureBundleTable.at(it->first).name + " not supported");
            }
        }
        if (!rating.suitable)
            return false;

        for (auto& feature : available)
            rating.score += featureBundleTable.at(feature.first).score;

        return true;
    }

};

