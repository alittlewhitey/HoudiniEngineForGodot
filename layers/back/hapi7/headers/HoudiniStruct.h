#ifndef HOUDINI_STRUCT_H
#define HOUDINI_STRUCT_H
#include <stdint.h>

#define HAPI_POSITION_VECTOR_SIZE           3
#define HAPI_SCALE_VECTOR_SIZE              3
#define HAPI_SHEAR_VECTOR_SIZE              3
#define HAPI_NORMAL_VECTOR_SIZE             3
#define HAPI_QUATERNION_VECTOR_SIZE         4
#define HAPI_EULER_VECTOR_SIZE              3
#define HAPI_UV_VECTOR_SIZE                 2
#define HAPI_COLOR_VECTOR_SIZE              4
#define HAPI_CV_VECTOR_SIZE                 4
#define HAPI_PRIM_MIN_VERTEX_COUNT          1
#define HAPI_PRIM_MAX_VERTEX_COUNT          16
#define HAPI_INVALID_PARM_ID                -1
#define HAPI_MAX_NUM_CONNECTIONS            128
#define HAPI_ATTRIB_POSITION                "P"
#define HAPI_ATTRIB_UV                      "uv"
#define HAPI_ATTRIB_UV2                     "uv2"
#define HAPI_ATTRIB_NORMAL                  "N"
#define HAPI_ATTRIB_TANGENT                 "tangentu"
#define HAPI_ATTRIB_TANGENT2                "tangentv"
#define HAPI_ATTRIB_COLOR                   "Cd"
#define HAPI_ATTRIB_NAME                    "name"
#define HAPI_ATTRIB_INSTANCE                "instance"
#define HAPI_ATTRIB_ROT                     "rot"
#define HAPI_ATTRIB_SCALE                   "scale"
#define HAPI_UNGROUPED_GROUP_NAME           "__ungrouped_group"
#define HAPI_RAW_FORMAT_NAME                "HAPI_RAW"
#define HAPI_PNG_FORMAT_NAME                "PNG"
#define HAPI_JPEG_FORMAT_NAME               "JPEG"
#define HAPI_BMP_FORMAT_NAME                "Bitmap"
#define HAPI_TIFF_FORMAT_NAME               "TIFF"
#define HAPI_TGA_FORMAT_NAME                "Targa"
#define HAPI_DEFAULT_IMAGE_FORMAT_NAME      HAPI_PNG_FORMAT_NAME
#define HAPI_GLOBAL_NODES_NODE_NAME         "GlobalNodes"
#define HAPI_ENV_HIP                        "HIP"
#define HAPI_ENV_JOB                        "JOB"
#define HAPI_ENV_CLIENT_NAME                "HAPI_CLIENT_NAME"
#define HAPI_CACHE_COP2_COOK                 "COP Cook Cache"
#define HAPI_CACHE_COP2_FLIPBOOK             "COP Flipbook Cache"
#define HAPI_CACHE_IMAGE                    "Image Cache"
#define HAPI_CACHE_OBJ                      "Object Transform Cache"
#define HAPI_CACHE_GL_TEXTURE               "OpenGL Texture Cache"
#define HAPI_CACHE_GL_VERTEX                "OpenGL Vertex Cache"
#define HAPI_CACHE_SOP                      "SOP Cache"
#define HAPI_CACHE_VEX                      "VEX File Cache"
#define HAPI_ATTRIB_INPUT_CURVE_COORDS                "hapi_input_curve_coords"
#if ( defined _WIN32 || defined WIN32 )
    typedef unsigned int HAPI_ProcessId;
#else
    typedef int HAPI_ProcessId;
#endif
typedef bool HAPI_Bool;
typedef int8_t HAPI_Int8;
typedef uint8_t HAPI_UInt8;
typedef int16_t HAPI_Int16;
typedef uint16_t HAPI_UInt16;
typedef int64_t HAPI_Int64;
typedef uint64_t HAPI_UInt64;
typedef int64_t HAPI_SessionId;
typedef int HAPI_StringHandle;
typedef int HAPI_AssetLibraryId;
typedef int HAPI_NodeId;
typedef int HAPI_ParmId;
typedef int HAPI_PartId;
typedef int HAPI_PDG_WorkItemId;
typedef HAPI_PDG_WorkItemId HAPI_PDG_WorkitemId;
typedef int HAPI_PDG_GraphContextId;
typedef int HAPI_HIPFileId;
typedef int HAPI_ErrorCodeBits;
typedef int HAPI_NodeTypeBits;
typedef int HAPI_NodeFlagsBits;
// typedef HAPI_PDG_WorkItemState HAPI_PDG_WorkitemState;
// typedef HAPI_PDG_WorkItemInfo HAPI_PDG_WorkitemInfo;
// typedef HAPI_PDG_WorkItemOutputFile HAPI_PDG_WorkitemResultInfo;
enum HAPI_License
{
    HAPI_LICENSE_NONE,
    HAPI_LICENSE_HOUDINI_ENGINE,
    HAPI_LICENSE_HOUDINI,
    HAPI_LICENSE_HOUDINI_FX,
    HAPI_LICENSE_HOUDINI_ENGINE_INDIE,
    HAPI_LICENSE_HOUDINI_INDIE,
    HAPI_LICENSE_HOUDINI_ENGINE_UNITY_UNREAL,
    HAPI_LICENSE_HOUDINI_EDUCATION,
    HAPI_LICENSE_HOUDINI_ENGINE_EDUCATION,
    HAPI_LICENSE_MAX
};

enum HAPI_StatusType
{
    HAPI_STATUS_CALL_RESULT,
    HAPI_STATUS_COOK_RESULT,
    HAPI_STATUS_COOK_STATE,
    HAPI_STATUS_MAX
};

