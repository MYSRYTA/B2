#include "CustomLocator.h"

// ロケータサイズやカラーなどのジオメトリに関するアトリビュートを取得し、メンバー変数に格納する
void TKCMCustomLocatorGeoOverride::updateDG ()
{
	// アイコン
	int inIconType;
	MPlug iconP ( mLocatorNode, TKCMCustomLocator::icon );
	if (!iconP.isNull ()){ iconP.getValue ( inIconType ); }

	// カラー設定
	Vec3 inColor0, inColor1;
	MPlug col0P ( mLocatorNode, TKCMCustomLocator::color0 );
	if (!col0P.isNull ()){
		for (u32 i = 0; i < 3; i++){ inColor0[i] = float ( col0P.child ( i ).asDouble () ); }
	}
	MPlug col1P ( mLocatorNode, TKCMCustomLocator::color1 );
	if (!col1P.isNull ()){
		for (u32 i = 0; i < 3; i++){ inColor1[i] = float ( col1P.child ( i ).asDouble () ); }
	}

	// オフセットトランスフォーム設定
	Vec3 inTr, inSc;
	Rad3 inRot;
	MPlug scP ( mLocatorNode, TKCMCustomLocator::size );
	if (!scP.isNull ()){ for (u32 i = 0; i < 3; i++){ inSc[i] = float ( scP.child ( i ).asDouble () ); } }
	MPlug trP ( mLocatorNode, TKCMCustomLocator::offsetTr );
	if (!trP.isNull ()){ for (u32 i = 0; i < 3; i++){ inTr[i] = float ( trP.child ( i ).asDouble () ); } }
	MPlug rotP ( mLocatorNode, TKCMCustomLocator::offsetRot );
	if (!rotP.isNull ()){ for (u32 i = 0; i < 3; i++){ inRot[i] = float ( rotP.child ( i ).asDouble () ); }	}

	Bool inWire, inFace;
	MPlug wP ( mLocatorNode, TKCMCustomLocator::wire );
	if (!wP.isNull ()){ wP.getValue ( inWire ); }
	MPlug fP ( mLocatorNode, TKCMCustomLocator::face );
	if (!fP.isNull ()){ fP.getValue ( inFace ); }
	
	if( this->iconType != inIconType ||
		this->wireDraw != inWire ||
		this->faceDraw != inFace ||
		this->color0.almostEqual( inColor0 ) == false || 
		this->color1.almostEqual (inColor1 ) == false ||
		this->iconScale.almostEqual ( inSc ) == false ||
		this->offsetTr.almostEqual ( inTr ) == false ||
		this->offsetRot.almostEqual ( inRot ) == false )
	{
		this->iconScale = inSc;
		this->iconType = inIconType;
		this->color0 = inColor0;
		this->color1 = inColor1;
		this->offsetTr = inTr;
		this->offsetRot = inRot;
		this->wireDraw = inWire;
		this->faceDraw = inFace;
		this->changed = true;
	}
}

// レンダーアイテムの追加とシェーダーのアサイン
void TKCMCustomLocatorGeoOverride::updateRenderItems ( const MDagPath& path, MHWRender::MRenderItemList& list )
{
	// シェーダー作成
	MHWRender::MShaderInstance* shaderWire;
	MHWRender::MShaderInstance* shaderFace;
	switch (MHWRender::MGeometryUtilities::displayStatus ( path )){
		case MHWRender::kLead:
		case MHWRender::kActive:
		case MHWRender::kHilite:
		case MHWRender::kActiveComponent:
			shaderWire = get3dSolidShader ( MColor{ this->color1.x, this->color1.y, this->color1.z, 1.0f } );
			shaderFace = get3dSolidShader ( MColor{ this->color1.x, this->color1.y, this->color1.z, 0.1f } );
			break;
		default:
			shaderWire = get3dSolidShader ( MColor{this->color0.x, this->color0.y, this->color0.z, 1.0f } );
			shaderFace = get3dSolidShader ( MColor{this->color0.x, this->color0.y, this->color0.z, 0.1f } );
			break;
	}
	if ( !shaderWire || !shaderFace ) return;
	//////////////////////////// 描画オブジェクトを作成する ////////////////////////////
	////* ワイヤーフレームオブジェクト */
	MHWRender::MRenderItem* wireframeItem = NULL;
	int index = list.indexOf ( "TKCMCustomLocatorWires" );
	if (index < 0){
		// 描画アイテムが未登録の場合はライン型のジオメトリを新規登録する
		wireframeItem = MHWRender::MRenderItem::Create ( "TKCMCustomLocatorWires", MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kLines );
		wireframeItem->setDrawMode ( MHWRender::MGeometry::kAll );
		wireframeItem->depthPriority ( MRenderItem::sDormantFilledDepthPriority );
		list.append ( wireframeItem );
	}else{
		wireframeItem = list.itemAt ( index );
	}
	// 描画アイテムにシェーダー等をアサインして描画を有効にする
	if (wireframeItem){
		wireframeItem->setShader ( shaderWire );
		wireframeItem->enable ( this->wireDraw ); // 描画のON/OFF
	}

	////* フェースオブジェクト */
	MHWRender::MRenderItem* shadedItem = NULL;
	index = list.indexOf ( "TKCMCustomLocatorTriangles" );
	if (index < 0) {
		// 描画アイテムが未登録の場合はフェース型のジオメトリを新規登録する
		shadedItem = MHWRender::MRenderItem::Create ("TKCMCustomLocatorTriangles", MHWRender::MRenderItem::DecorationItem, MHWRender::MGeometry::kTriangles );
		shadedItem->setDrawMode ( MHWRender::MGeometry::kAll );
		shadedItem->depthPriority ( MRenderItem::sActivePointDepthPriority );
		list.append ( shadedItem );
	}else{
		shadedItem = list.itemAt ( index );
	}
	// 描画アイテムにシェーダー等をアサインして描画を有効にする
	if (shadedItem){
		shadedItem->setShader ( shaderFace );
		shadedItem->enable ( this->faceDraw ); // 描画のON/OFF
	}
}

