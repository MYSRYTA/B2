#pragma once
#include <burgess2_main/Geometry/Attributes/Attributes.h>
#include <burgess2_main/Geometry/Octree/Octree.h>
#include <burgess2_main/Geometry/SpatialQuery/PolygonMeshSpatialQuery.h>

//******************************************************************************
//! @file	PolygonMesh.h
//! @brief	 PolygonMesh
//******************************************************************************
/** @defgroup Geometry
*	   @{
*/
/**				@brief PolygonMesh <br>
*					#include <burgess2_main/Geometry.h><br>
*					#alias <br> \b : PolygonMesh = PolygonMeshT<f32> <br> \b : PolygonMeshd = PolygonMeshT<f64> <br>
*					 <br>
*					@par ��{�̃A�g���r���[�g - basic attributes
*						- <b> ���_�ʒu - point position </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "pointPositions"
*							- �z�� - array size = ���_�� - point count
*						- <b> �@�� - vertex normal </b>
*							- type = @link Vec3AttributeT<T> @endlink
*							- attribute name = "normals"
*							- �z�� - array size = �o�[�e�b�N�X�� - vertex count
*						- <b> �e�|���S�����\�����钸�_���̃��X�g�i�ҏW�֎~�j- List of point size that make up polygon. Do not edit. </b>
*							- type = @link U32AttributeT<T> @endlink
*							- attribute name = "polygonSizes"
*							- �z�� - array size = �|���S����+1 (+1��[0]=0�̂Ԃ�) - polygon count + 1
*							- ���� - note : 0�X�^�[�g�̉��Z�i�[�@���̗p - addition style
*							- �� - example :�@�|���S���̊p����"4,3,5,4"�̏ꍇ�ApolygonSizes = [0,4,7,12,16] �ƂȂ�
*						- <b> �e�|���S�����\�����钸�_ID�̃��X�g�i�ҏW�֎~�j- List of point IDs that make up polygon. Do not edit. </b>
*							- type = @link U32AttributeT<T> @endlink
*							- attribute name = "packedPointIDs"
*							- �z�� - array size = �o�[�e�b�N�X�� - vertex count
*							- ���� - note : polygonSizes�Ƒ΂ɂȂ�
*							- �� - example :�@�|���S������"4,3,5..."�̏ꍇ�ApackedPointIDs = [10,31,2,4, 2,14,0, 5,4,13,10,1,...]�ƂȂ�
*						- <b> ���_�ŋ��L����Ă���o�[�e�b�N�X��ID���X�g�i�ҏW�֎~�j- List of vertex IDs in the point. Do not edit. </b>
*							- type = @link L16ArrayU32AttributeT<T> @endlink
*							- attribute name = "sharedVertexIDs"<br>
*							- �z�� - array size = ���_�� - point count
*							- ���� - note : vertexID���i�[���鏇�Ԃ͍�����
*							- ���� - warning�@: �����̃|���S���A�C�����h��1�̒��_�����L���Ă���ꍇ�͍�����ŃA�C�����h���i�[����Ƃ͌���Ȃ�
*					@note ��{�̃A�g���r���[�g��PolygonMeshT<T>�̃C�j�V�����C�Y���Ɏ����I�ɍ쐬����܂� <br> Basic attributes are automatically created when PolygonMeshT<T> is initialized.
*						
*					<br>
*					@par �I�v�V���� �A�g���r���[�g - sub attributes
*						- <b> UV-map </b>
*							- type = @link Vec2AttributeT<T> @endlink
*							- attribute name = Maya uv-map name
*							- �z�� - array size = UV�o�[�e�b�N�X�� - uv vertex count
*							- �A�g���r���[�g�쐬�֐� - attribute creation function = addUVMap (const String &uvName)
*						- <b> �o�[�e�b�N�X�J���[ - vertex color </b>
*							- type = @link Vec4AttributeT<T> @endlink
*							- attribute name = Maya��RGB/RGBA�o�[�e�b�N�X�J���[�}�b�v�� - Maya vertex RGB/RGBA color map name
*							- �z�� - array size = �o�[�e�b�N�X�� - vertex count
*							- �A�g���r���[�g�쐬�֐� - attribute creation function = addVertexColorMap (const String &colorMapName, Bool hasAlpha=false)
*						- <b> �E�F�C�g�}�b�v - weight map </b>
*							- type = @link F32AttributeT<T> @endlink
*							- attribute name = Maya��A�`�����l�������̃o�[�e�b�N�X�J���[�}�b�v�� - Maya vertex color map name that only A channel
*							- �z�� - array size = �o�[�e�b�N�X�� - vertex count
*							- �A�g���r���[�g�쐬�֐� - attribute creation function =
*						- <b> �X�L���E�F�C�g - skin weight </b>
*							- type = @link SkinningAttributeT<T> @endlink
*							- attribute name = "weightList"
*							- �z�� - array size = ���_�� - point count
*							- �A�g���r���[�g�쐬�֐� - attribute creation function = addWeightMap (const String &weightMapName)
*						- <b> ���_�ɗאڂ��钸�_ID���X�g - List of point surrounding point IDs </b>
*							- type = @link L16ArrayU32AttributeT<T> @endlink
*							- attribute name = "surroundingPointIDs"
*							- �z�� - array size = ���_�� - point count
*							- �A�g���r���[�g�쐬�֐� - attribute creation function = getOrCreateSurroundingPointIDsAttribute (Bool includeAllPolygonPoints)
*					 <br>
*				
*				@code
*					PolygonMesh cube;
*					cube.createCube( Xfo(), 1.0f );
*					
*					Vec3 poiPos = cube.getPointPosition( 5 ); // 5�Ԃ̒��_�ʒu���擾���� - get point position of point-id-5
*					cube.setPointPosition( 3, poiPos ); // 3�Ԓ��_�̈ʒu��ݒ肷�� - set point position of point-id-3
*
*					Vec2 uvPos = cube.getUVValue("uv0", 2); // "uv0"��uv-map����2�Ԗڂ̃o�[�e�b�N�X��uv�l���擾���� - get uv value of vertex-id-2
*				@endcode
*				<br>
*				�J�X�^���A�g���r���[�g���쐬���Ēl���Z�b�g���� - create custom attribute and set value <br>
*				@code
*					PolygonMesh cube;
*					cube.createCube( Xfo(), 1.0f );
*
*					Vec2Attribute & newAttr = cube.getOrCreatVec2Attribute ( "new_name" );
*					newAttr.resize( cube.pointCount() );
*					newAttr[2] = Vec2( 0.1f, 0.5f );
*				@endcode
*				<br>
*				�g�|���W�[�f�[�^���烁�b�V����V�K�쐬���� - build new polygon mesh topology <br>
*				@code
*					std::vector<Vec3> pointPositionsVal = { Vec3(3.0,1.0,1.5)... }; // ���_�ʒu�̃��X�g
*					std::vector<u32> polygonSizesVal = { 0,4,7,12,16... }; // �e�|���S���̊p�����X�g ([0]=0)
*					std::vector<u32> packedPointIDsVal = { 10,31,2,4, 2,14,0, 5,4,13,10,1,... }; // �e�|���S�����\�����钸�_ID�̃��X�g
*					
*					PolygonMesh mesh; 
*					mesh2.beginStructureChanges ();
*						mesh.createPoints ( pointPositionsVal );
*						mesh.createPolygons ( polygonSizesVal, packedPointIDsVal, true );
*					mesh2.endStructureChanges ();
*				@endcode
*				<br>
*				�|���S����ǉ����� - add new polygon <br>
*				@code 
*					PolygonMesh mesh;
*					mesh2.beginStructureChanges ();
*						L8Array<Vec3> newPoints ( 3 );
*						newPoints[0] = Vec3 ( 0, 1, 0 );
*						newPoints[1] = Vec3 ( 2, 1, 0 );
*						newPoints[2] = Vec3 ( 1.5, 2, 0 );
*						mesh.addPolygon ( newPoints ); // �|�C���g�ƃ|���S����ǉ�
*						
*						L8Array<Vec3> newPoints2 ( 3 );
*						newPoints2[0] = Vec3 ( 0, 1, 0 );
*						newPoints2[1] = Vec3 ( 1.5, 0, -1 );
*						newPoints2[2] = Vec3 ( 2, 1, 0 );
*						mesh.addPolygon ( newPoints2, true ); // �|�C���g�ƃ|���S����ǉ��i�d���|�C���g�̓}�[�W�j
*					mesh.endStructureChanges ();
*					mesh.createPointNormals (); // �@����V�K�쐬
*				@endcode
*/

