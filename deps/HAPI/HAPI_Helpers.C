/*
 * PROPRIETARY INFORMATION.  This software is proprietary to
 * Side Effects Software Inc., and is not to be reproduced,
 * transmitted, or disclosed in any way without written permission.
 *
 * COMMENTS:
 *      Methods for initializing HAPI structs with the correct default values.
 */

#include "HAPI_Helpers.h"

#include "HAPI.h"

// Session ------------------------------------------------------------------

HAPI_DECL_RETURN( void )
    HAPI_SessionInfo_Init( HAPI_SessionInfo * in )
{
    in->connectionCount = 0;
    in->portType = HAPI_TCP_PORT_ANY;
    in->minPort = 1024;
    in->maxPort = 65535;
    in->sharedMemoryBufferType = HAPI_THRIFT_SHARED_MEMORY_FIXED_LENGTH_BUFFER;
    in->sharedMemoryBufferSize = 100; // 100 MB
}

HAPI_DECL_RETURN( HAPI_SessionInfo )
    HAPI_SessionInfo_Create()
{
    HAPI_SessionInfo info;
    HAPI_SessionInfo_Init( &info );
    return info;
}

// Server -------------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_ThriftServerOptions_Init( HAPI_ThriftServerOptions * in )
{
    in->autoClose = true;
    in->timeoutMs = 30000.f;
    in->verbosity = HAPI_STATUSVERBOSITY_WARNINGS;
    in->sharedMemoryBufferType = HAPI_THRIFT_SHARED_MEMORY_FIXED_LENGTH_BUFFER;
    in->sharedMemoryBufferSize = 100; // 100 MB
}

HAPI_DECL_RETURN( HAPI_ThriftServerOptions )
HAPI_ThriftServerOptions_Create()
{
    HAPI_ThriftServerOptions options;
    HAPI_ThriftServerOptions_Init( &options );
    return options;
}

// TIME ---------------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_TimelineOptions_Init( HAPI_TimelineOptions * in )
{
    in->fps = 0.0f;

    in->startTime = 0.0f;
    in->endTime = 0.0f;
}

HAPI_DECL_RETURN( HAPI_TimelineOptions )
HAPI_TimelineOptions_Create()
{
    HAPI_TimelineOptions info;
    HAPI_TimelineOptions_Init( &info );
    return info;
}

// COMPOSITOR SETTINGS ------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_CompositorOptions_Init( HAPI_CompositorOptions * in )
{
    in->maximumResolutionX = 10000;
    in->maximumResolutionY = 10000;
}

HAPI_DECL_RETURN( HAPI_CompositorOptions )
HAPI_CompositorOptions_Create()
{
    HAPI_CompositorOptions settings;
    HAPI_CompositorOptions_Init(&settings);
    return settings;
}

// ASSETS -------------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_AssetInfo_Init( HAPI_AssetInfo * in )
{
    in->nodeId = -1;
    in->objectNodeId = -1;

    in->hasEverCooked = false;

    in->nameSH = 0;
    in->labelSH = 0;
    in->filePathSH = 0;

    in->versionSH = 0;
    in->fullOpNameSH = 0;
    in->helpTextSH = 0;
    in->helpURLSH = 0;

    in->objectCount = -1;
    in->handleCount = -1;

    in->transformInputCount = -1;
    in->geoInputCount = -1;
    in->geoOutputCount = -1;

    in->haveObjectsChanged = false;
    in->haveMaterialsChanged = false;
}

HAPI_DECL_RETURN( HAPI_AssetInfo )
HAPI_AssetInfo_Create()
{
    HAPI_AssetInfo info;
    HAPI_AssetInfo_Init( &info );
    return info;
}

HAPI_DECL_RETURN( void )
HAPI_CookOptions_Init( HAPI_CookOptions * in )
{
    in->splitGeosByGroup = false;
    in->splitGroupSH = 0;
    in->splitGeosByAttribute = false;
    in->splitAttrSH = 0;

    in->maxVerticesPerPrimitive = -1;

    in->refineCurveToLinear = false;
    in->curveRefineLOD = 8.0f;

    in->clearErrorsAndWarnings = false;

    in->cookTemplatedGeos = false;

    in->splitPointsByVertexAttributes = false;

    in->packedPrimInstancingMode = HAPI_PACKEDPRIM_INSTANCING_MODE_DISABLED;

    in->handleBoxPartTypes = false;
    in->handleSpherePartTypes = false;

    in->checkPartChanges = false;

    in->cacheMeshTopology = false;

    in->preferOutputNodes = false;

    in->extraFlags = 0;
}