enum HAPI_StatusVerbosity
{
    HAPI_STATUSVERBOSITY_0,
    HAPI_STATUSVERBOSITY_1,
    HAPI_STATUSVERBOSITY_2,
    HAPI_STATUSVERBOSITY_ALL = HAPI_STATUSVERBOSITY_2,
    HAPI_STATUSVERBOSITY_ERRORS = HAPI_STATUSVERBOSITY_0,
    HAPI_STATUSVERBOSITY_WARNINGS = HAPI_STATUSVERBOSITY_1,
    HAPI_STATUSVERBOSITY_MESSAGES = HAPI_STATUSVERBOSITY_2,
};

enum HAPI_JobStatus
{
    HAPI_JOB_STATUS_RUNNING,
    HAPI_JOB_STATUS_IDLE,
    HAPI_JOB_STATUS_MAX
};

enum HAPI_Result
{
    HAPI_RESULT_SUCCESS                                 = 0,
    HAPI_RESULT_FAILURE                                 = 1,
    HAPI_RESULT_ALREADY_INITIALIZED                     = 2,
    HAPI_RESULT_NOT_INITIALIZED                         = 3,
    HAPI_RESULT_CANT_LOADFILE                           = 4,
    HAPI_RESULT_PARM_SET_FAILED                         = 5,
    HAPI_RESULT_INVALID_ARGUMENT                        = 6,
    HAPI_RESULT_CANT_LOAD_GEO                           = 7,
    HAPI_RESULT_CANT_GENERATE_PRESET                    = 8,
    HAPI_RESULT_CANT_LOAD_PRESET                        = 9,
    HAPI_RESULT_ASSET_DEF_ALREADY_LOADED                = 10,
    HAPI_RESULT_NO_LICENSE_FOUND                        = 110,
    HAPI_RESULT_DISALLOWED_NC_LICENSE_FOUND             = 120,
    HAPI_RESULT_DISALLOWED_NC_ASSET_WITH_C_LICENSE      = 130,
    HAPI_RESULT_DISALLOWED_NC_ASSET_WITH_LC_LICENSE     = 140,
    HAPI_RESULT_DISALLOWED_LC_ASSET_WITH_C_LICENSE      = 150,
    HAPI_RESULT_DISALLOWED_HENGINEINDIE_W_3PARTY_PLUGIN = 160,
    HAPI_RESULT_ASSET_INVALID                           = 200,
    HAPI_RESULT_NODE_INVALID                            = 210,
    HAPI_RESULT_USER_INTERRUPTED                        = 300,
    HAPI_RESULT_INVALID_SESSION                         = 400
};

enum HAPI_ErrorCode
{
    HAPI_ERRORCODE_ASSET_DEF_NOT_FOUND                  = 1 << 0,
    HAPI_ERRORCODE_PYTHON_NODE_ERROR                    = 1 << 1
};

enum HAPI_SessionType
{
    HAPI_SESSION_INPROCESS,
    HAPI_SESSION_THRIFT,
    HAPI_SESSION_CUSTOM1,
    HAPI_SESSION_CUSTOM2,
    HAPI_SESSION_CUSTOM3,
    HAPI_SESSION_MAX
};

enum HAPI_State
{
    HAPI_STATE_READY,
    HAPI_STATE_READY_WITH_FATAL_ERRORS,
    HAPI_STATE_READY_WITH_COOK_ERRORS,
    HAPI_STATE_STARTING_COOK,
    HAPI_STATE_COOKING,
    HAPI_STATE_STARTING_LOAD,
    HAPI_STATE_LOADING,
    HAPI_STATE_MAX,
    HAPI_STATE_MAX_READY_STATE = HAPI_STATE_READY_WITH_COOK_ERRORS
};

enum HAPI_PackedPrimInstancingMode
{
    HAPI_PACKEDPRIM_INSTANCING_MODE_INVALID = -1,
    HAPI_PACKEDPRIM_INSTANCING_MODE_DISABLED,
    HAPI_PACKEDPRIM_INSTANCING_MODE_HIERARCHY,
    HAPI_PACKEDPRIM_INSTANCING_MODE_FLAT,
    HAPI_PACKEDPRIM_INSTANCING_MODE_MAX
};

enum HAPI_Permissions
{
    HAPI_PERMISSIONS_NON_APPLICABLE,
    HAPI_PERMISSIONS_READ_WRITE,
    HAPI_PERMISSIONS_READ_ONLY,
    HAPI_PERMISSIONS_WRITE_ONLY,
    HAPI_PERMISSIONS_MAX
};

enum HAPI_RampType
{
    HAPI_RAMPTYPE_INVALID = -1,
    HAPI_RAMPTYPE_FLOAT,
    HAPI_RAMPTYPE_COLOR,
    HAPI_RAMPTYPE_MAX,
};