template<typename T>
class PolygonMeshT{
	AttributesT<T> attr; // �e��A�g���r���[�g
	AttributesT<T> uvs; // uv
	AttributesT<T> weightMaps; // �E�F�C�g�}�b�v�i=�P�F�̃o�[�e�b�N�X�J���[�}�b�v�j
	AttributesT<T> vertexColors; // �o�[�e�b�N�X�J���[
	Bool modelingMode = false;
	Bool deleteUnusedPoints = false;
public:
	PolygonMeshT ();
	~PolygonMeshT(){}

	PolygonMeshT<T> clone () const;
	PolygonMeshT<T> cloneTopology () const;

	void applyTransform ( const Mat4T<T> &transform );
	void applyTransform ( const XfoT<T> &transform );

	Bool sameTopology ( const PolygonMeshT<T> &other )const;

	Bool isPureTriangleMesh ()const;

	std::vector<GeometryLocationT<T>> generateLocationPerFace ( u32 detail );
	std::vector<GeometryLocationT<T>> generateLocationPerFace ( u32 detail, const String &vertexColorMaskName, Bool invertMask = false );

	/*! @name attribute*/
	/* @{ */
	BoolAttributeT<T>& getOrCreateBoolAttribute ( const String &_name );
	F32AttributeT<T>& getOrCreateF32Attribute ( const String &_name );
	F64AttributeT<T>& getOrCreateF64Attribute ( const String &_name );
	S32AttributeT<T>& getOrCreateS32Attribute ( const String &_name );
	U32AttributeT<T>& getOrCreateU32Attribute ( const String &_name );
	L16ArrayU32AttributeT<T>& getOrCreateL16ArrayU32Attribute ( const String &_name );
	Vec2AttributeT<T>& getOrCreatVec2Attribute ( const String &_name );
	Vec3AttributeT<T>& getOrCreatVec3Attribute ( const String &_name );
	Vec4AttributeT<T>& getOrCreatVec4Attribute ( const String &_name );
	SkinningAttributeT<T>& getOrCreateSkinningAttribute ( const String &_name = "weightList" );

