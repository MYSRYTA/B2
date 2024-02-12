#pragma once
#include <burgess2_main/Core/base.h>
#include <cereal/archives/xml.hpp>

//******************************************************************************
//! @file	XmlIO.h
//! @brief	 xmlファイルの書き込み、読み込み
//******************************************************************************
/** @defgroup FileIO
*　@{
*/
/**		@brief	クラスデータをxmlファイルへ書き込み、読み込み<br>
*			#include <burgess2_main/FileIO.h> <br>
*
*			@code
* 　　　　　　　　// Burgess2以外のストラクトやクラスをFileIOで扱いたい場合は、定義にcerealのセットアップを記述する
*				// メンバ変数をそのまま扱うのであればserialize()でOK（BinaryIO.hのサンプルコードを参照）
*				// 保存データの準備やロード時の処理、保存データ名を個別で設定したい場合はsave()/load()関数を設定（VecT<T>のソースコードを参照）
*				Xfo val;
*				val.tr = ....;
*				val.ori = ....;
*
*				// 保存先を登録する
*				String path = "D:/temp/file.xml";
*				XmlIO <Xfo> xmlIO ( path );
*
*				// 書き込み
*				xmlIO.setWritable (); // 書き込みモードにする
*				xmlIO.write ( val ); // データの書き込み
*
*				// XmlのIO処理の終了を宣言する
*				xmlIO.close();
*
*				///////////////////////////////////////////////////
*
*				// ロード先を登録する
*				String path2 = "D:/temp/file.xml";
*				XmlIO <Xfo> xmlIO2 ( path2 );
*				//　読み込み
*				Xfo val2; // 読み込み先のデータを準備
*				xmlIO2.setReadable (); // 読み込みモードにする
*				xmlIO2.read ( val2 ); // Xmlファイルからデータをロードする
*
*				// JSONのIO処理の終了を宣言する
*				xmlIO2.close();
*			@endcode
*/
template<class C>
class XmlIO{
	String path;
	u8 mode = u8 ( 0 ); // 0=無効, 1=書き込み, 2=読み込み
public:
	std::ifstream fin;
	std::ofstream fout;

	XmlIO (){};
	XmlIO ( const String &path );
	virtual  ~XmlIO (){}

	void setPath ( const String &path );
	Bool setWritable ();
	Bool setReadable ();
	void close ();

	void write ( const C &val );
	void read ( OUT C &val );
};

#include "XmlIO.inl"
/** @} */

//******************************************************************************
//! End of File
//******************************************************************************