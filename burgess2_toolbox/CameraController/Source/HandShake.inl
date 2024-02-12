//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     éËêUÇÍÉmÉCÉYÇê∂ê¨Ç∑ÇÈ 2é≤
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void HandShakeT<T>::solve ( OUT Vec2T<T> &handShake, T time, const Vec3T<T> &cameraPos, const Vec2T<T> &shakePow, T timeScale ) {
	T t = time * timeScale * T ( 0.1 );
	Vec2T<T> baseNoise;
	baseNoise.x = ( 0.1*( std::sin ( t*3.0 ) + 0.15*std::sin ( t*1.0 ) ) - 0.2*std::sin ( t*6.0 ) ) * 0.03 * timeScale;
	baseNoise.y = ( 0.25*( std::sin ( t*3.0 ) + 0.25*std::sin ( t*1.5 ) ) - 0.2*std::sin ( t*7.0 ) ) * 0.04 * timeScale;

	Vec2T<T> pinkNoise;
	pinkNoise.x = this->core.applyNoise ( u32 ( 1 ), time, shakePow.x, timeScale );
	pinkNoise.y = this->core.applyNoise ( u32 ( 1 ), time, shakePow.y, timeScale );
	pinkNoise.x -= cameraPos.x;
	pinkNoise.y -= cameraPos.y;
	pinkNoise = pinkNoise * shakePow * 0.01;

	handShake = baseNoise + pinkNoise;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!	     éËêUÇÍÉmÉCÉYÇê∂ê¨Ç∑ÇÈÅ@3é≤
//------------------------------------------------------------------------------------------------------------------------------------------------
template <typename T>
void HandShakeT<T>::solve ( OUT Vec3T<T> &handShake, T time, const Vec3T<T> &cameraPos, const Vec3T<T> &shakePow, T timeScale ) {
	T t = time * timeScale * T ( 0.1 );
	Vec3T<T> baseNoise;
	baseNoise.x = (0.1*(std::sin ( t*3.0 ) + 0.15*std::sin ( t*1.0 )) - 0.2*std::sin ( t*6.0 )) * 0.03 * timeScale;
	baseNoise.y = (0.25*(std::sin ( t*3.0 ) + 0.25*std::sin ( t*1.5 )) - 0.2*std::sin ( t*7.0 )) * 0.04 * timeScale;
	baseNoise.z = ( 0.25*( std::sin ( t*4.0 ) + 0.3*std::sin ( t*2.0 ) ) - 0.2*std::sin ( t*4.0 ) ) * 0.01 * timeScale;

	Vec3T<T> pinkNoise;
	pinkNoise.x = this->core.applyNoise ( u32 ( 1 ), time, shakePow.x, timeScale );
	pinkNoise.y = this->core.applyNoise ( u32 ( 1 ), time, shakePow.y, timeScale );
	pinkNoise.z = this->core.applyNoise ( u32 ( 1 ), time, shakePow.z, timeScale );
	pinkNoise = ( pinkNoise - cameraPos ) * shakePow * 0.01;

	handShake = baseNoise + pinkNoise;
}