	BoolAttributeT<T>& getBoolAttribute ( const String &_name ) const;
	F32AttributeT<T>& getF32Attribute ( const String &_name ) const;
	F64AttributeT<T>& getF64Attribute ( const String &_name ) const;
	S32AttributeT<T>& getS32Attribute ( const String &_name ) const;
	U32AttributeT<T>& getU32Attribute ( const String &_name ) const;
	L16ArrayU32AttributeT<T>& getL16ArrayU32Attribute ( const String &_name ) const;
	Vec2AttributeT<T>& getVec2Attribute ( const String &_name ) const;
	Vec3AttributeT<T>& getVec3Attribute ( const String &_name ) const;
	Vec4AttributeT<T>& getVec4Attribute ( const String &_name ) const;
	SkinningAttributeT<T>& getSkinningAttribute ( const String &_name = "weightList" ) const;

	void deleteAttribute(const String& _name);
	void clearAttribute(const String& _name);

	Bool hasAttribute ( const String &_name ) const;
	Bool hasSkinningAttribute ( const String &_name = "weightList" ) const;
	u8 getAttrType( const String& _name ) const;
	/* @{ */
	/*! @name unique attribute */
	/* @{ */
	L16ArrayU32AttributeT<T>& getOrCreateSurroundingPointIDsAttribute ( Bool includeAllPolygonPoints );
	L16ArrayU32AttributeT<T>& getSurroundingPointIDsAttribute ()const;
	L16ArrayU32AttributeT<T>& updateSurroundingPointIDsAttribute ()const;
	L16ArrayU32AttributeT<T>& recreateSurroundingPointIDsAttribute(Bool includeAllPolygonPoints)const;
	Vec3AttributeT<T>& getPointPositionAttribute () const;
	Vec3AttributeT<T>& getNormalAttribute () const;
	Vec2AttributeT<T>& getUVAttribute ( const String &_uvName ) const;
	F32AttributeT<T>& getWeightMapAttribute ( const String &_weightMapName ) const;
	Vec4AttributeT<T>& getVertexColorAttribute ( const String &_vertexColorMapName ) const;
	U32AttributeT<T>& getPackedUVPointIDsAttribute ( const String &_uvName ) const;

