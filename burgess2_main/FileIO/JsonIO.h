#pragma once
#include <burgess2_main/Core/base.h>
#include <cereal/archives/json.hpp>

//******************************************************************************
//! @file	JsonIO.h
//! @brief	 JSONファイルの書き込み、読み込み
//******************************************************************************
/** @defgroup FileIO
*　@{
*/
/**		@brief	クラスデータをJSONファイルへ書き込み、読み込み <br>
*			#include <burgess2_main/FileIO.h> <br>
*
*			@code
* 　　　　　　　　// Burgess2以外のストラクトやクラスをFileIOで扱いたい場合は、定義にcerealのセットアップを記述する
*				// メンバ変数をそのまま扱うのであればserialize()でOK
*				// 保存データの準備やロード時の処理、保存データ名を個別で設定したい場合はsave()/load()関数を設定（VecT<T>のソースコードを参照）
*				struct ClassAAA{
*					Xfo xfo;
*					Vec3 v3;
*								
*					// cereal setup 
*					template<class cerealArchive>
*					void serialize ( cerealArchive &archive ){
*						archive (
*							CEREAL_NVP ( xfo ),
*							CEREAL_NVP ( v3 )
*						);
*					}
*				};
*				
*				ClassAAA val;
*				val.xfo = ....;
*				val.v3 = ....;
*
*				// 保存先を登録する
*				String path = "D:/temp/file.json";
*				JsonIO <ClassAAA> jsonIO ( path );
*
*				// 書き込み
*				jsonIO.setWritable (); // 書き込みモードにする
*				jsonIO.write ( val ); // データの書き込み
*				
*				// JSONのIO処理の終了を宣言する
*				jsonIO.close();
*				
*				///////////////////////////////////////////////////
*				
*				// ロード先を登録する
*				String path2 = "D:/temp/file.json";
*				JsonIO <ClassAAA> jsonIO2 ( path2 );
*				//　読み込み
*				ClassAAA val2; // 読み込み先のデータを準備
*				jsonIO2.setReadable (); // 読み込みモードにする
*				jsonIO2.read ( val2 ); // JSONファイルからデータをロードする
*
*				// JSONのIO処理の終了を宣言する
*				jsonIO2.close();
*			@endcode
*/
template<class C>
class JsonIO{
	String path;
	u8 mode = u8 ( 0 ); // 0=無効, 1=書き込み, 2=読み込み
public:
	std::ifstream fin;
	std::ofstream fout;

	JsonIO (){};
	JsonIO ( const String &path );
	virtual  ~JsonIO (){}

	void setPath ( const String &path );
	Bool setWritable ();
	Bool setReadable ();
	void close ();

	void write ( const C &val );
	void read ( OUT C &val );
};

#include "JsonIO.inl"
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************