HAPI_DECL_RETURN( HAPI_CookOptions )
HAPI_CookOptions_Create()
{
    HAPI_CookOptions info;
    HAPI_CookOptions_Init( &info );
    return info;
}

HAPI_DECL_RETURN( HAPI_Bool )
HAPI_CookOptions_AreEqual(
    const HAPI_CookOptions * left,
    const HAPI_CookOptions * right )
{
    // Note: DO NOT check the extraFlags here. They do not exist!

    return left->splitGeosByGroup == right->splitGeosByGroup
        && left->splitGroupSH == right->splitGroupSH
        && left->splitGeosByAttribute == right->splitGeosByAttribute
        && left->splitAttrSH == right->splitAttrSH
        && left->maxVerticesPerPrimitive == right->maxVerticesPerPrimitive
        && left->refineCurveToLinear == right->refineCurveToLinear
        && left->curveRefineLOD == right->curveRefineLOD
        && left->cookTemplatedGeos == right->cookTemplatedGeos
        && left->packedPrimInstancingMode == right->packedPrimInstancingMode
        && left->handleBoxPartTypes == right->handleBoxPartTypes
        && left->handleSpherePartTypes == right->handleSpherePartTypes
        && left->cacheMeshTopology == right->cacheMeshTopology;
}

// NODES --------------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_NodeInfo_Init( HAPI_NodeInfo * in )
{
    in->id = -1;
    in->parentId = -1;
    in->nameSH = 0;
    in->type = HAPI_NODETYPE_NONE;

    in->isValid = true;

    in->totalCookCount = 0;

    in->uniqueHoudiniNodeId = -1;
    in->internalNodePathSH = 0;

    in->parmCount = 0;
    in->parmIntValueCount = 0;
    in->parmFloatValueCount = 0;
    in->parmStringValueCount = 0;
    in->parmChoiceCount = 0;

    in->childNodeCount = 0;
    in->inputCount = 0;
    in->outputCount = 0;
    in->createdPostAssetLoad = false;
}

HAPI_DECL_RETURN( HAPI_NodeInfo )
HAPI_NodeInfo_Create()
{
    HAPI_NodeInfo info;
    HAPI_NodeInfo_Init( &info );
    return info;
}

// PARAMETERS ---------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_ParmInfo_Init( HAPI_ParmInfo * in )
{
    in->id = HAPI_INVALID_PARM_ID;
    in->parentId = HAPI_INVALID_PARM_ID;
    in->childIndex = 0;

    in->type = HAPI_PARMTYPE_INT;
    in->scriptType = HAPI_PRM_SCRIPT_TYPE_INT;
    in->typeInfoSH = 0;

    in->permissions = HAPI_PERMISSIONS_NON_APPLICABLE;

    in->tagCount = 0;

    in->size = 0;

    in->choiceListType = HAPI_CHOICELISTTYPE_NONE;
    in->choiceCount = 0;

    in->nameSH = 0;
    in->labelSH = 0;
    in->templateNameSH = 0;
    in->helpSH = 0;

    in->hasMin = false;
    in->hasMax = false;
    in->hasUIMin = false;
    in->hasUIMax = false;

    in->min = 0.0f;
    in->max = 10.0f;
    in->UIMin = 0.0f;
    in->UIMax = 10.0f;

    in->invisible = false;
    in->disabled = false;
    in->spare = false;
    in->joinNext = false;
    in->labelNone = false;

    in->intValuesIndex = -1;
    in->floatValuesIndex = -1;
    in->stringValuesIndex = -1;
    in->choiceIndex = -1;

    in->inputNodeType = HAPI_NODETYPE_NONE;
    in->inputNodeFlag = HAPI_NODEFLAGS_ANY;

    in->isChildOfMultiParm = false;
    in->instanceNum = -1;
    in->instanceLength = 0;
    in->instanceCount = 0;
    in->instanceStartOffset = -1;

    in->rampType = HAPI_RAMPTYPE_INVALID;

    in->visibilityConditionSH = 0;
    in->disabledConditionSH = 0;
}

HAPI_DECL_RETURN( HAPI_ParmInfo )
HAPI_ParmInfo_Create()
{
    HAPI_ParmInfo info;
    HAPI_ParmInfo_Init( &info );
    return info;
}

