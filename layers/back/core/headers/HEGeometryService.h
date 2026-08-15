#ifndef HE_GEOMETRY_SERVICE_H
#define HE_GEOMETRY_SERVICE_H

#include <cstdint>
#include <unordered_map>
#include <utility>
#include <vector>

// Neutral part types. The integer values intentionally match HAPI_PartType
// so frontend adapters can assign HAPI values directly.
enum class HEPartType {
    Invalid = -1,
    Mesh,
    Curve,
    Volume,
    Instancer,
    Box,
    Sphere
};

// Integer values intentionally match HAPI_AttributeOwner so adapters can
// assign HAPI values directly.
enum class HEAttribOwner {
    Invalid = -1,
    Vertex,
    Point,
    Prim,
    Detail
};

// Neutral mesh part cache produced by the HAPI adapter.
struct HEMeshPartData {
    std::vector<int> faceCounts;
    std::vector<float> positions;
    std::vector<int> vertexList;

    std::pair<HEAttribOwner, std::vector<float>> colors;
    std::pair<HEAttribOwner, std::vector<float>> normals;
    std::pair<HEAttribOwner, std::vector<float>> uvs;
    std::pair<HEAttribOwner, std::vector<float>> uv2s;
};

// Neutral transform used by instancer parts.
struct HETransform {
    float position[3] = {0.0f, 0.0f, 0.0f};
    float rotationQuaternion[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    float scale[3] = {1.0f, 1.0f, 1.0f};
    float shear[3] = {0.0f, 0.0f, 0.0f};
    int rstOrder = 0;
};

// Integer values intentionally match HAPI_CurveType so adapters can assign
// HAPI values directly.
enum class HECurveType {
    Invalid = -1,
    Linear,
    Nurbs,
    Bezier
};

struct HECurvePartData {
    HECurveType curveType = HECurveType::Invalid;
    int curveCount = 0;
    int vertexCount = 0;
    int knotCount = 0;
    bool isPeriodic = false;
    bool isRational = false;
    int order = 0;
    bool hasKnots = false;
    bool isClosed = false;

    std::vector<int> curveCounts;
    std::vector<float> controlPoints;
    std::vector<float> weights;
    std::vector<float> knots;
};

struct HEInstancerPartData {
    std::vector<int> instancedPartIds;
    std::vector<HETransform> transforms;
};

// Host-independent geometry registry.
class HEGeometryService {
public:
    void clear(){
        partType.clear();
        meshGeometries.clear();
        curveGeometries.clear();
        instancerParts.clear();
        imageBuffers.clear();
    }

    void clearNode(int nodeId){
        partType.erase(nodeId);
        meshGeometries.erase(nodeId);
        curveGeometries.erase(nodeId);
        instancerParts.erase(nodeId);
        imageBuffers.erase(nodeId);
    }

    std::unordered_map<int, std::unordered_map<int, HEPartType>> partType;
    std::unordered_map<int, std::unordered_map<int, HEMeshPartData>> meshGeometries;
    std::unordered_map<int, std::unordered_map<int, HECurvePartData>> curveGeometries;
    std::unordered_map<int, std::unordered_map<int, HEInstancerPartData>> instancerParts;

    // nodeId -> encoded image bytes (currently PNG for COP nodes).
    std::unordered_map<int, std::vector<unsigned char>> imageBuffers;
};

#endif // HE_GEOMETRY_SERVICE_H