	/* @{ */
	/*! @name point*/
	/* @{ */
	inline u32 pointCount ()const;

	void setAllPointPositions ( const std::vector<Vec3T<T>> &positions );
	inline void setPointPosition ( u32 pointID,  const Vec3T<T> &position );

	std::vector<Vec3T<T>> getAllPointPositions ()const;
	inline Vec3T<T> getPointPosition ( u32 pointID )const;
	inline u32 getPointID ( u32 vertexID )const;
	inline L8Array<u32> getPolygonPointIDs ( u32 polygonID )const;
	inline L16Array<u32> getPointSurroundingPoints ( u32 pointID, Bool includeAllPolygonPoints )const;
	inline u32 getNextPolygonPointID ( u32 polygonID, u32 pointID )const;
	inline u32 getPreviousPolygonPointID ( u32 polygonID, u32 pointID )const;
	inline void getPointPolygonAndIndex ( u32 pointID, LocalIndex poiLocalID, u32 &polygonID, LocalIndex &polyLocalID )const;
	inline u32 getNextPointVertexID ( u32 vertexID )const;
	inline u32 getPreviousPointVertexID ( u32 vertexID )const;

	inline Bool isBorderPoint ( u32 pointID )const;

	/* @{ */
	/*! @name vertex*/
	/* @{ */
	inline u32 vertexCount ()const;

	inline L16Array<u32> getSharedVertexIDs ( u32 pointID )const;
	inline L8Array<u32> getPolygonVertexIDs ( u32 polygonID )const;
	inline u32 getVertexID ( u32 polygonID, u32 pointID )const;
	inline u32 getPolygonFirstVertexID ( u32 polygonID )const;
	inline u32 getNextPolygonVertexID ( u32 vertexID )const;
	inline u32 getPreviousPolygonVertexID ( u32 vertexID )const;

	inline Bool isPolygonFirstVertex ( u32 vertexID )const;
	inline Bool isPolygonLastVertex ( u32 vertexID )const;

	/* @{ */
	/*! @name polygon local vertex*/
	/* @{ */

	inline u32 getLocalVertexID(u32 polygonID, u32 pointID)const;
	inline u32 getNextLocalVertexID(u32 polygonID, u32 localVertexID)const;
	inline u32 getPreviousLocalVertexID(u32 polygonID, u32 localVertexID)const;

	/* @{ */
	/*! @name polygon*/
	/* @{ */
	inline u32 polygonCount ()const;

	std::vector<u32> getAllPolygonSizes ( Bool getAdditionStyle )const;
	inline u32 getPolygonSize ( u32 polygonID )const;
	inline L16Array<u32> getPointPolygonIDs( u32 pointID )const;
	inline u32 getPointPolygonCount ( u32 pointID )const;
	inline u32 getVertexPolygonID ( u32 vertexID )const;
	inline u32 getEdgeLeftPolygonID ( u32 edgeID )const;
	inline u32 getEdgeRightPolygonID ( u32 edgeID )const;
	inline u32 getPolygonPointAdjacentPolygonID ( u32 polygonID, u32 pointID )const;
	inline u32 getPolygonVertexAdjacentPolygonID ( u32 edgeID )const;
	inline u32 getPolygonPoint ( u32 polygonID, LocalIndex polylocalID )const;
	inline u32 getNextPolygonID ( u32 vertexID )const;
	inline u32 getPreviousPolygonID ( u32 vertexID )const;

	inline u8 triangleCount ( u32 polygonID )const;
	inline L4Array<u32> getTrianglePointIDs ( u32 polygonID, u8 triangleID )const;
	inline L4Array<u8> getTriangleIDs ( u32 polygonID, u32 pointID )const;
	inline L4Array<u8> getTriangleIDs ( u32 vertexID )const;

	inline Bool isPolygonBorder ( u32 polygonID, u32 pointID )const;

	/* @{ */
	/*! @name halfEdge*/
	/* @{ */
	inline u32 edgeCount ()const;
	inline u32 pointEdgeCount ( u32 pointID ) const;