HAPI_DECL_RETURN( bool )
HAPI_ParmInfo_IsInt( const HAPI_ParmInfo * in )
{
    return ( in->type >= HAPI_PARMTYPE_INT_START
             && in->type <= HAPI_PARMTYPE_INT_END ) ||
        in->type == HAPI_PARMTYPE_MULTIPARMLIST ||
        in->type == HAPI_PARMTYPE_FOLDERLIST_RADIO;
}

HAPI_DECL_RETURN( bool )
HAPI_ParmInfo_IsFloat( const HAPI_ParmInfo * in )
{
    return ( in->type >= HAPI_PARMTYPE_FLOAT_START
             && in->type <= HAPI_PARMTYPE_FLOAT_END );
}

HAPI_DECL_RETURN( bool )
HAPI_ParmInfo_IsString( const HAPI_ParmInfo * in )
{
    return ( ( in->type >= HAPI_PARMTYPE_STRING_START && in->type <= HAPI_PARMTYPE_STRING_END )
        || ( in->type == HAPI_PARMTYPE_LABEL )
	|| ( in->type == HAPI_PARMTYPE_PATH_FILE_DIR) );
}

HAPI_DECL_RETURN( bool )
HAPI_ParmInfo_IsPath( const HAPI_ParmInfo * in )
{
  return ( (in->type >= HAPI_PARMTYPE_PATH_START
	    && in->type <= HAPI_PARMTYPE_PATH_END)
	    || ( in->type == HAPI_PARMTYPE_PATH_FILE_DIR));
}

HAPI_DECL_RETURN( bool )
HAPI_ParmInfo_IsNode( const HAPI_ParmInfo * in )
{
    return ( in->type >= HAPI_PARMTYPE_NODE_START
             && in->type <= HAPI_PARMTYPE_NODE_END );
}

HAPI_DECL_RETURN( bool )
HAPI_ParmInfo_IsNonValue( const HAPI_ParmInfo * in )
{
    return ( in->type >= HAPI_PARMTYPE_NONVALUE_START
             && in->type <= HAPI_PARMTYPE_NONVALUE_END );
}

HAPI_DECL_RETURN( int )
HAPI_ParmInfo_GetIntValueCount( const HAPI_ParmInfo * in )
{
    if ( HAPI_ParmInfo_IsInt( in ) )
        return in->size;
    else
        return 0;
}

HAPI_DECL_RETURN( int )
HAPI_ParmInfo_GetFloatValueCount( const HAPI_ParmInfo * in )
{
    if ( HAPI_ParmInfo_IsFloat( in ) )
        return in->size;
    else
        return 0;
}

HAPI_DECL_RETURN( int )
HAPI_ParmInfo_GetStringValueCount( const HAPI_ParmInfo * in )
{
    if ( HAPI_ParmInfo_IsString( in ) )
        return in->size;
    else
        return 0;
}

HAPI_DECL_RETURN( void )
HAPI_ParmChoiceInfo_Init( HAPI_ParmChoiceInfo * in )
{
    in->parentParmId = 0;
    in->labelSH = 0;
    in->valueSH = 0;
}

HAPI_DECL_RETURN( HAPI_ParmChoiceInfo )
HAPI_ParmChoiceInfo_Create()
{
    HAPI_ParmChoiceInfo info;
    HAPI_ParmChoiceInfo_Init( &info );
    return info;
}

// HANDLES ------------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_HandleInfo_Init( HAPI_HandleInfo * in )
{
    in->nameSH = 0;
    in->typeNameSH = 0;
    in->bindingsCount = 0;
}

HAPI_DECL_RETURN( HAPI_HandleInfo )
HAPI_HandleInfo_Create()
{
    HAPI_HandleInfo info;
    HAPI_HandleInfo_Init( &info );
    return info;
}

HAPI_DECL_RETURN( void )
HAPI_HandleBindingInfo_Init( HAPI_HandleBindingInfo * in )
{
    in->handleParmNameSH = 0;
    in->assetParmNameSH = 0;
    in->assetParmId = -1;
}

HAPI_DECL_RETURN( HAPI_HandleBindingInfo )
HAPI_HandleBindingInfo_Create()
{
    HAPI_HandleBindingInfo info;
    HAPI_HandleBindingInfo_Init( &info );
    return info;
}

