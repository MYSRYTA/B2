#include "Converter.h"

Bool CreateBurgessCurveDataFromMayaCurve ( u8 &degree, u8 &curveForm, std::vector<Vec3> &curvePoints, std::vector<f32> &knots, const MFnNurbsCurve &mFnCurve ) {
	if ( mFnCurve.numCVs () <= 2 ) { return false; }

	/* �����̎擾 */
	degree = mFnCurve.degree ();

	/* Burgess�ł͎���=1~3�̂݃T�|�[�g */
	if ( 3 < degree ) { return false; }

	/* �J�[�u�̃t�H�[�����擾 */
	MFnNurbsCurve::Form form = mFnCurve.form ();
	if ( form == MFnNurbsCurve::kOpen ) {
		curveForm = 0;
	} else if ( form == MFnNurbsCurve::kClosed ) {
		curveForm = 1;
	} else if ( form == MFnNurbsCurve::kPeriodic ) {
		curveForm = 2;
	}

	/* MayaCurve�̃J�[�u�|�C���g�|�W�V����(���[�J���X�y�[�X)���擾 */
	MPointArray mayaPoints;
	mFnCurve.getCVs ( mayaPoints, MSpace::kObject );

	/* MayaCurve�̃m�b�g�x�N�g�����擾 */
	MDoubleArray mayaKnots;
	mFnCurve.getKnots ( mayaKnots );

	/* �|�W�V�����ƃm�b�g�̏���ϊ� */
	switch ( curveForm ) {
	case 0: /* Open */
		curvePoints = Convert ( mayaPoints );
		knots.push_back ( f32 ( mayaKnots [0] ) );
		for ( u32 i = 0; i < mayaKnots.length (); ++i ) {
			knots.push_back ( f32 ( mayaKnots [i] ) );
		}
		knots.push_back ( f32 ( mayaKnots [mayaKnots.length ()-1] ) );
		return true;
	case 1: /* Loop:Close ??????????????? */
		return false;
	case 2: /* Loop:Periodic */
		curvePoints = Convert ( mayaPoints );
		curvePoints.resize ( curvePoints.size () - degree );

		knots.push_back ( f32 ( mayaKnots [0] ) - 1.0f + f32 ( degree ) );
		for ( u32 i = 0; i < mayaKnots.length (); ++i ) {
			knots.push_back ( f32 ( mayaKnots [i] ) + f32 ( degree ) );
		}
		knots.push_back ( f32 ( mayaKnots [mayaKnots.length ()-1] ) + 1.0f + f32 ( degree ) );
		return true;
	}
	return false;
}