enum HAPI_ParmType
{
    HAPI_PARMTYPE_INT = 0,
    HAPI_PARMTYPE_MULTIPARMLIST,
    HAPI_PARMTYPE_TOGGLE,
    HAPI_PARMTYPE_BUTTON,
    HAPI_PARMTYPE_FLOAT,
    HAPI_PARMTYPE_COLOR,
    HAPI_PARMTYPE_STRING,
    HAPI_PARMTYPE_PATH_FILE,
    HAPI_PARMTYPE_PATH_FILE_GEO,
    HAPI_PARMTYPE_PATH_FILE_IMAGE,
    HAPI_PARMTYPE_NODE,
    HAPI_PARMTYPE_FOLDERLIST,
    HAPI_PARMTYPE_FOLDERLIST_RADIO,
    HAPI_PARMTYPE_FOLDER,
    HAPI_PARMTYPE_LABEL,
    HAPI_PARMTYPE_SEPARATOR,
    HAPI_PARMTYPE_PATH_FILE_DIR,
    HAPI_PARMTYPE_MAX,
    HAPI_PARMTYPE_INT_START         = HAPI_PARMTYPE_INT,
    HAPI_PARMTYPE_INT_END           = HAPI_PARMTYPE_BUTTON,
    HAPI_PARMTYPE_FLOAT_START       = HAPI_PARMTYPE_FLOAT,
    HAPI_PARMTYPE_FLOAT_END         = HAPI_PARMTYPE_COLOR,
    HAPI_PARMTYPE_STRING_START      = HAPI_PARMTYPE_STRING,
    HAPI_PARMTYPE_STRING_END        = HAPI_PARMTYPE_NODE,
    HAPI_PARMTYPE_PATH_START        = HAPI_PARMTYPE_PATH_FILE,
    HAPI_PARMTYPE_PATH_END          = HAPI_PARMTYPE_PATH_FILE_IMAGE,
    HAPI_PARMTYPE_NODE_START        = HAPI_PARMTYPE_NODE,
    HAPI_PARMTYPE_NODE_END          = HAPI_PARMTYPE_NODE,
    HAPI_PARMTYPE_CONTAINER_START   = HAPI_PARMTYPE_FOLDERLIST,
    HAPI_PARMTYPE_CONTAINER_END     = HAPI_PARMTYPE_FOLDERLIST_RADIO,
    HAPI_PARMTYPE_NONVALUE_START    = HAPI_PARMTYPE_FOLDER,
    HAPI_PARMTYPE_NONVALUE_END      = HAPI_PARMTYPE_SEPARATOR
};

enum HAPI_PrmScriptType
{
    HAPI_PRM_SCRIPT_TYPE_INT = 0,
    HAPI_PRM_SCRIPT_TYPE_FLOAT,
    HAPI_PRM_SCRIPT_TYPE_ANGLE,
    HAPI_PRM_SCRIPT_TYPE_STRING,
    HAPI_PRM_SCRIPT_TYPE_FILE,
    HAPI_PRM_SCRIPT_TYPE_DIRECTORY,
    HAPI_PRM_SCRIPT_TYPE_IMAGE,
    HAPI_PRM_SCRIPT_TYPE_GEOMETRY,
    HAPI_PRM_SCRIPT_TYPE_TOGGLE,
    HAPI_PRM_SCRIPT_TYPE_BUTTON,
    HAPI_PRM_SCRIPT_TYPE_VECTOR2,
    HAPI_PRM_SCRIPT_TYPE_VECTOR3,
    HAPI_PRM_SCRIPT_TYPE_VECTOR4,
    HAPI_PRM_SCRIPT_TYPE_INTVECTOR2,
    HAPI_PRM_SCRIPT_TYPE_INTVECTOR3,
    HAPI_PRM_SCRIPT_TYPE_INTVECTOR4,
    HAPI_PRM_SCRIPT_TYPE_UV,
    HAPI_PRM_SCRIPT_TYPE_UVW,
    HAPI_PRM_SCRIPT_TYPE_DIR,
    HAPI_PRM_SCRIPT_TYPE_COLOR,
    HAPI_PRM_SCRIPT_TYPE_COLOR4,
    HAPI_PRM_SCRIPT_TYPE_HUECIRCLE,
    HAPI_PRM_SCRIPT_TYPE_OPPATH,
    HAPI_PRM_SCRIPT_TYPE_OPLIST,
    HAPI_PRM_SCRIPT_TYPE_OBJECT,
    HAPI_PRM_SCRIPT_TYPE_OBJECTLIST,
    HAPI_PRM_SCRIPT_TYPE_RENDER,
    HAPI_PRM_SCRIPT_TYPE_SEPARATOR,
    HAPI_PRM_SCRIPT_TYPE_GEOMETRY_DATA,
    HAPI_PRM_SCRIPT_TYPE_KEY_VALUE_DICT,
    HAPI_PRM_SCRIPT_TYPE_LABEL,
    HAPI_PRM_SCRIPT_TYPE_RGBAMASK,
    HAPI_PRM_SCRIPT_TYPE_ORDINAL,
    HAPI_PRM_SCRIPT_TYPE_RAMP_FLT,
    HAPI_PRM_SCRIPT_TYPE_RAMP_RGB,
    HAPI_PRM_SCRIPT_TYPE_FLOAT_LOG,
    HAPI_PRM_SCRIPT_TYPE_INT_LOG,
    HAPI_PRM_SCRIPT_TYPE_DATA,
    HAPI_PRM_SCRIPT_TYPE_FLOAT_MINMAX,
    HAPI_PRM_SCRIPT_TYPE_INT_MINMAX,
    HAPI_PRM_SCRIPT_TYPE_INT_STARTEND,
    HAPI_PRM_SCRIPT_TYPE_BUTTONSTRIP,
    HAPI_PRM_SCRIPT_TYPE_ICONSTRIP,
    HAPI_PRM_SCRIPT_TYPE_GROUPRADIO = 1000,
    HAPI_PRM_SCRIPT_TYPE_GROUPCOLLAPSIBLE,
    HAPI_PRM_SCRIPT_TYPE_GROUPSIMPLE,
    HAPI_PRM_SCRIPT_TYPE_GROUP
};