	inline u32 getEdgeID ( u32 vertexID )const;
	inline u32 getEdgeStartVertexID ( u32 edgeID )const;
	inline Vec2u getEdgePointIDs ( u32 edgeID )const;
	inline Vec2u getEdgeVertexIDs ( u32 edgeID )const;
	inline u32 getEdgeIDFromStartEnd ( u32 startPointID, u32 endPointID )const;
	inline L16Array<u32> getPointEdgeIDs ( u32 pointID, Bool oneWay = false ) const;
	inline u32 getPolygonPointEdgeID ( u32 polygonID, u32 pointID )const;
	inline u32 getEdgeAdjacentEdgeID ( u32 edgeID )const;
	inline u32 getEdgeAdjacentEdgeID ( u32 polygonID, u32 pointID )const;

	inline Bool isPolygonBorder ( u32 edgeID )const;

	/* @{ */
	/*! @name meshEdge*/
	/* @{ */
	inline u32 meshEdgeCount ()const;
	inline u32 pointMeshEdgeCount ( u32 pointID ) const;

	/* @{ */
	/*! @name normal*/
	/* @{ */
	void setAllNormals ( const std::vector<Vec3T<T>> &_normals );
	void setAllPointNormals( const std::vector<Vec3T<T>> &_normals );
	void setNormal ( u32 vertexID, const Vec3T<T> &_normal );
	void setPointNormal ( u32 pointID, const Vec3T<T> _normal );

	std::vector<Vec3T<T>> getAllNormals ()const;
	inline Vec3T<T> getNormal ( u32 vertexID )const;
	inline L16Array<Vec3T<T>> getPointNormal ( u32 pointID )const;
	std::vector<Vec3T<T>> getAllPointAverageNormals ()const;
	inline Vec3T<T> getPointAverageNormal ( u32 pointID )const;
	inline Vec3T<T> getPolygonNormal ( u32 polygonID )const;

	void recomputePointNormals ();
	void createPointNormals ();

	inline Bool hasNormal ()const;

	/* @{ */
	/*! @name uv*/
	/* @{ */
	inline u8 uvCount ()const;
	inline u32 uvPointCount ( const String &uvName )const;

	Vec2AttributeT<T>& addUVMap ( const String &uvName );

	//void setAllUVValues ( const String &uvName, const std::vector<Vec2T<T>> &uv );
	void resetAllUVValues ( const String &uvName, const std::vector<Vec2T<T>> &uv, const std::vector<u32> &packedUVPointIDs );
	inline void setUVValue ( const String &uvName, u32 vertexID, const Vec2T<T> &uv );

	std::vector<Vec2T<T>> getAllUVValues ( const String &uvName )const;
	inline Vec2T<T> getUVValue ( const String &uvName, u32 vertexID )const;
	std::vector<String> getAllUVNames ()const;

	void deleteUV ( const String &uvName );
	void clearUV ( const String &uvName );

	inline Bool hasUVMap ( const String &uvName ) const;

	/* @{ */
	/*! @name vertex color*/
	/* @{ */
	u8 vertexColorMapCount ()const;

	Vec4AttributeT<T>& addVertexColorMap ( const String &colorMapName, Bool hasAlpha = false );

	//void setAllVertexColor ( const String &colorMapName, const std::vector<Vec4T<T>> &color );
	void resetAllVertexColors ( const String &colorMapName, const std::vector<Vec4T<T>> &color );
	void setVertexColor ( const String &colorMapName, u32 vertexID, const Vec4T<T> &color );

	std::vector<Vec4T<T>> getAllVertexColor ( const String &colorMapName )const;
	Vec4T<T> getVertexColor ( const String &colorMapName, u32 vertexID )const;
	std::vector<String> getAllVertexColorNames ()const;

	void deleteVertexColorMap ( const String &colorMapName );
	void clearVertexColorValues ( const String &colorMapName );

	Bool hasVertexColorMap ( const String &colorMapName ) const;
	Bool hasAlpha( const String &colorMapName ) const;

	/* @{ */
	/*! @name weight map*/
	/* @{ */
	u8 weightMapCount ()const;

	F32AttributeT<T>& addWeightMap ( const String &weightMapName );

	// void setAllWeightMapValues ( const String &weightMapName, const std::vector<f32> &val );
	void resetAllWeightMapValues ( const String &weightMapName, const std::vector<f32> &val );
	void setWeightMapValue ( const String &weightMapName, u32 vertexID, f32 val );