Bool CreateMayaCurveDataFromBurgessCurve ( u8 &mDegree, MFnNurbsCurve::Form &mForm, MPointArray &mayaPoints, MDoubleArray &mayaKnots, BSplineCurves &curves, u32 curveID ) {
	if ( curves.curveCount () == 0 || curves.curveCount () < curveID ) {
		return false;
	}

	mDegree = curves.getDegree ( curveID );

	mayaPoints = ConvertMPointArray ( curves.getAllPointPositions ( curveID ) );
	F32Attribute &refKnotAttr = curves.getF32Attribute ( curveID, "knotVector" );
	switch ( curves.getForm ( curveID ) ) {
	case 0: /* Open */
		mForm = MFnNurbsCurve::kOpen;
		for ( u32 i = 1; i < refKnotAttr.size () - 1; ++i ) {
			mayaKnots.append ( f64 ( refKnotAttr [i] ) );
		}
		return true;
	case 1: /* Close */
		return false;
	case 2: /* Periodic */
		mForm = MFnNurbsCurve::kPeriodic;
		for ( u32 i = 1; i < refKnotAttr.size () - 1; ++i ) {
			mayaKnots.append ( f64 ( refKnotAttr [i] ) - f64( mDegree ));
		}
		for ( u8 i = 0; i < mDegree; ++i ) {
			mayaPoints.append ( mayaPoints [i] );
		}
		return true;
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnNurbsCurve �� BSplineCurve
//!
//!		pointSize Maya �� BurgessCurve
//!			open : points.size �� points.size<br>
//!			close :  ??? �� points.size<br>
//!			periodic :  points.size + degree ��  points.size<br>
//!		knotVector Maya �� BurgessCurve
//!			open : points.size + degree - 1 �� points.size + degree +1<br>
//!			close :  ??? �� points.size + degree + 2<br>
//!			periodic : point.size + (degree * 2) - 1 / [0] = -degree + 1 �� point.size + (degree * 2) + 1 / [0] = 0
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Convert ( IO BSplineCurves &curves, const MFnNurbsCurve &mFnCurve ) {
	u8 degree;
	u8 curveForm;
	std::vector<Vec3> curvePoints;
	std::vector<f32> knots;
	Bool b = CreateBurgessCurveDataFromMayaCurve ( degree, curveForm, curvePoints, knots, mFnCurve );
	if ( b == false ) { return false; }

	/* BSplineCurves�ɒǉ� */
	u32 s = curves.curveCount ();
	curves.add ( curvePoints, degree, curveForm, knots );
	u32 c = curves.curveCount ();
	return s == c ? false : true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnNurbsCurve �� NurbsCurve (Weight�l�͑S��1.0)
//!
//!		point��knot�̕ϊ��Ɋւ������Create(BSpline�^�C�v)�̏ڍׂ��Q��
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Convert ( IO NURBSCurves &curves, const MFnNurbsCurve &mFnCurve ) {
	u8 degree;
	u8 curveForm;
	std::vector<Vec3> curvePoints;
	std::vector<f32> knots;
	Bool b = CreateBurgessCurveDataFromMayaCurve ( degree, curveForm, curvePoints, knots, mFnCurve );
	if ( b == false ) { return false; }

	/* NURBSCurves�ɒǉ� */
	std::vector<f32> weights ( curvePoints.size () );
	for ( u32 i = 0; i<curvePoints.size (); ++i ) {
		weights [i] = 1.0f;
	}
	u32 s = curves.curveCount ();
	curves.add ( curvePoints, degree, curveForm, knots, weights );
	u32 c = curves.curveCount ();
	return s == c ? false : true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnNurbsCurve��controlPoint��ʂ�CatmullRomCurves���쐬����
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Convert ( IO CatmullRomCurves &curves, const MFnNurbsCurve &mFnCurve ) {
	u8 degree;
	u8 curveForm;
	std::vector<Vec3> curvePoints;
	std::vector<f32> knots;
	Bool b = CreateBurgessCurveDataFromMayaCurve ( degree, curveForm, curvePoints, knots, mFnCurve );
	if ( b == false ) { return false; }

	/* CatmullRomCurves�ɒǉ� */
	u32 s = curves.curveCount ();
	curves.add ( curvePoints, curveForm );
	u32 c = curves.curveCount ();
	return s == c ? false : true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnNurbsCurve��controlPoint��ʂ�LinearCurves���쐬����
//------------------------------------------------------------------------------------------------------------------------------------------------
Bool Convert ( IO LinearCurves &curves, const MFnNurbsCurve &mFnCurve ) {
	u8 degree;
	u8 curveForm;
	std::vector<Vec3> curvePoints;
	std::vector<f32> knots;
	Bool b = CreateBurgessCurveDataFromMayaCurve ( degree, curveForm, curvePoints, knots, mFnCurve );
	if ( b == false ) { return false; }

	/* LinearCurves�ɒǉ� */
	u32 s = curves.curveCount ();
	curves.add ( curvePoints, curveForm );
	u32 c = curves.curveCount ();
	return s == c ? false : true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnMesh �� PolygonMesh
//------------------------------------------------------------------------------------------------------------------------------------------------
PolygonMesh Convert ( MFnMesh &mFnMesh, Bool allAttribute ) {
	PolygonMesh result;
	result.beginStructureChanges ();

	/* ���_ */
	MPointArray mPoints;
	mFnMesh.getPoints ( mPoints );
	result.resetPoints ( Convert ( mPoints ) );
	mPoints.clear ();

	/* �|���S�� */
	MIntArray mPolygonSizes, mPolygonPointIDs;
	mFnMesh.getVertices ( mPolygonSizes, mPolygonPointIDs );
	result.resetTopology ( Convert ( mPolygonSizes ), Convert ( mPolygonPointIDs ), false );
	mPolygonSizes.clear ();
	mPolygonPointIDs.clear ();

	if ( allAttribute == false ){ return result; }

	/* �@�� */
	MFloatVectorArray mNormals;
	mFnMesh.getNormals ( mNormals );
	MIntArray dummy, mNormalIDs;
	mFnMesh.getNormalIds ( dummy, mNormalIDs );
	dummy.clear ();
	MFloatVectorArray values ( mNormalIDs.length () );
	s32 c (mNormalIDs.length());
	#pragma omp parallel for if(1000 < c)
	for ( s32 i = 0; i<c; ++i ) {
		values [i] = mNormals [mNormalIDs [i]];
	}
	result.resetNormals ( Convert ( values ) );
	mNormals.clear ();
	mNormalIDs.clear ();
	values.clear ();
	
	result.endStructureChanges ();

	/* UV */
	if ( 0 < mFnMesh.numUVSets () ){
		MStringArray mUvNames;
		mFnMesh.getUVSetNames ( mUvNames );
		for ( u32 i = 0; i < mUvNames.length (); ++i ){
			MFloatArray u, v;
			mFnMesh.getUVs ( u, v, &mUvNames[i] );
			MIntArray dummy, uvVertexIDs;
			mFnMesh.getAssignedUVs ( dummy, uvVertexIDs, &mUvNames[i] );
			dummy.clear ();
			if ( 0 < uvVertexIDs.length () ){
				result.addUVMap ( Convert ( mUvNames[i] ) );
				std::vector<Vec2> uv ( u.length () );
				std::vector<u32> uvPackedPoiIDs ( uvVertexIDs.length () );
				s32 c1( u.length() );
				#pragma omp parallel for if(1000 < c1)
				for ( s32 k = 0; k<c1; ++k ){
					uv[k] = Vec2 ( u[k], v[k] );
				}
				s32 c2( uvVertexIDs.length() );
				#pragma omp parallel for if(1000 < c2)
				for ( s32 k = 0; k<c2; ++k ){
					uvPackedPoiIDs[k] = uvVertexIDs[k];
				}
				result.resetAllUVValues ( Convert ( mUvNames[i] ), uv, uvPackedPoiIDs );
			}
			u.clear ();
			v.clear ();
		}
	}

	/* Vertex Color / weight map */
	if ( 0 < mFnMesh.numColorSets () ){
		MStringArray mColorNames;
		mFnMesh.getColorSetNames ( mColorNames );
		for ( u32 i = 0; i < mColorNames.length (); ++i ){
			String mapName = Convert ( mColorNames[i] );
			MColorArray mColors;
			mFnMesh.getFaceVertexColors ( mColors, &mColorNames[i] );
			u32 channel = mFnMesh.getColorRepresentation ( mColorNames[i] ); // 1=Alpha, 3=RGB, 4=RGBA
			if( channel != 1 ){
				// vertex color�@�@Vec4Attr�ɃJ���[�l���i�[����
				if ( mColors.length () == result.vertexCount () ){
					// RGBA
					result.addVertexColorMap ( mapName, channel == 4 ); // Alpha�L�薳����Attr.v[0]�ɕۑ�
					result.resetAllVertexColors ( mapName, Convert ( mColors ) );
				}
			}else{
				// weight map
				if ( mColors.length () == result.vertexCount () ){
					s32 len = mColors.length ();
					result.addWeightMap ( mapName );
					std::vector<f32> wm ( len );
					#pragma omp parallel for
						for(s32 j=0; j<len; ++j ){
							wm[j] = f32(mColors[j].a);
						}
					result.resetAllWeightMapValues ( mapName, wm ); //Alpha�l�݂̂��E�F�C�g�}�b�v�Ɋi�[����@
				}
			}
			mColors.clear ();
		}
	}

	return result;
}
 
//------------------------------------------------------------------------------------------------------------------------------------------------
//!		MFnMesh �� PolygonMesh
//------------------------------------------------------------------------------------------------------------------------------------------------
PolygonMesh Convert ( MFnMesh& mFnMesh, const std::vector<u32>& faceMask, Bool allAttribute ) {
	PolygonMesh result;
	result.beginStructureChanges ();

	/* ���_ */
	MPointArray mPoints;
	mFnMesh.getPoints ( mPoints );
	result.resetPoints ( Convert ( mPoints ) );
	mPoints.clear ();

	/* �|���S�� */
	MIntArray mPolygonSizes, mPolygonPointIDs;
	mFnMesh.getVertices ( mPolygonSizes, mPolygonPointIDs );
	result.resetTopology ( Convert ( mPolygonSizes ), Convert ( mPolygonPointIDs ), false );
	mPolygonSizes.clear ();
	mPolygonPointIDs.clear ();

	if ( allAttribute == false ) { return result; }

	/* �@�� */
	MFloatVectorArray mNormals;
	mFnMesh.getNormals ( mNormals );
	MIntArray dummy, mNormalIDs;
	mFnMesh.getNormalIds ( dummy, mNormalIDs );
	dummy.clear ();
	MFloatVectorArray values ( mNormalIDs.length () );
	s32 c( mNormalIDs.length() );
	#pragma omp parallel for if(1000 < c)
	for ( s32 i = 0; i < c; ++i ) {
		values[i] = mNormals[mNormalIDs[i]];
	}
	result.resetNormals ( Convert ( values ) );
	mNormals.clear ();
	mNormalIDs.clear ();
	values.clear ();

	result.endStructureChanges ();

	/* UV */
	if ( 0 < mFnMesh.numUVSets () ) {
		MStringArray mUvNames;
		mFnMesh.getUVSetNames ( mUvNames );
		for ( u32 i = 0; i < mUvNames.length (); ++i ) {
			MFloatArray u, v;
			mFnMesh.getUVs ( u, v, &mUvNames[i] );
			MIntArray dummy, uvVertexIDs;
			mFnMesh.getAssignedUVs ( dummy, uvVertexIDs, &mUvNames[i] );
			dummy.clear ();
			if ( 0 < uvVertexIDs.length () ) {
				result.addUVMap ( Convert ( mUvNames[i] ) );
				std::vector<Vec2> uv ( u.length () );
				std::vector<u32> uvPackedPoiIDs ( uvVertexIDs.length () );
				s32 c1( u.length() );
				#pragma omp parallel for if(1000 < c1)
				for ( s32 k = 0; k < c1; ++k ) {
					uv[k] = Vec2 ( u[k], v[k] );
				}
				s32 c2( uvVertexIDs.length() );
				#pragma omp parallel for if(1000 < c2)
				for ( s32 k = 0; k < c2; ++k ) {
					uvPackedPoiIDs[k] = uvVertexIDs[k];
				}
				result.resetAllUVValues ( Convert ( mUvNames[i] ), uv, uvPackedPoiIDs );
			}
			u.clear ();
			v.clear ();
		}
	}

	/* Vertex Color / weight map */
	if ( 0 < mFnMesh.numColorSets () ) {
		MStringArray mColorNames;
		mFnMesh.getColorSetNames ( mColorNames );
		for ( u32 i = 0; i < mColorNames.length (); ++i ) {
			String mapName = Convert ( mColorNames[i] );
			MColorArray mColors;
			mFnMesh.getFaceVertexColors ( mColors, &mColorNames[i] );
			u32 channel = mFnMesh.getColorRepresentation ( mColorNames[i] ); // 1=Alpha, 3=RGB, 4=RGBA
			if ( channel != 1 ) {
				// vertex color�@�@Vec4Attr�ɃJ���[�l���i�[����
				if ( mColors.length () == result.vertexCount () ) {
					// RGBA
					result.addVertexColorMap ( mapName, channel == 4 ); // Alpha�L�薳����Attr.v[0]�ɕۑ�
					result.resetAllVertexColors ( mapName, Convert ( mColors ) );
				}
			} else {
				// weight map
				if ( mColors.length () == result.vertexCount () ) {
					s32 len = mColors.length ();
					result.addWeightMap ( mapName );
					std::vector<f32> wm ( len );
#pragma omp parallel for
					for ( s32 j = 0; j < len; ++j ) {
						wm[j] = f32 ( mColors[j].a );
					}
					result.resetAllWeightMapValues ( mapName, wm ); //Alpha�l�݂̂��E�F�C�g�}�b�v�Ɋi�[����@
				}
			}
			mColors.clear ();
		}
	}

	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		BSplineCurves �� MFnNurbsCurve
//------------------------------------------------------------------------------------------------------------------------------------------------
MObject Convert ( OUT MFnNurbsCurve &mFnCurve, BSplineCurves &curves, u32 curveID ) {
	MFnNurbsCurveData outCurveData;
	MObject outCurveObj = outCurveData.create ();
	u8 mDegree;
	MFnNurbsCurve::Form mForm;
	MPointArray mayaPoints;
	MDoubleArray mayaKnots;
	Bool b = CreateMayaCurveDataFromBurgessCurve ( mDegree, mForm, mayaPoints, mayaKnots, curves, curveID );
	if ( b == false ) { return outCurveObj; }
	/* MayaNurbs�J�[�u�f�[�^�̍\�z */
	mFnCurve.create (mayaPoints, mayaKnots, mDegree, mForm, false, /* 3D Curve */ false,/* isRational */ outCurveObj );
	return outCurveObj;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		NURBSCurve �� MFnNurbsCurve
//------------------------------------------------------------------------------------------------------------------------------------------------
MObject Convert ( OUT MFnNurbsCurve &mFnCurve, NURBSCurves &curves, u32 curveID ) {
	BSplineCurves* bs = static_cast<BSplineCurves*>( &curves );
	return Convert ( mFnCurve, *bs, curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		CatmullRomCurves �� MFnNurbsCurve
//------------------------------------------------------------------------------------------------------------------------------------------------
MObject Convert ( OUT MFnNurbsCurve &mFnCurve, CatmullRomCurves &curves, u32 curveID ) {
	BSplineCurves* bs = static_cast<BSplineCurves*>( &curves );
	return Convert ( mFnCurve, *bs, curveID );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		CatmullRomCurves �� MFnNurbsCurve
//------------------------------------------------------------------------------------------------------------------------------------------------
MObject Convert ( OUT MFnNurbsCurve &mFnCurve, LinearCurves &curves, u32 curveID ) {
	MFnNurbsCurveData outCurveData;
	MObject outCurveObj = outCurveData.create ();
	if ( curves.curveCount () == 0 || curves.curveCount () < curveID ) {
		return outCurveObj;
	}
	u8 mDegree = 1;
	MFnNurbsCurve::Form mForm = curves.getForm ( curveID ) == 0 ? MFnNurbsCurve::kOpen : MFnNurbsCurve::kPeriodic;
	MPointArray mayaPoints = ConvertMPointArray ( curves.getAllPointPositions ( curveID ) );
	if ( curves.getForm ( curveID ) != 0 ) {
		mayaPoints.append ( Convert(curves.getPointPosition ( curveID, 0 )) );
	}
	MDoubleArray mayaKnots( mayaPoints.length () );
	for ( u32 i = 0; i<mayaPoints.length (); ++i ) {
		mayaKnots [i] = f64 ( i );
	}

	mFnCurve.create ( mayaPoints, mayaKnots, mDegree, mForm, false, /* 3D Curve */ false,/* isRational */ outCurveObj );
	return outCurveObj;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		PolygonMesh �� MFnMesh 
//------------------------------------------------------------------------------------------------------------------------------------------------
MObject Convert ( OUT MFnMesh &mFnMesh, const PolygonMesh &mesh ){
	MFnMeshData outMeshData;
	MObject result = outMeshData.create ();
	if ( mesh.polygonCount () == 0 ) { return result; }

	/* ���_�ʒu */
	MPointArray mPoints = ConvertMPointArray ( mesh.getAllPointPositions() );
	/* �e�|���S���̒��_�����X�g */
	MIntArray mPolygonSizes = Convert ( mesh.getAllPolygonSizes ( false ) );
	/* �e�|���S�����\�����钸�_ID���X�g */
	U32Attribute &packedPoiIDsAttr = mesh.getU32Attribute ( "packedPointIDs" );
	MIntArray mPolygonPointIDs = Convert ( packedPoiIDsAttr.getValuesRef() );

	/* MFnMesh���쐬 */
	mFnMesh.create ( mPoints.length (), mPolygonSizes.length (), mPoints, mPolygonSizes, mPolygonPointIDs, result );
	mFnMesh.updateSurface ();

	/* �@�� */
	if ( mesh.hasNormal () ){
		u32 offset = 0;
		MVectorArray mNormals = ConvertMVectorArray ( mesh.getAllNormals () );
		MIntArray normalFace ( mPolygonPointIDs.length () ); // �V�A�g���r���[�gvertexFaceID���ł�����A������R���o�[�g���邾���ɂȂ�
		for ( u32 i = 0; i < mesh.polygonCount (); ++i ){
			for ( u32 j = 0; j < mesh.getPolygonSize ( i ); ++j ){
				normalFace[offset + j] = i;
			}
			offset += mesh.getPolygonSize ( i );
		}
		mFnMesh.setFaceVertexNormals ( mNormals, normalFace, mPolygonPointIDs );
	}

	/* UV */
	std::vector<String> uvNames = mesh.getAllUVNames ();
	for ( u32 i = 0; i < uvNames .size(); ++i ) {
		Vec2Attribute &uvAttr = mesh.getUVAttribute ( uvNames[i] );
		MFloatArray u ( uvAttr.size () );
		MFloatArray v ( uvAttr.size () );
		s32 c( uvAttr.size() );
		#pragma omp parallel for if(1000 < c)
		for ( s32 k = 0; k < c; k++ ) {
			u [k] = uvAttr [k].x();
			v [k] = uvAttr [k].y();
		}
		MString setName ( Convert( uvNames[i] ) );
		if ( uvNames [i] != "map1" ) {/* "map1"�̂݊��ɗ\��ς݂̂��ߐV�K�쐬���Ȃ� */
			mFnMesh.createUVSetDataMesh ( setName );/* MFnMesh.create() �̃��b�V���ɑ΂��ĕ���UV���Z�b�g���� �B���֐� */
		}
		mFnMesh.setUVs ( u, v, &setName );

		U32Attribute &uvPackedPoiIDsAttr = mesh.getPackedUVPointIDsAttribute ( uvNames [i] );
		s32 len = uvPackedPoiIDsAttr.size ();
		MIntArray uvIDs ( len );
		#pragma omp parallel for
			for ( s32 k = 0; k < len; k++ ) {
				uvIDs [k] = uvPackedPoiIDsAttr [k];
			}
		mFnMesh.assignUVs ( mPolygonSizes, uvIDs, &setName );
	}

	/* Vertex Color */
	std::vector<String> vcNames = mesh.getAllVertexColorNames ();
	for ( u32 i = 0; i < vcNames.size (); ++i ){
		MString mVCName = Convert ( vcNames[i] ); // �J���[�Z�b�g��
		Vec4Attribute &vcAttr = mesh.getVertexColorAttribute ( vcNames[i] ); // �o�[�e�b�N�X�J���[�l
		MFnMesh::MColorRepresentation comp = vcAttr.v[0] == 0 ? MFnMesh::MColorRepresentation::kRGB : MFnMesh::MColorRepresentation::kRGBA; // �A���t�@�L�薳��
		mFnMesh.createColorSetDataMesh ( mVCName, false, comp ); // MayaMesh�ɑ΂��ăJ���[�Z�b�g��V�K�쐬
		MColorArray mColors = Convert ( vcAttr.getValuesRef () );
		mFnMesh.setSomeColors ( mPolygonPointIDs, mColors, &mVCName );
		mFnMesh.assignColors ( mPolygonPointIDs, &mVCName );
	}

	/* weight map */
	std::vector<String> wmNames = mesh.getAllWeightMapNames ();
	for ( u32 i = 0; i < wmNames.size (); ++i ){
		MString mWMName = Convert ( wmNames[i] ); // �E�F�C�g�}�b�v��
		F32Attribute &wmAttr = mesh.getWeightMapAttribute ( wmNames[i] ); // �E�F�C�g�}�b�v�l
		MFnMesh::MColorRepresentation comp = MFnMesh::MColorRepresentation::kAlpha; // �A���t�@�̂�
		mFnMesh.createColorSetDataMesh ( mWMName, false, comp ); // MayaMesh�ɑ΂��ăJ���[�Z�b�g��V�K�쐬
		s32 len = wmAttr.size ();
		MColorArray mColors ( len );
		#pragma omp parallel for
			for ( s32 j = 0; j<len; ++j ){
				mColors[j].a = wmAttr[j]; // �E�F�C�g�}�b�v�l��MColor�̃A���t�@�`�����l���ɃR�s�[
			}
		mFnMesh.setSomeColors ( mPolygonPointIDs, mColors, &mWMName );
		mFnMesh.assignColors ( mPolygonPointIDs, &mWMName );
	}
	
	return result;
}