enum HAPI_ChoiceListType
{
    HAPI_CHOICELISTTYPE_NONE,
    HAPI_CHOICELISTTYPE_NORMAL,
    HAPI_CHOICELISTTYPE_MINI,
    HAPI_CHOICELISTTYPE_REPLACE,
    HAPI_CHOICELISTTYPE_TOGGLE
};

enum HAPI_PresetType
{
    HAPI_PRESETTYPE_INVALID = -1,
    HAPI_PRESETTYPE_BINARY = 0,
    HAPI_PRESETTYPE_IDX,
    HAPI_PRESETTYPE_MAX
};

enum HAPI_NodeType
{
    HAPI_NODETYPE_ANY       = -1,
    HAPI_NODETYPE_NONE      = 0,
    HAPI_NODETYPE_OBJ       = 1 << 0,
    HAPI_NODETYPE_SOP       = 1 << 1,
    HAPI_NODETYPE_CHOP      = 1 << 2,
    HAPI_NODETYPE_ROP       = 1 << 3,
    HAPI_NODETYPE_SHOP      = 1 << 4,
    HAPI_NODETYPE_COP       = 1 << 5,
    HAPI_NODETYPE_VOP       = 1 << 6,
    HAPI_NODETYPE_DOP       = 1 << 7,
    HAPI_NODETYPE_TOP       = 1 << 8
};

enum HAPI_NodeFlags
{
    HAPI_NODEFLAGS_ANY          = -1,
    HAPI_NODEFLAGS_NONE         = 0,
    HAPI_NODEFLAGS_DISPLAY      = 1 << 0,
    HAPI_NODEFLAGS_RENDER       = 1 << 1,
    HAPI_NODEFLAGS_TEMPLATED    = 1 << 2,
    HAPI_NODEFLAGS_LOCKED       = 1 << 3,
    HAPI_NODEFLAGS_EDITABLE     = 1 << 4,
    HAPI_NODEFLAGS_BYPASS       = 1 << 5,
    HAPI_NODEFLAGS_NETWORK      = 1 << 6,
    HAPI_NODEFLAGS_OBJ_GEOMETRY = 1 << 7,
    HAPI_NODEFLAGS_OBJ_CAMERA   = 1 << 8,
    HAPI_NODEFLAGS_OBJ_LIGHT    = 1 << 9,
    HAPI_NODEFLAGS_OBJ_SUBNET   = 1 << 10,
    HAPI_NODEFLAGS_SOP_CURVE    = 1 << 11,
    HAPI_NODEFLAGS_SOP_GUIDE    = 1 << 12,
    HAPI_NODEFLAGS_TOP_NONSCHEDULER = 1 << 13,
    HAPI_NODEFLAGS_NON_BYPASS   = 1 << 14 
};

enum HAPI_GroupType
{
    HAPI_GROUPTYPE_INVALID = -1,
    HAPI_GROUPTYPE_POINT,
    HAPI_GROUPTYPE_PRIM,
    HAPI_GROUPTYPE_EDGE,
    HAPI_GROUPTYPE_MAX
};

enum HAPI_AttributeOwner
{
    HAPI_ATTROWNER_INVALID = -1,
    HAPI_ATTROWNER_VERTEX,
    HAPI_ATTROWNER_POINT,
    HAPI_ATTROWNER_PRIM,
    HAPI_ATTROWNER_DETAIL,
    HAPI_ATTROWNER_MAX
};

enum HAPI_CurveType
{
    HAPI_CURVETYPE_INVALID = -1,
    HAPI_CURVETYPE_LINEAR,
    HAPI_CURVETYPE_NURBS,
    HAPI_CURVETYPE_BEZIER,
    HAPI_CURVETYPE_MAX
};

enum HAPI_InputCurveMethod
{
    HAPI_CURVEMETHOD_INVALID = -1,
    HAPI_CURVEMETHOD_CVS,
    HAPI_CURVEMETHOD_BREAKPOINTS,
    HAPI_CURVEMETHOD_MAX
};

enum HAPI_InputCurveParameterization
{
    HAPI_CURVEPARAMETERIZATION_INVALID = -1,
    HAPI_CURVEPARAMETERIZATION_UNIFORM,
    HAPI_CURVEPARAMETERIZATION_CHORD,
    HAPI_CURVEPARAMETERIZATION_CENTRIPETAL,
    HAPI_CURVEPARAMETERIZATION_MAX
};

enum HAPI_VolumeType
{
    HAPI_VOLUMETYPE_INVALID = -1,
    HAPI_VOLUMETYPE_HOUDINI,
    HAPI_VOLUMETYPE_VDB,
    HAPI_VOLUMETYPE_MAX
};

enum HAPI_VolumeVisualType
{
    HAPI_VOLUMEVISTYPE_INVALID = -1,
    HAPI_VOLUMEVISTYPE_SMOKE,
    HAPI_VOLUMEVISTYPE_RAINBOW,
    HAPI_VOLUMEVISTYPE_ISO,
    HAPI_VOLUMEVISTYPE_INVISIBLE,
    HAPI_VOLUMEVISTYPE_HEIGHTFIELD,
    HAPI_VOLUMEVISTYPE_MAX
};

enum HAPI_StorageType
{
    HAPI_STORAGETYPE_INVALID = -1,
    HAPI_STORAGETYPE_INT,
    HAPI_STORAGETYPE_INT64,
    HAPI_STORAGETYPE_FLOAT,
    HAPI_STORAGETYPE_FLOAT64,
    HAPI_STORAGETYPE_STRING,
    HAPI_STORAGETYPE_UINT8,
    HAPI_STORAGETYPE_INT8,
    HAPI_STORAGETYPE_INT16,
    HAPI_STORAGETYPE_DICTIONARY,
    HAPI_STORAGETYPE_INT_ARRAY,
    HAPI_STORAGETYPE_INT64_ARRAY,
    HAPI_STORAGETYPE_FLOAT_ARRAY,
    HAPI_STORAGETYPE_FLOAT64_ARRAY,
    HAPI_STORAGETYPE_STRING_ARRAY,
    HAPI_STORAGETYPE_UINT8_ARRAY,
    HAPI_STORAGETYPE_INT8_ARRAY,
    HAPI_STORAGETYPE_INT16_ARRAY,
    HAPI_STORAGETYPE_DICTIONARY_ARRAY,
    HAPI_STORAGETYPE_MAX
};

