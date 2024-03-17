//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template<class C>
Bool InputC ( OUT C &val, MDataBlock& dataBlock, const MObject &burgessMayaData ){
	MDataHandle customDataHandleIn = dataBlock.inputValue ( burgessMayaData );
	Burgess2MayaData* myData = ( BurgessMayaData* ) customDataHandleIn.asPluginData ();
	if ( myData == NULL ){
		return false;
	}
	// デシリアライズ
	try{
		cereal::BinaryInputArchive i_bin ( myData->data );
		i_bin ( val );
	} catch ( std::runtime_error e ){
		String t ( "!!!!!!!! Error !!!!!!!! Burgess2MayaData's class data type mismatch !!!!!!!!!!!!!" );
		MGlobal::displayInfo ( MString () + "[Burgess log message] ::::: " + t.c_str () );
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template<class C>
Bool OutputC ( OUT MObject &burgessMayaData, OUT MDataBlock& dataBlock, const C &val ){
	MStatus status;
	MDataHandle customDataHandleOut = dataBlock.outputValue ( burgessMayaData );
	//カスタムプラグインデータを作成
	MFnPluginData fnCustomData;
	fnCustomData.create ( Burgess2MayaData::id, &status );
	if ( status == MStatus::kFailure ){ return false; } // 作成に失敗した場合は終了
	//カスタムアトリビュートにセット
	Burgess2MayaData* myData = NULL;
	myData = ( Burgess2MayaData* ) fnCustomData.data ( &status );
	if ( status == MStatus::kFailure ){ return false; } // セットに失敗した場合は終了
	// シリアライズ
	cereal::BinaryOutputArchive o_bin ( myData->data );
	o_bin ( val );
	//データハンドルにセットする
	customDataHandleOut.set ( myData );
	return true;
}