// OBJECTS ------------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_ObjectInfo_Init( HAPI_ObjectInfo * in )
{
    in->nameSH = 0;
    in->objectInstancePathSH = 0;

    in->hasTransformChanged = true;
    in->haveGeosChanged = true;

    in->isVisible = true;
    in->isInstancer = false;
    in->isInstanced = false;

    in->geoCount = 0;

    in->nodeId = -1;
    in->objectToInstanceId = -1;
}

HAPI_DECL_RETURN( HAPI_ObjectInfo )
HAPI_ObjectInfo_Create()
{
    HAPI_ObjectInfo out;
    HAPI_ObjectInfo_Init( &out );
    return out;
}

// GEOMETRY -----------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_GeoInfo_Init( HAPI_GeoInfo * in )
{
    in->type = HAPI_GEOTYPE_INVALID;
    in->nameSH = 0;
    in->nodeId = -1;

    in->isEditable = false;
    in->isTemplated = false;
    in->isDisplayGeo = true;

    in->hasGeoChanged = true;
    in->hasMaterialChanged = true;

    in->pointGroupCount = 0;
    in->primitiveGroupCount = 0;
    in->edgeGroupCount = 0;

    in->partCount = 1;
}

HAPI_DECL_RETURN( HAPI_GeoInfo )
HAPI_GeoInfo_Create()
{
    HAPI_GeoInfo info;
    HAPI_GeoInfo_Init( &info );
    return info;
}

HAPI_DECL_RETURN( int )
HAPI_GeoInfo_GetGroupCountByType(
    HAPI_GeoInfo * in, HAPI_GroupType type )
{
    if ( !in )
        return 0;

    switch ( type )
    {
        case HAPI_GROUPTYPE_POINT: return in->pointGroupCount;
        case HAPI_GROUPTYPE_PRIM: return in->primitiveGroupCount;
        case HAPI_GROUPTYPE_EDGE: return in->edgeGroupCount;
        default: return 0;
    }
}

HAPI_DECL_RETURN( void )
HAPI_PartInfo_Init( HAPI_PartInfo * in )
{
    in->id = 0;
    in->nameSH = 0;
    in->type = HAPI_PARTTYPE_INVALID;

    in->faceCount = 0;
    in->vertexCount = 0;
    in->pointCount = 0;

    in->attributeCounts[ HAPI_ATTROWNER_POINT ] = 0;
    in->attributeCounts[ HAPI_ATTROWNER_PRIM ] = 0;
    in->attributeCounts[ HAPI_ATTROWNER_VERTEX ] = 0;
    in->attributeCounts[ HAPI_ATTROWNER_DETAIL ] = 0;

    in->isInstanced = false;
    in->instancedPartCount = 0;
    in->instanceCount = 0;
    in->hasChanged = true;
}

HAPI_DECL_RETURN( HAPI_PartInfo )
HAPI_PartInfo_Create()
{
    HAPI_PartInfo info;
    HAPI_PartInfo_Init( &info );
    return info;
}

HAPI_DECL_RETURN( int )
HAPI_PartInfo_GetElementCountByAttributeOwner(
    HAPI_PartInfo * in, HAPI_AttributeOwner owner )
{
    if ( !in )
        return 0;

    switch ( owner )
    {
        case HAPI_ATTROWNER_VERTEX: return in->vertexCount;
        case HAPI_ATTROWNER_POINT: return in->pointCount;
        case HAPI_ATTROWNER_PRIM: return in->faceCount;
        case HAPI_ATTROWNER_DETAIL: return 1;
        default: return 0;
    }
}

HAPI_DECL_RETURN( int )
HAPI_PartInfo_GetElementCountByGroupType(
    HAPI_PartInfo * in, HAPI_GroupType type )
{
    if ( !in )
        return 0;

    switch ( type )
    {
        case HAPI_GROUPTYPE_POINT: return in->pointCount;
        case HAPI_GROUPTYPE_PRIM: return in->faceCount;
        default: return 0;
    }
}

HAPI_DECL_RETURN( int )
HAPI_PartInfo_GetAttributeCountByOwner(
    HAPI_PartInfo * in, HAPI_AttributeOwner owner )
{
    if ( !in )
        return 0;

    switch ( owner )
    {
        case HAPI_ATTROWNER_VERTEX: return in->attributeCounts[ HAPI_ATTROWNER_VERTEX ];
        case HAPI_ATTROWNER_POINT: return in->attributeCounts[ HAPI_ATTROWNER_POINT ];
        case HAPI_ATTROWNER_PRIM: return in->attributeCounts[ HAPI_ATTROWNER_PRIM ];
        case HAPI_ATTROWNER_DETAIL: return in->attributeCounts[ HAPI_ATTROWNER_DETAIL ];
        default: return 0;
    }
}

