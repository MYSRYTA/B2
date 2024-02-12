//#include "LogMessage.h"

#define BurgessLogMessageHeader String t = title; if(title.length() !=0){t+=" : ";} MGlobal::displayInfo ( MString () + "[Burgess2 log message] ::::: " + t.c_str()
#define BurgessLogMessageHeaderArray MGlobal::displayInfo ( MString () + "                    "

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template<class ClassT>
void report ( const ClassT data, const String title) {
	BurgessLogMessageHeader  + data.toString ().c_str() );
}

template<>
void report ( const String str, const String title ) {
	BurgessLogMessageHeader  + "String:" + str.c_str () );
}

template<>
void report ( const unsigned char data, const String title ) {
	BurgessLogMessageHeader  + "u_char:" + std::to_string(data).c_str () );
}

template<>
void report ( const unsigned short data, const String title ) {
	BurgessLogMessageHeader  + "u_short:" + std::to_string ( data ).c_str () );
}

template<>
void report ( const unsigned int data, const String title ) {
	BurgessLogMessageHeader  + "u_int:" + std::to_string ( data ).c_str () );
}

template<>
void report ( const char data, const String title ) {
	BurgessLogMessageHeader  + "char:" + std::to_string ( data ).c_str () );
}

template<>
void report ( const short data, const String title ) {
	BurgessLogMessageHeader + "short:" + std::to_string ( data ).c_str () );
}

template<>
void report ( const int data, const String title ) {
	BurgessLogMessageHeader + "int:" + std::to_string ( data ).c_str () );
}

template<>
void report ( const float data, const String title ) {
	BurgessLogMessageHeader + "float:" + GetFloorString<f32>(data).c_str() );
}

template<>
void report ( const double data, const String title ) {
	BurgessLogMessageHeader + "double:" + GetFloorString<f64>(data).c_str() );
}

template<>
void report ( const size_t data, const String title ) {
	BurgessLogMessageHeader + "size_t:" + std::to_string ( data ).c_str () );
}

template<>
void report ( const Bool data, const String title ) {
	if ( data ) {
		BurgessLogMessageHeader + "bool:true" );
	} else {
		BurgessLogMessageHeader + "bool:false" );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template<class ClassT>
void report ( const std::vector<ClassT> data, const String title, u32 size ) {
	BurgessLogMessageHeader  + "Array Size = " + std::to_string(data.size()).c_str () );
	u8 count = u32 ( data.size () ) < size ? data.size () : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + data[i].toString ( "[" + std::to_string ( i ) + "]" ).c_str () );
	}
	if ( size <= data.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( data.size ()-1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<Bool> str, const String title, u32 size ){
	BurgessLogMessageHeader + "String : Array Size = " + std::to_string ( str.size () ).c_str () );
	u32 count = u32 ( str.size () ) < size ? u32 ( str.size () ) : size;
	for ( u8 i = 0; i < count; i++ ){
		if ( str[i] ){
			BurgessLogMessageHeaderArray + "true" );
		} else{
			BurgessLogMessageHeaderArray + "false" );
		}
	}
	if ( size <= str.size () ){
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( str.size () - 1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<String> str, const String title, u32 size ) {
	BurgessLogMessageHeader  + "String : Array Size = " + std::to_string ( str.size () ).c_str () );
	u32 count = u32(str.size ()) < size ? u32(str.size ()) : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + str [i].c_str () );
	}
	if ( size <= str.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( str.size ()-1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<unsigned char> data, const String title, u32 size ) {
	BurgessLogMessageHeader  + "u8 : Array Size = " + std::to_string ( data.size () ).c_str () );
	u32 count = u32 ( data.size () ) < size ? u32(data.size ()) : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + "[" + std::to_string ( i ).c_str () + "] " + std::to_string( data [i] ).c_str () );
	}
	if ( size <= data.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( data.size ()-1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<unsigned short> data, const String title, u32 size ) {
	BurgessLogMessageHeader  + "u16 : Array Size = " + std::to_string ( data.size () ).c_str () );
	u32 count = u32(data.size ()) < size ? u32(data.size ()) : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + "[" + std::to_string ( i ).c_str () + "] " + std::to_string ( data [i] ).c_str () );
	}
	if ( size <= data.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( data.size ()-1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<unsigned int> data, const String title, u32 size ) {
	BurgessLogMessageHeader  + "u32 : Array Size = " + std::to_string ( data.size () ).c_str () );
	u32 count = u32 ( data.size () ) < size ? u32(data.size ()) : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + "[" + std::to_string ( i ).c_str () + "] " + std::to_string ( data [i] ).c_str () );
	}
	if ( size <= data.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( data.size ()-1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<char> data, const String title, u32 size ) {
	BurgessLogMessageHeader  + "s8 : Array Size = " + std::to_string ( data.size () ).c_str () );
	u32 count = u32 ( data.size () ) < size ? u32(data.size ()) : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + "[" + std::to_string ( i ).c_str () + "] " + std::to_string ( data [i] ).c_str () );
	}
	if ( size <= data.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( data.size ()-1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<short> data, const String title, u32 size ) {
	BurgessLogMessageHeader  + "s16 : Array Size = " + std::to_string ( data.size () ).c_str () );
	u32 count = u32 ( data.size () ) < size ? u32(data.size ()) : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + "[" + std::to_string ( i ).c_str () + "] " + std::to_string ( data [i] ).c_str () );
	}
	if ( size <= data.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( data.size ()-1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<int> data, const String title, u32 size ) {
	BurgessLogMessageHeader  + "s32 : Array Size = " + std::to_string ( data.size () ).c_str () );
	u32 count = u32 ( data.size () ) < size ? u32(data.size ()) : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + "[" + std::to_string ( i ).c_str () + "] " + std::to_string ( data [i] ).c_str () );
	}
	if ( size <= data.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( data.size ()-1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<size_t> data, const String title, u32 size ) {
	BurgessLogMessageHeader  + "size_t : Array Size = " + std::to_string ( data.size () ).c_str () );
	u32 count = u32 ( data.size () ) < size ? u32(data.size ()) : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + "[" + std::to_string ( i ).c_str () + "] " + std::to_string ( data [i] ).c_str () );
	}
	if ( size <= data.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( data.size ()-1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<float> data, const String title, u32 size ) {
	BurgessLogMessageHeader  + "f32 : Array Size = " + std::to_string ( data.size () ).c_str () );
	u32 count = u32 ( data.size () )< size ? u32(data.size ()) : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + "[" + std::to_string ( i ).c_str () + "] " + GetFloorString<f32>(data[i]).c_str() );
	}
	if ( size <= data.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( data.size ()-1 ).c_str () + "]" );
	}
}