// 要求されたジオメトリ情報（IndexBuffer や VertexBuffer）を更新
void TKCMCustomLocatorGeoOverride::populateGeometry (
	const MHWRender::MGeometryRequirements& requirements,
	const MHWRender::MRenderItemList& renderItems,
	MHWRender::MGeometry& data )
{
	MHWRender::MVertexBuffer* verticesBuffer = NULL;
	float* vertices = NULL;

	const MHWRender::MVertexBufferDescriptorList& vertexBufferDescriptorList = requirements.vertexRequirements ();
	const int numberOfVertexRequirments = vertexBufferDescriptorList.length ();
	
	int poiCount;
	switch (this->iconType){
		case 0:	poiCount = linePoint.size (); break;
		case 1:	poiCount = rectPoint.size (); break;
		case 2: poiCount = circlePoint.size (); break;
		case 3: poiCount = circlePoint2.size (); break;
		case 4: poiCount = circleHalfPoint.size (); break;
		case 5: poiCount = rectArrowPoint.size (); break;
		case 6: poiCount = trianglePoint.size (); break;
		case 7: break;
		case 8: break;
		case 9: break;
		case 10: poiCount = boxPoint.size (); break;
		case 11: poiCount = slopePoint.size (); break;
		case 12: poiCount = trapezoidPoint.size (); break;
		case 13: poiCount = pyramidPoint.size (); break;
		case 14: poiCount = diamondPoint.size (); break;
		case 15: poiCount = ringPoint.size (); break;
		case 16: poiCount = gravityPoint.size (); break;
		case 17: poiCount = windPoint.size (); break;
		case 18: poiCount = solverPoint.size (); break;
		default: poiCount = rectPoint.size ();
	}

	MHWRender::MVertexBufferDescriptor vertexBufferDescriptor;
	for (int requirmentNumber = 0; requirmentNumber < numberOfVertexRequirments; ++requirmentNumber)
	{
		if (!vertexBufferDescriptorList.getDescriptor ( requirmentNumber, vertexBufferDescriptor ) ) { continue; }

		switch (vertexBufferDescriptor.semantic ())	{
			case MHWRender::MGeometry::kPosition:
				if (!verticesBuffer) {
					verticesBuffer = data.createVertexBuffer ( vertexBufferDescriptor );
					if (verticesBuffer)
					{
						vertices = (float*)verticesBuffer->acquire ( poiCount );
					}
				}
				break;
			default:
				break;
		}
	}

	// 頂点を登録
	int verticesPointerOffset = 0;
	for (int currentVertex = 0; currentVertex < poiCount; ++currentVertex)
	{
		if (vertices)
		{
			Vec3 poiPos;
			switch (this->iconType){
				case 0:	poiPos = linePoint [currentVertex]; break;
				case 1:	poiPos = rectPoint[currentVertex]; break;
				case 2: poiPos = circlePoint[currentVertex]; break;
				case 3: poiPos = circlePoint2[currentVertex]; break;
				case 4: poiPos = circleHalfPoint [currentVertex]; break;
				case 5: poiPos = rectArrowPoint [currentVertex]; break;
				case 6: poiPos = trianglePoint [currentVertex]; break;
				case 7: break;
				case 8: break;
				case 9: break;
				case 10: poiPos = boxPoint[currentVertex]; break;
				case 11: poiPos = slopePoint [currentVertex]; break;
				case 12: poiPos = trapezoidPoint [currentVertex]; break;
				case 13: poiPos = pyramidPoint [currentVertex]; break;
				case 14: poiPos = diamondPoint [currentVertex]; break;
				case 15: poiPos = ringPoint [currentVertex]; break;
				case 16: poiPos = gravityPoint[currentVertex]; break;
				case 17: poiPos = windPoint[currentVertex]; break;
				case 18: poiPos = solverPoint[currentVertex]; break;
				default: poiPos = rectPoint[currentVertex];
			}
			Xfo offset ( this->iconScale, Quat(this->offsetRot), this->offsetTr);
			poiPos = offset * poiPos;
			vertices[verticesPointerOffset++] = poiPos.x;
			vertices[verticesPointerOffset++] = poiPos.y;
			vertices[verticesPointerOffset++] = poiPos.z;
		}
	}

	if (verticesBuffer && vertices){ verticesBuffer->commit ( vertices ); }
	// 描画要素をループする（現状はワイヤーとフェース描画の計2回）
	for (int i = 0; i < renderItems.length (); ++i)	{
		const MHWRender::MRenderItem* item = renderItems.itemAt ( i ); // 描画要素を取得
		if (!item ){ continue; } // 描画要素が無ければ終了

		MHWRender::MIndexBuffer* indexBuffer = data.createIndexBuffer ( MHWRender::MGeometry::kUnsignedInt32 );
		// ワイヤー描画で各ラインを構成するポイントIDを登録する
		if (item->name () == "TKCMCustomLocatorWires" && this->wireDraw) {
			std::vector<u32> wirePointIDs;
			switch (this->iconType){
				case 0:	wirePointIDs = lineWirePointIDs; break;
				case 1:	wirePointIDs = rectWirePointIDs; break;
				case 2: wirePointIDs = circleWirePointIDs; break;
				case 3: wirePointIDs = circle2WirePointIDs; break;
				case 4: wirePointIDs = circleHalfWirePointIDs; break;
				case 5: wirePointIDs = rectArrowWirePointIDs; break;
				case 6: wirePointIDs = triangleWirePointIDs; break;
				case 7: break;
				case 8: break;
				case 9: break;
				case 10: wirePointIDs = boxWirePointIDs; break;
				case 11: wirePointIDs = slopeWirePointIDs; break;
				case 12: wirePointIDs = trapezoidWirePointIDs; break;
				case 13: wirePointIDs = pyramidWirePointIDs; break;
				case 14: wirePointIDs = diamondWirePointIDs; break;
				case 15: wirePointIDs = ringWirePointIDs; break;
				case 16: wirePointIDs = gravityWirePointIDs; break;
				case 17: wirePointIDs = windWirePointIDs; break;
				case 18: wirePointIDs = solverPointIDs; break;
				default: wirePointIDs = rectWirePointIDs;
			}
			unsigned int* indices = (unsigned int*)indexBuffer->acquire ( wirePointIDs.size () );
			for (u32 i = 0, n = wirePointIDs.size (); i < n; i++){
				indices[i] = wirePointIDs[i];
			}
			indexBuffer->commit ( indices );
			
		}
		// フェース描画で各トライアングルフェースを構成するポイントIDを登録する
		else if (item->name () == "TKCMCustomLocatorTriangles" && this->faceDraw){
			std::vector<u32> facePointIDs;
			switch (this->iconType){
				case 0:	facePointIDs = lineFacePointIDs; break;
				case 1:	facePointIDs = rectFacePointIDs; break;
				case 2: facePointIDs = circleFacePointIDs; break;
				case 3: facePointIDs = circle2FacePointIDs; break;
				case 4: facePointIDs = circleHalfFacePointIDs; break;
				case 5: facePointIDs = rectArrowFacePointIDs; break;
				case 6: facePointIDs = triangleFacePointIDs; break;
				case 7: break;
				case 8: break;
				case 9: break;
				case 10: facePointIDs = boxFacePointIDs; break;
				case 11: facePointIDs = slopeFacePointIDs; break;
				case 12: facePointIDs = trapezoidFacePointIDs; break;
				case 13: facePointIDs = pyramidFacePointIDs; break;
				case 14: facePointIDs = diamondFacePointIDs; break;
				case 15: facePointIDs = ringFacePointIDs; break;
				case 16: facePointIDs = gravityFacePointIDs; break;
				case 17: facePointIDs = windFacePointIDs; break;
				case 18: facePointIDs = solverFacePointIDs; break;
				default: facePointIDs = rectFacePointIDs;
			}
			unsigned int* indices = (unsigned int*) indexBuffer->acquire ( facePointIDs.size () );
			for (u32 i = 0, n = facePointIDs.size (); i < n; i++){
				indices[i] = facePointIDs[i];
			}
			indexBuffer->commit ( indices );
		}
		
		item->associateWithIndexBuffer ( indexBuffer );
	}

	this->changed = false;
}