HAPI_DECL_RETURN( void )
HAPI_AttributeInfo_Init( HAPI_AttributeInfo * in )
{
    in->exists = false;

    in->owner = HAPI_ATTROWNER_INVALID;
    in->storage = HAPI_STORAGETYPE_INVALID;

    in->originalOwner = HAPI_ATTROWNER_INVALID;

    in->count = 0;
    in->tupleSize = 0;

    in->totalArrayElements = 0;
    in->typeInfo = HAPI_ATTRIBUTE_TYPE_NONE;
}

HAPI_DECL_RETURN( HAPI_AttributeInfo )
HAPI_AttributeInfo_Create()
{
    HAPI_AttributeInfo info;
    HAPI_AttributeInfo_Init( &info );
    return info;
}

// MATERIALS ----------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_MaterialInfo_Init( HAPI_MaterialInfo * in )
{
    in->nodeId = 0;
    in->exists = false;
    in->hasChanged = true;
}

HAPI_DECL_RETURN( HAPI_MaterialInfo )
HAPI_MaterialInfo_Create()
{
    HAPI_MaterialInfo info;
    HAPI_MaterialInfo_Init( &info );
    return info;
}

HAPI_DECL_RETURN( void )
HAPI_ImageFileFormat_Init( HAPI_ImageFileFormat * in )
{
    in->nameSH = 0;
    in->descriptionSH = 0;
    in->defaultExtensionSH = 0;
}

HAPI_DECL_RETURN( HAPI_ImageFileFormat )
HAPI_ImageFileFormat_Create()
{
    HAPI_ImageFileFormat info;
    HAPI_ImageFileFormat_Init( &info );
    return info;
}

HAPI_DECL_RETURN( void )
HAPI_ImageInfo_Init( HAPI_ImageInfo * in )
{
    in->imageFileFormatNameSH = 0;

    in->xRes = 0;
    in->yRes = 0;

    in->dataFormat = HAPI_IMAGE_DATA_DEFAULT;

    in->interleaved = true;
    in->packing = HAPI_IMAGE_PACKING_DEFAULT4;

    in->gamma = 2.2;
}

HAPI_DECL_RETURN( HAPI_ImageInfo )
HAPI_ImageInfo_Create()
{
    HAPI_ImageInfo image_info;
    HAPI_ImageInfo_Init( &image_info );
    return image_info;
}

// ANIMATION ----------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_Keyframe_Init( HAPI_Keyframe * in )
{
    in->time = 0;
    in->value = 0;
    in->inTangent = 0;
    in->outTangent = 0;
}

HAPI_DECL_RETURN( HAPI_Keyframe )
HAPI_Keyframe_Create()
{
    HAPI_Keyframe info;
    HAPI_Keyframe_Init( &info );
    return info;
}

// VOLUMES ------------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_VolumeInfo_Init( HAPI_VolumeInfo * in )
{
    in->nameSH = 0;

    in->type = HAPI_VOLUMETYPE_INVALID;

    in->xLength = 0;
    in->yLength = 0;
    in->zLength = 0;
    in->minX = 0;
    in->minY = 0;
    in->minZ = 0;

    in->tupleSize = 0;
    in->storage = HAPI_STORAGETYPE_INVALID;

    in->tileSize = 0;

    in->hasTaper = false;
    in->xTaper = 0.0f;
    in->yTaper = 0.0f;
}

HAPI_DECL_RETURN( HAPI_VolumeInfo )
HAPI_VolumeInfo_Create()
{
    HAPI_VolumeInfo info;
    HAPI_VolumeInfo_Init( &info );
    return info;
}

HAPI_DECL_RETURN( void )
HAPI_VolumeTileInfo_Init( HAPI_VolumeTileInfo * in )
{
    in->minX = 0;
    in->minY = 0;
    in->minZ = 0;
    in->isValid = false;
}

HAPI_DECL_RETURN( HAPI_VolumeTileInfo )
HAPI_VolumeTileInfo_Create()
{
    HAPI_VolumeTileInfo info;
    HAPI_VolumeTileInfo_Init( &info );
    return info;
}

