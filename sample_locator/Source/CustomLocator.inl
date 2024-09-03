namespace{
	struct MColorHash
	{
		std::size_t operator()( const MColor& color ) const
		{
			std::size_t seed = 0;
			CombineHashCode ( seed, color.r );
			CombineHashCode ( seed, color.g );
			CombineHashCode ( seed, color.b );
			CombineHashCode ( seed, color.a );
			return seed;
		}

		void CombineHashCode ( std::size_t& seed, float v ) const
		{
			std::hash<float> hasher;
			seed ^= hasher ( v ) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
	};

	std::unordered_map<MColor, MHWRender::MShaderInstance*, MColorHash> the3dSolidShaders;

	MHWRender::MShaderInstance* get3dSolidShader ( const MColor& color )
	{
		// Return the shader instance if exists.
		auto it = the3dSolidShaders.find ( color );
		if (it != the3dSolidShaders.end ())
		{
			return it->second;
		}

		MHWRender::MShaderInstance* shader = NULL;

		MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer ();
		if (renderer)
		{
			const MHWRender::MShaderManager* shaderMgr = renderer->getShaderManager ();
			if (shaderMgr)
			{
				shader = shaderMgr->getStockShader ( MHWRender::MShaderManager::k3dSolidShader );
			}
		}

		if (shader)
		{
			float solidColor[] = { color.r, color.g, color.b, color.a };
			shader->setParameter ( "solidColor", solidColor );

			the3dSolidShaders[color] = shader;
		}

		return shader;
	}

	void setSolidColor ( MHWRender::MShaderInstance* shaderInstance, const float *value )
	{
		if (!shaderInstance)
			return;
		const MString colorParameterName = "solidColor";
		shaderInstance->setParameter ( colorParameterName, value );
	}

	MStatus releaseShaders ()
	{
		MHWRender::MRenderer* renderer = MHWRender::MRenderer::theRenderer ();
		if (renderer)
		{
			const MHWRender::MShaderManager* shaderMgr = renderer->getShaderManager ();
			if (shaderMgr)
			{
				for (auto it = the3dSolidShaders.begin (); it != the3dSolidShaders.end (); it++)
				{
					shaderMgr->releaseShader ( it->second );
				}

				the3dSolidShaders.clear ();
				return MS::kSuccess;
			}
		}

		return MS::kFailure;
	}
}