	std::vector<f32> getAllWeightMapValues ( const String &weightMapName )const;
	f32 getWeightMapValue ( const String &weightMapName, u32 vertexID )const;
	std::vector<String> getAllWeightMapNames ()const;

	void deleteWeightMap ( const String &weightMapName );
	void clearWeightMapValues ( const String &weightMapName );

	Bool hasWeightMap ( const String &weightMapName ) const;

	/* @{ */
	/*! @name modeling*/
	/* @{ */
private:
	void deletePolygonTask();
	void deleteUnusedPointsTask();
	void resetSharedPointAttr();
	void setSharedPointAttrDirty();
	u32 splitEdgeCore(
		IO std::vector<u32>& packedPointIDs,
		IO std::vector<u32>& polygonSizes,
		IO std::vector<Vec3T<T>>& addPoiPos,
		const std::vector<u32>& edgeIDs,
		const std::vector<f32>& ratios,
		const T ratio_tolerance = BURGESS2::TOLERANCE,
		const T pos_tolerance = BURGESS2::TOLERANCE ) const;
public:
	void rebuildTopology();
	void beginStructureChanges ();
	void endStructureChanges ();

	void resetPoints ( const std::vector<Vec3T<T>> &points );
	void createPoints ( const std::vector<Vec3T<T>> &newPoints );
	void addPoints ( const std::vector<Vec3T<T>> &newPoints );

	void resetTopology ( const std::vector<u32> &polygonSizes, const std::vector<u32> &packedPointIDs, Bool sourceIsAdditionStyle );
	void createPolygons ( const std::vector<u32> &polygonSizes, const std::vector<u32> &packedPointIDs, Bool sourceIsAdditionStyle );
	void addPolygons ( const std::vector<u32> &polygonSizes, const std::vector<u32> &packedPointIDs, Bool sourceIsAdditionStyle );
	u32 addPolygon ( const L8Array<u32> &pointIDs );
	u32 addPolygon ( u32 pointID1, u32 pointID2, u32 pointID3 );
	u32 addPolygon ( u32 pointID1, u32 pointID2, u32 pointID3, u32 pointID4 );
	u32 addPolygon ( const L8Array<Vec3T<T>> &newPoints, Bool merge = false, T tolerance = BURGESS2::TOLERANCE );

	void deletePolygon(u32 polygonID, Bool keepUnusedPoints = false );
	void resetNormals ( const std::vector<Vec3T<T>> &normals );

	void mergeMesh ( const PolygonMeshT<T> &source, Bool creatMaterialIDAttr = false );
	void mergeMeshes ( const std::vector<PolygonMeshT<T>> &source, Bool creatMaterialIDAttr = false );
	void mergeMeshCloneOnXfo ( const PolygonMeshT<T> &source, const std::vector<XfoT<T>> &xfo, Bool creatMaterialIDAttr = false );

	//void merge ( T tolerance = T ( 0.001 ) );
	//void mergePoint ( u32 firstPoint, u32 secondPointToDelete, T attrMergeRatio = T ( 0.0 ), f32 uvMergeTolerance = -1.0f );
	//void updateMergeAttribute ();

	u32 splitEdge ( u32 vertexID, f32 ratio );
	u32 splitEdge ( u32 polygonID, u32 pointID, f32 ratio );
	u32 splitEdges ( const std::vector<u32> &vertexIDs, const std::vector<f32> &ratios, const T tolerance = BURGESS2::TOLERANCE );
	u32 splitEdges ( const std::vector<u32> &polygonIDs, const std::vector<u32> &pointIDs, const std::vector<f32> &ratios, const T tolerance = BURGESS2::TOLERANCE );

	u32 splitPolygon ( u32 polygonID, u32 point1, u32 point2 );
	u32 splitPolygons ( const std::vector<u32> &polygonIDs, const std::vector<Vec2u> &points );