// CURVES -------------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_CurveInfo_Init( HAPI_CurveInfo * in )
{
    in->curveType = HAPI_CURVETYPE_LINEAR;
    in->curveCount = 0;
    in->vertexCount = 0;
    in->knotCount = 0;

    in->isPeriodic = false;
    in->isRational = false;
    in->isClosed = false;
    in->order = 2;

    in->hasKnots = false;
}

HAPI_DECL_RETURN( HAPI_CurveInfo )
HAPI_CurveInfo_Create()
{
    HAPI_CurveInfo info;
    HAPI_CurveInfo_Init( &info );
    return info;
}

// INPUT CURVES -------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_InputCurveInfo_Init( HAPI_InputCurveInfo * in )
{
    in->curveType = HAPI_CURVETYPE_LINEAR;
    in->closed = false;
    in->order = 2;
    in->reverse = false;
    in->inputMethod = HAPI_CURVEMETHOD_CVS;
    in->breakpointParameterization = HAPI_CURVEPARAMETERIZATION_UNIFORM;
}

HAPI_DECL_RETURN( HAPI_InputCurveInfo )
HAPI_InputCurveInfo_Create()
{
    HAPI_InputCurveInfo info;
    HAPI_InputCurveInfo_Init( &info );
    return info;
}

// TRANSFORMS ---------------------------

HAPI_DECL_RETURN( void )
HAPI_Transform_Init( HAPI_Transform * in )
{
    for ( int n = 0; n < HAPI_POSITION_VECTOR_SIZE; n++ )
        in->position[ n ] = 0.0f;

    for ( int n = 0; n < HAPI_QUATERNION_VECTOR_SIZE; n++ )
        in->rotationQuaternion[ n ] = 0.0f;

    for ( int n = 0; n < HAPI_SCALE_VECTOR_SIZE; n++ )
        in->scale[ n ] = 0.0f;

    for ( int n = 0; n < HAPI_SHEAR_VECTOR_SIZE; n++ )
        in->shear[ n ] = 0.0f;

    in->rstOrder = HAPI_RSTORDER_DEFAULT;
}

HAPI_DECL_RETURN( HAPI_Transform )
HAPI_Transform_Create()
{
    HAPI_Transform transform;
    HAPI_Transform_Init( &transform );
    return transform;
}

HAPI_DECL_RETURN( void )
HAPI_TransformEuler_Init( HAPI_TransformEuler * in )
{
    for ( int n = 0; n < HAPI_POSITION_VECTOR_SIZE; n++ )
        in->position[ n ] = 0.0f;

    for ( int n = 0; n < HAPI_EULER_VECTOR_SIZE; n++ )
        in->rotationEuler[ n ] = 0.0f;

    for ( int n = 0; n < HAPI_SCALE_VECTOR_SIZE; n++ )
        in->scale[ n ] = 0.0f;

    for ( int n = 0; n < HAPI_SHEAR_VECTOR_SIZE; n++ )
        in->shear[ n ] = 0.0f;

    in->rstOrder = HAPI_RSTORDER_DEFAULT;
    in->rotationOrder = HAPI_XYZORDER_DEFAULT;
}

HAPI_DECL_RETURN( HAPI_TransformEuler )
HAPI_TransformEuler_Create()
{
    HAPI_TransformEuler transformEuler;
    HAPI_TransformEuler_Init( &transformEuler );
    return transformEuler;
}

// SESSIONSYNC --------------------------------------------------------------

HAPI_DECL_RETURN( void )
HAPI_Viewport_Init( HAPI_Viewport * in )
{
    for ( int n = 0; n < HAPI_POSITION_VECTOR_SIZE; n++ )
	in->position[ n ] = 0.0f;

    for ( int n = 0; n < HAPI_QUATERNION_VECTOR_SIZE; n++ )
	in->rotationQuaternion[ n ] = 0.0f;

    in->offset = 0;
}

HAPI_DECL_RETURN( HAPI_Viewport )
HAPI_Viewport_Create()
{
    HAPI_Viewport viewport;
    HAPI_Viewport_Init( &viewport );
    return viewport;
}

HAPI_DECL_RETURN( void )
HAPI_SessionSyncInfo_Init( HAPI_SessionSyncInfo * in )
{
    in->cookUsingHoudiniTime = true;
    in->syncViewport = false;
}

HAPI_DECL_RETURN( HAPI_SessionSyncInfo )
HAPI_SessionSyncInfo_Create()
{
    HAPI_SessionSyncInfo syncInfo;
    HAPI_SessionSyncInfo_Init( &syncInfo );
    return syncInfo;
}