enum HAPI_AttributeTypeInfo
{
    HAPI_ATTRIBUTE_TYPE_INVALID = -1,
    HAPI_ATTRIBUTE_TYPE_NONE,
    HAPI_ATTRIBUTE_TYPE_POINT,
    HAPI_ATTRIBUTE_TYPE_HPOINT,
    HAPI_ATTRIBUTE_TYPE_VECTOR,
    HAPI_ATTRIBUTE_TYPE_NORMAL,
    HAPI_ATTRIBUTE_TYPE_COLOR,
    HAPI_ATTRIBUTE_TYPE_QUATERNION,
    HAPI_ATTRIBUTE_TYPE_MATRIX3,
    HAPI_ATTRIBUTE_TYPE_MATRIX,
    HAPI_ATTRIBUTE_TYPE_ST,
    HAPI_ATTRIBUTE_TYPE_HIDDEN,
    HAPI_ATTRIBUTE_TYPE_BOX2,
    HAPI_ATTRIBUTE_TYPE_BOX,
    HAPI_ATTRIBUTE_TYPE_TEXTURE,
    HAPI_ATTRIBUTE_TYPE_MAX
};

enum HAPI_GeoType
{
    HAPI_GEOTYPE_INVALID = -1,
    HAPI_GEOTYPE_DEFAULT,
    HAPI_GEOTYPE_INTERMEDIATE,
    HAPI_GEOTYPE_INPUT,
    HAPI_GEOTYPE_CURVE,
    HAPI_GEOTYPE_MAX
};

enum HAPI_PartType
{
    HAPI_PARTTYPE_INVALID = -1,
    HAPI_PARTTYPE_MESH,
    HAPI_PARTTYPE_CURVE,
    HAPI_PARTTYPE_VOLUME,
    HAPI_PARTTYPE_INSTANCER,
    HAPI_PARTTYPE_BOX,
    HAPI_PARTTYPE_SPHERE,
    HAPI_PARTTYPE_MAX
};

enum HAPI_InputType
{
    HAPI_INPUT_INVALID = -1,
    HAPI_INPUT_TRANSFORM,
    HAPI_INPUT_GEOMETRY,
    HAPI_INPUT_MAX
};

enum HAPI_CurveOrders
{
    HAPI_CURVE_ORDER_VARYING = 0,
    HAPI_CURVE_ORDER_INVALID = 1,
    HAPI_CURVE_ORDER_LINEAR = 2,
    HAPI_CURVE_ORDER_QUADRATIC = 3,
    HAPI_CURVE_ORDER_CUBIC = 4,
};

enum HAPI_TransformComponent
{
    HAPI_TRANSFORM_TX = 0,
    HAPI_TRANSFORM_TY,
    HAPI_TRANSFORM_TZ,
    HAPI_TRANSFORM_RX,
    HAPI_TRANSFORM_RY,
    HAPI_TRANSFORM_RZ,
    HAPI_TRANSFORM_QX,
    HAPI_TRANSFORM_QY,
    HAPI_TRANSFORM_QZ,
    HAPI_TRANSFORM_QW,
    HAPI_TRANSFORM_SX,
    HAPI_TRANSFORM_SY,
    HAPI_TRANSFORM_SZ
};

enum HAPI_RSTOrder
{
    HAPI_TRS = 0,
    HAPI_TSR,
    HAPI_RTS,
    HAPI_RST,
    HAPI_STR,
    HAPI_SRT,
    HAPI_RSTORDER_DEFAULT = HAPI_SRT
};

enum HAPI_XYZOrder
{
    HAPI_XYZ = 0,
    HAPI_XZY,
    HAPI_YXZ,
    HAPI_YZX,
    HAPI_ZXY,
    HAPI_ZYX,
    HAPI_XYZORDER_DEFAULT = HAPI_XYZ
};

enum HAPI_ImageDataFormat
{
    HAPI_IMAGE_DATA_UNKNOWN = -1,
    HAPI_IMAGE_DATA_INT8,
    HAPI_IMAGE_DATA_INT16,
    HAPI_IMAGE_DATA_INT32,
    HAPI_IMAGE_DATA_FLOAT16,
    HAPI_IMAGE_DATA_FLOAT32,
    HAPI_IMAGE_DATA_MAX,
    HAPI_IMAGE_DATA_DEFAULT = HAPI_IMAGE_DATA_INT8
};

enum HAPI_ImagePacking
{
    HAPI_IMAGE_PACKING_UNKNOWN = -1,
    HAPI_IMAGE_PACKING_SINGLE,  
    HAPI_IMAGE_PACKING_DUAL,    
    HAPI_IMAGE_PACKING_RGB,     
    HAPI_IMAGE_PACKING_BGR,     
    HAPI_IMAGE_PACKING_RGBA,    
    HAPI_IMAGE_PACKING_ABGR,    
    HAPI_IMAGE_PACKING_MAX,
    HAPI_IMAGE_PACKING_DEFAULT3 = HAPI_IMAGE_PACKING_RGB,
    HAPI_IMAGE_PACKING_DEFAULT4 = HAPI_IMAGE_PACKING_RGBA
};

