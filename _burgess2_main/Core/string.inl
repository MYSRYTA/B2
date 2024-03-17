
//------------------------------------------------------------------------------------------------------------------------------------------------
//!		­”“_‘æ‚RˆÊˆÈ‰º‚ğØ‚èÌ‚Ä‚½’l‚ğ•¶š—ñ‚Åæ“¾‚·‚é
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
String GetFloorString ( T val ) {
	String type = typeid( T ).name ();
	if ( std::is_floating_point_v<T> ) {
		// vs2019.3ˆÈ~‚Åstd::_Floating_to_string‚ª–³‚­‚È‚Á‚½
		#if _MSC_VER >= 1923
		const auto _Len = static_cast<size_t>(_CSTD _scprintf("%.3f", val));
		std::string _Str(_Len, '\0');
		_CSTD sprintf_s(&_Str[0], _Len + 1, "%.3f", val);
		return _Str;
		#endif
		#if _MSC_VER < 1923
		return std::_Floating_to_string ( "%.3f", val );
		#endif
	} else {
		return std::to_string ( val );
	}
}