	u32 splitEdgesAndPolygon( 
		u32 polygonID, 
		LocalIndex poiLocalID1, 
		T ratio1, 
		LocalIndex poiLocalID2, 
		T ratio2, 
		const std::vector<Vec3T<T>>& addPointPositions = std::vector<Vec3T<T>>(),
		const T tolerance = BURGESS2::TOLERANCE );
	u32 splitEdgesAndPolygon( 
		u32 polygonID, 
		u32 pointID1, 
		T ratio1, 
		u32 pointID2, 
		T ratio2, 
		const std::vector<Vec3T<T>>& addPointPositions = std::vector<Vec3T<T>>(), 
		const T tolerance = BURGESS2::TOLERANCE );
	u32 splitEdgesAndPolygons (
		const std::vector<u32>& polygonIDs,
		const std::vector<u32>& pointIDs1,
		const std::vector<T>& ratio1,
		const std::vector<u32>& pointIDs2,
		const std::vector<T>& ratio2,
		const std::vector<std::vector<Vec3T<T>>>& addPointPositionsList,
		const T ratio_tolerance = BURGESS2::TOLERANCE,
		const T pos_tolerance = BURGESS2::TOLERANCE );

	PolygonMeshT<T> createSubdivideMesh ( u8 type, u8 iteration )const;

	PolygonMeshT<T> createPureTriangleMesh ()const;

	PolygonMeshT<T> createDualMesh ( T scale = T ( 1.0 ), const std::vector<u32> &mask = std::vector<u32> ( 0 ) ) const;
	PolygonMeshT<T> createDualMesh ( T scale, const std::vector<u32> &mask, OUT std::vector<u32> &dualFaceToSourcePoint )const;

	/* @{ */
	/*! @name spatial query*/
	/* @{ */
	//PolygonMeshSpatialQueryT<T> toSpatialQuery ()const;
	//PolygonMeshSpatialQueryT<T> toSpatialQuery ( const String &vertexColorMaskName, Bool invertMask = false )const;

	//void getLocationPolygonPointIndicesAndWeights( const GeometryLocationT<T>& location, OUT Vec3u& poiLocalID, OUT Vec3T<T>& weights )const;
	inline void getLocationPointsAndWeights( const GeometryLocationT<T>& location, OUT Vec3u& poiID, OUT Vec3T<T>& weights )const;
	inline Vec3T<T> getPositionAtLocation ( const GeometryLocationT<T> &location )const;
	inline Vec3T<T> getNormalAtLocation ( const GeometryLocationT<T> &location )const;
	inline Vec2T<T> getUVAtLocation ( const GeometryLocationT<T> &location, const String &uvName )const;
	inline Vec4T<T> getVertexColorAtLocation ( const GeometryLocationT<T> &location, const String &colorMapName )const;
	inline f32 getWeightMapValueAtLocation ( const GeometryLocationT<T> &location, const String &weightMapName )const;

	/* @{ */
	/*! @name primitive*/
	/* @{ */
	void createCube ( const XfoT<T> &xfo, T sideLength );

	/* @{ */
	/*! @name modify*/
	/* @{ */
	void smooth ( u8 iteration, T t = 0.5 );
	//void smooth ( u8 iteration, T t, Bool keepBorder = false, u8 type = u8(0) );
	/* @{ */

	/// @cond
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	template<class cerealArchive>
	void serialize ( cerealArchive &archive ){
		archive (
			CEREAL_NVP ( this->attr ),
			CEREAL_NVP ( this->uvs ),
			CEREAL_NVP ( this->weightMaps ),
			CEREAL_NVP ( this->vertexColors ),
			CEREAL_NVP ( this->modelingMode )
		);
	}
	/***************************************************************************************************** FileIO�p�̎d���� �h�L�������g�ɂ͍ڂ�Ȃ�*/
	/// @endcond
};

#include "PolygonMesh.inl"
#include "PolygonMesh_attribute.inl"
#include "PolygonMesh_point.inl"
#include "PolygonMesh_vertex.inl"
#include "PolygonMesh_normal.inl"
#include "PolygonMesh_polygon.inl"
#include "PolygonMesh_halfEdge.inl"
#include "PolygonMesh_modeling.inl"
#include "PolygonMesh_modeling2.inl"
#include "PolygonMesh_modeling3.inl"
#include "PolygonMesh_uv.inl"
#include "PolygonMesh_location.inl"
#include "PolygonMesh_prim.inl"
#include "PolygonMesh_vertexColor.inl"
#include "PolygonMesh_weightMap.inl"
#include "PolygonMesh_modify.inl"

/** @} */

using PolygonMesh = PolygonMeshT<f32>;
using PolygonMeshd = PolygonMeshT<f64>;

//******************************************************************************
//! End of File
//******************************************************************************