enum HAPI_EnvIntType
{
    HAPI_ENVINT_INVALID = -1,
    HAPI_ENVINT_VERSION_HOUDINI_MAJOR = 100,
    HAPI_ENVINT_VERSION_HOUDINI_MINOR = 110,
    HAPI_ENVINT_VERSION_HOUDINI_BUILD = 120,
    HAPI_ENVINT_VERSION_HOUDINI_PATCH = 130,
    HAPI_ENVINT_VERSION_HOUDINI_ENGINE_MAJOR = 200,
    HAPI_ENVINT_VERSION_HOUDINI_ENGINE_MINOR = 210,
    HAPI_ENVINT_VERSION_HOUDINI_ENGINE_API = 220,
    HAPI_ENVINT_MAX,
};

enum HAPI_SessionEnvIntType
{
    HAPI_SESSIONENVINT_INVALID = -1,
    HAPI_SESSIONENVINT_LICENSE = 100,
    HAPI_SESSIONENVINT_MAX
};

enum HAPI_CacheProperty
{
    HAPI_CACHEPROP_CURRENT,
    HAPI_CACHEPROP_HAS_MIN, 
    HAPI_CACHEPROP_MIN, 
    HAPI_CACHEPROP_HAS_MAX, 
    HAPI_CACHEPROP_MAX, 
    HAPI_CACHEPROP_CULL_LEVEL,
};

enum HAPI_HeightFieldSampling
{
    HAPI_HEIGHTFIELD_SAMPLING_CENTER,
    HAPI_HEIGHTFIELD_SAMPLING_CORNER
};

enum HAPI_PDG_State
{
    HAPI_PDG_STATE_READY,
    HAPI_PDG_STATE_COOKING,
    HAPI_PDG_STATE_MAX,
    HAPI_PDG_STATE_MAX_READY_STATE = HAPI_PDG_STATE_READY
};

enum HAPI_PDG_EventType
{
    HAPI_PDG_EVENT_NULL,
    HAPI_PDG_EVENT_WORKITEM_ADD,
    HAPI_PDG_EVENT_WORKITEM_REMOVE,
    HAPI_PDG_EVENT_WORKITEM_STATE_CHANGE,
    HAPI_PDG_EVENT_WORKITEM_ADD_DEP,
    HAPI_PDG_EVENT_WORKITEM_REMOVE_DEP,
    HAPI_PDG_EVENT_WORKITEM_ADD_PARENT,
    HAPI_PDG_EVENT_WORKITEM_REMOVE_PARENT,
    HAPI_PDG_EVENT_NODE_CLEAR,
    HAPI_PDG_EVENT_COOK_ERROR,
    HAPI_PDG_EVENT_COOK_WARNING,
    HAPI_PDG_EVENT_COOK_COMPLETE,
    HAPI_PDG_EVENT_DIRTY_START,
    HAPI_PDG_EVENT_DIRTY_STOP,
    HAPI_PDG_EVENT_DIRTY_ALL,
    HAPI_PDG_EVENT_UI_SELECT,
    HAPI_PDG_EVENT_NODE_CREATE,
    HAPI_PDG_EVENT_NODE_REMOVE,
    HAPI_PDG_EVENT_NODE_RENAME,
    HAPI_PDG_EVENT_NODE_CONNECT,
    HAPI_PDG_EVENT_NODE_DISCONNECT,
    HAPI_PDG_EVENT_NODE_FIRST_COOK,
    HAPI_PDG_EVENT_WORKITEM_SET_INT,
    HAPI_PDG_EVENT_WORKITEM_SET_FLOAT,
    HAPI_PDG_EVENT_WORKITEM_SET_STRING,
    HAPI_PDG_EVENT_WORKITEM_SET_FILE,
    HAPI_PDG_EVENT_WORKITEM_SET_DICT,
    HAPI_PDG_EVENT_WORKITEM_SET_PYOBJECT,
    HAPI_PDG_EVENT_WORKITEM_SET_GEOMETRY,
    HAPI_PDG_EVENT_WORKITEM_MERGE,
    HAPI_PDG_EVENT_WORKITEM_RESULT,
    HAPI_PDG_EVENT_WORKITEM_PRIORITY,
    HAPI_PDG_EVENT_COOK_START,
    HAPI_PDG_EVENT_WORKITEM_ADD_STATIC_ANCESTOR,
    HAPI_PDG_EVENT_WORKITEM_REMOVE_STATIC_ANCESTOR,
    HAPI_PDG_EVENT_NODE_PROGRESS_UPDATE,
    HAPI_PDG_EVENT_BATCH_ITEM_INITIALIZED,
    HAPI_PDG_EVENT_ALL,
    HAPI_PDG_EVENT_LOG,
    HAPI_PDG_EVENT_SCHEDULER_ADDED,
    HAPI_PDG_EVENT_SCHEDULER_REMOVED,
    HAPI_PDG_EVENT_SET_SCHEDULER,
    HAPI_PDG_EVENT_SERVICE_MANAGER_ALL,
    HAPI_PDG_EVENT_NODE_COOKED,
    HAPI_PDG_EVENT_NODE_GENERATED,
    HAPI_PDG_CONTEXT_EVENTS,
};

enum HAPI_PDG_WorkItemState
{
    HAPI_PDG_WORKITEM_UNDEFINED,
    HAPI_PDG_WORKITEM_UNCOOKED,
    HAPI_PDG_WORKITEM_WAITING,
    HAPI_PDG_WORKITEM_SCHEDULED,
    HAPI_PDG_WORKITEM_COOKING,
    HAPI_PDG_WORKITEM_COOKED_SUCCESS,
    HAPI_PDG_WORKITEM_COOKED_CACHE,
    HAPI_PDG_WORKITEM_COOKED_FAIL,
    HAPI_PDG_WORKITEM_COOKED_CANCEL,
    HAPI_PDG_WORKITEM_DIRTY
};