template<>
void report ( const std::vector<double> data, const String title, u32 size ) {
	BurgessLogMessageHeader  + "f64 : Array Size = " + std::to_string ( data.size () ).c_str () );
	u32 count = u32 ( data.size () ) < size ? u32(data.size ()) : size;
	for ( u8 i = 0; i<count; i++ ) {
		BurgessLogMessageHeaderArray + "[" + std::to_string ( i ).c_str () + "] " + GetFloorString<f64>(data[i]).c_str() );
	}
	if ( size <= data.size () ) {
		BurgessLogMessageHeaderArray + "...(omitted)...[" + std::to_string ( data.size ()-1 ).c_str () + "]" );
	}
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
/*template<class ClassT>
void print ( const ClassT data, const String title) {
	report ( data, title );
}

template<>
void print ( const String data, const String title ) {
	report ( data, title );
}

template<>
void print ( const unsigned char data, const String title ) {
	report ( data, title );
}

template<>
void print ( const unsigned short data, const String title ) {
	report ( data, title );
}

template<>
void print ( const unsigned int data, const String title ) {
	report ( data, title );
}

template<>
void print ( const char data, const String title ) {
	report ( data, title );
}

template<>
void print ( const short data, const String title ) {
	report ( data, title );
}

template<>
void print ( const int data, const String title ) {
	report ( data, title );
}

template<>
void print ( const float data, const String title ) {
	report ( data, title );
}

template<>
void print ( const double data, const String title ) {
	report ( data, title );
}

template<>
void print ( const size_t data, const String title ) {
	report ( data, title );
}

template<>
void print ( const Bool data, const String title ) {
	report ( data, title );
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		
//------------------------------------------------------------------------------------------------------------------------------------------------
template<class ClassT>
void print ( const std::vector<ClassT> data, const String title, u32 size ) {
	report ( data, title, size );
}

template<>
void print ( const std::vector<String> data, const String title, u32 size ) {
	report ( data, title, size );
}

template<>
void print ( const std::vector<unsigned char> data, const String title, u32 size ) {
	report ( data, title, size );
}

template<>
void print ( const std::vector<unsigned short> data, const String title, u32 size ) {
	report ( data, title, size );
}

template<>
void print ( const std::vector<unsigned int> data, const String title, u32 size ) {
	report ( data, title, size );
}

template<>
void print ( const std::vector<char> data, const String title, u32 size ) {
	report ( data, title, size );
}

template<>
void print ( const std::vector<short> data, const String title, u32 size ) {
	report ( data, title, size );
}

template<>
void print ( const std::vector<int> data, const String title, u32 size ) {
	report ( data, title, size );
}

template<>
void print ( const std::vector<size_t> data, const String title, u32 size ) {
	report ( data, title, size );
}

template<>
void print ( const std::vector<float> data, const String title, u32 size ) {
	report ( data, title, size );
}

template<>
void print ( const std::vector<double> data, const String title, u32 size ) {
	report ( data, title, size );
}*/