enum HAPI_TCP_PortType
{
    HAPI_TCP_PORT_ANY,
    HAPI_TCP_PORT_RANGE,
    HAPI_TCP_PORT_LIST
};

enum HAPI_ThriftSharedMemoryBufferType
{
    HAPI_THRIFT_SHARED_MEMORY_FIXED_LENGTH_BUFFER,
    HAPI_THRIFT_SHARED_MEMORY_RING_BUFFER
};
struct HAPI_Transform
{
    float position[ HAPI_POSITION_VECTOR_SIZE ];
    float rotationQuaternion[ HAPI_QUATERNION_VECTOR_SIZE ];
    float scale[ HAPI_SCALE_VECTOR_SIZE ];
    float shear[ HAPI_SHEAR_VECTOR_SIZE ];
    HAPI_RSTOrder rstOrder;
};

struct HAPI_TransformEuler
{
    float position[ HAPI_POSITION_VECTOR_SIZE ];
    float rotationEuler[ HAPI_EULER_VECTOR_SIZE ];
    float scale[ HAPI_SCALE_VECTOR_SIZE ];
    float shear[ HAPI_SHEAR_VECTOR_SIZE ];
    HAPI_XYZOrder rotationOrder;
    HAPI_RSTOrder rstOrder;
};

struct HAPI_Session
{
    HAPI_SessionType type;
    HAPI_SessionId id;
};

struct HAPI_SessionInfo
{
    int connectionCount;
    HAPI_TCP_PortType portType;
    int minPort;
    int maxPort;
    int ports[ HAPI_MAX_NUM_CONNECTIONS ];
    HAPI_ThriftSharedMemoryBufferType sharedMemoryBufferType;
    int64_t sharedMemoryBufferSize;
};

struct HAPI_ThriftServerOptions
{
    bool autoClose;
    float timeoutMs;
    HAPI_StatusVerbosity verbosity;
    HAPI_ThriftSharedMemoryBufferType sharedMemoryBufferType;
    int64_t sharedMemoryBufferSize;
};

struct HAPI_TimelineOptions
{
    float fps;
    float startTime;
    float endTime;
};

struct HAPI_AssetInfo
{
    HAPI_NodeId nodeId;
    HAPI_NodeId objectNodeId;
    bool hasEverCooked;
    HAPI_StringHandle nameSH; 
    HAPI_StringHandle labelSH; 
    HAPI_StringHandle filePathSH; 
    HAPI_StringHandle versionSH; 
    HAPI_StringHandle fullOpNameSH; 
    HAPI_StringHandle helpTextSH; 
    HAPI_StringHandle helpURLSH; 
    int objectCount; 
    int handleCount; 
    int transformInputCount;
    int geoInputCount;
    int geoOutputCount;
    bool haveObjectsChanged;
    bool haveMaterialsChanged;
};

struct HAPI_CookOptions
{
    bool splitGeosByGroup;
    HAPI_StringHandle splitGroupSH;
    bool splitGeosByAttribute;
    HAPI_StringHandle splitAttrSH;
    int maxVerticesPerPrimitive;
    bool refineCurveToLinear;
    float curveRefineLOD;
    bool clearErrorsAndWarnings;
    bool cookTemplatedGeos;
    bool splitPointsByVertexAttributes;
    HAPI_PackedPrimInstancingMode packedPrimInstancingMode;
    bool handleBoxPartTypes;
    bool handleSpherePartTypes;
    bool checkPartChanges;
    bool cacheMeshTopology;
    bool preferOutputNodes;
    int extraFlags;
};

struct HAPI_NodeInfo
{
    HAPI_NodeId id;
    HAPI_NodeId parentId;
    HAPI_StringHandle nameSH;
    HAPI_NodeType type;
    bool isValid;
    int totalCookCount;
    int uniqueHoudiniNodeId;
    HAPI_StringHandle internalNodePathSH;
    int parmCount;
    int parmIntValueCount;
    int parmFloatValueCount;
    int parmStringValueCount;
    int parmChoiceCount;
    int childNodeCount;
    int inputCount;
    int outputCount;
    bool createdPostAssetLoad;
    bool isTimeDependent;
};

struct HAPI_ParmInfo
{
    HAPI_ParmId id;
    HAPI_ParmId parentId;
    int childIndex;
    HAPI_ParmType type;
    HAPI_PrmScriptType scriptType;
    HAPI_StringHandle typeInfoSH;
    HAPI_Permissions permissions;
    int tagCount;
    int size;
    HAPI_ChoiceListType choiceListType;
    int choiceCount;
    HAPI_StringHandle nameSH;
    HAPI_StringHandle labelSH;
    HAPI_StringHandle templateNameSH;
    HAPI_StringHandle helpSH;
    bool hasMin;
    bool hasMax;
    bool hasUIMin;
    bool hasUIMax;
    float min;
    float max;
    float UIMin;
    float UIMax;
    bool invisible;
    bool disabled;
    bool spare;
    bool joinNext;  
    bool labelNone; 
    int intValuesIndex;
    int floatValuesIndex;
    int stringValuesIndex;
    int choiceIndex;
    HAPI_NodeType inputNodeType;
    HAPI_NodeFlags inputNodeFlag;
    bool isChildOfMultiParm;
    int instanceNum; 
    int instanceLength; 
    int instanceCount; 
    int instanceStartOffset;
    HAPI_RampType rampType;
    HAPI_StringHandle visibilityConditionSH;
    HAPI_StringHandle disabledConditionSH;
    bool useMenuItemTokenAsValue;
};

struct HAPI_ParmChoiceInfo
{
    HAPI_ParmId parentParmId;
    HAPI_StringHandle labelSH;
    HAPI_StringHandle valueSH;
};

struct HAPI_HandleInfo
{
    HAPI_StringHandle nameSH;
    HAPI_StringHandle typeNameSH;
    int bindingsCount;
};

struct HAPI_HandleBindingInfo
{
    HAPI_StringHandle handleParmNameSH;
    HAPI_StringHandle  assetParmNameSH;
    HAPI_ParmId assetParmId;
    int assetParmIndex;
};

struct HAPI_ObjectInfo
{
    HAPI_StringHandle nameSH;
    HAPI_StringHandle objectInstancePathSH;
    bool hasTransformChanged;
    bool haveGeosChanged;
    bool isVisible;
    bool isInstancer;
    bool isInstanced;
    int geoCount;
    HAPI_NodeId nodeId;
    HAPI_NodeId objectToInstanceId;
};

struct HAPI_GeoInfo
{
    HAPI_GeoType type;
    HAPI_StringHandle nameSH;
    HAPI_NodeId nodeId;
    bool isEditable;
    bool isTemplated;
    bool isDisplayGeo;
    bool hasGeoChanged;
    bool hasMaterialChanged;
    int pointGroupCount;
    int primitiveGroupCount;
    int edgeGroupCount;
    int partCount;
};

struct HAPI_PartInfo
{
    HAPI_PartId id;
    HAPI_StringHandle nameSH;
    HAPI_PartType type;
    int faceCount;
    int vertexCount;
    int pointCount;
    int attributeCounts[ HAPI_ATTROWNER_MAX ];
    bool isInstanced;
    int instancedPartCount;
    int instanceCount;
    bool hasChanged;
};

struct HAPI_AttributeInfo
{
    bool exists;
    HAPI_AttributeOwner owner;
    HAPI_StorageType storage;
    HAPI_AttributeOwner originalOwner;
    int count;
    int tupleSize;
    int64_t totalArrayElements;
    HAPI_AttributeTypeInfo typeInfo;
};

struct HAPI_MaterialInfo
{
    HAPI_NodeId nodeId;
    bool exists;
    bool hasChanged;
};

struct HAPI_ImageFileFormat
{
    HAPI_StringHandle nameSH;
    HAPI_StringHandle descriptionSH;
    HAPI_StringHandle defaultExtensionSH;
};

struct HAPI_ImageInfo
{
    HAPI_StringHandle imageFileFormatNameSH; 
    int xRes;
    int yRes;
    HAPI_ImageDataFormat dataFormat;
    bool interleaved; 
    HAPI_ImagePacking packing;
    double gamma;
};

struct HAPI_Keyframe
{
    float time;
    float value;
    float inTangent;
    float outTangent;
};

struct HAPI_VolumeInfo
{
    HAPI_StringHandle nameSH;
    HAPI_VolumeType type;
    int xLength;
    int yLength;
    int zLength;
    int minX;
    int minY;
    int minZ;
    int tupleSize;
    HAPI_StorageType storage;
    int tileSize;
    HAPI_Transform transform;
    bool hasTaper;
    float xTaper;
    float yTaper;
};

struct HAPI_VolumeTileInfo
{
    int minX;
    int minY;
    int minZ;
    bool isValid;
};

struct HAPI_VolumeVisualInfo
{
    HAPI_VolumeVisualType type;
    float iso;
    float density;
};

struct HAPI_CurveInfo
{
    HAPI_CurveType curveType;
    int curveCount;
    int vertexCount;
    int knotCount;
    bool isPeriodic;
    bool isRational;
    int order;
    bool hasKnots;
    bool isClosed;
};

struct HAPI_InputCurveInfo
{
    HAPI_CurveType curveType;
    int order;
    bool closed;
    bool reverse;
    HAPI_InputCurveMethod inputMethod;
    HAPI_InputCurveParameterization breakpointParameterization;
};

struct HAPI_BoxInfo
{
    float center[ HAPI_POSITION_VECTOR_SIZE ];
    float size[ HAPI_SCALE_VECTOR_SIZE ];
    float rotation[ HAPI_EULER_VECTOR_SIZE ];
};

struct HAPI_SphereInfo
{
    float center[ HAPI_POSITION_VECTOR_SIZE ];
    float radius;
};

struct HAPI_PDG_EventInfo
{
    HAPI_NodeId nodeId;
    HAPI_PDG_WorkItemId workItemId;
    HAPI_PDG_WorkItemId dependencyId;
    int currentState;
    int lastState;
    int eventType;
    HAPI_StringHandle msgSH;
};

struct HAPI_PDG_WorkItemInfo
{
    int index;
    int outputFileCount;
    HAPI_StringHandle nameSH; 
};

struct HAPI_PDG_WorkItemOutputFile
{
    int filePathSH;
    int tagSH;
    int64_t hash;
};

struct HAPI_Viewport
{
    float position[ HAPI_POSITION_VECTOR_SIZE ];
    float rotationQuaternion[ HAPI_QUATERNION_VECTOR_SIZE ];
    float offset;
};

struct HAPI_SessionSyncInfo
{
    bool cookUsingHoudiniTime;
    bool syncViewport;
};

struct HAPI_CompositorOptions
{
    int maximumResolutionX;
    int maximumResolutionY;
};

#endif // HOUDINI_STRUCT_H