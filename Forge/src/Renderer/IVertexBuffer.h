#pragma once

namespace renderer
{
	enum class InputType
	{
		Position,
		Color,
		TexCoord,
		Normal,
		BlendWeights,
		BlendIndices,
		Count
	};

	enum class InputFormat
	{
		R32_UINT,
		R32_FLOAT,
		R32G32_UINT,
		R32G32_FLOAT,
		R32G32B32_FLOAT,
		R32G32B32A32_UINT,
		R32G32B32A32_FLOAT,
	};

	enum class InputClassification
	{
		PerVertex
	};

	struct InputElementDescription
	{
		InputType m_inputType;
		InputFormat m_inputFormat;
		InputClassification m_classification;
		Uint32 m_size;
	};

	struct InputColor : Vector4
	{
		template< class... Types >
		InputColor( Types... types ) : Vector4( std::forward< Types >( types )... ) {}

		static constexpr InputElementDescription GetInputDescription()
		{
			return { InputType::Color, InputFormat::R32G32B32A32_FLOAT, InputClassification::PerVertex, sizeof( InputColor ) };
		}
	};

	struct InputPosition : Vector3
	{
		template< class... Types >
		InputPosition( Types... types ) : Vector3( std::forward< Types >( types )... ) {}

		static constexpr InputElementDescription GetInputDescription()
		{
			return { InputType::Position, InputFormat::R32G32B32_FLOAT, InputClassification::PerVertex, sizeof( InputPosition ) };
		}
	};

	struct InputNormal : Vector3
	{
		template< class... Types >
		InputNormal( Types... types ) : Vector3( std::forward< Types >( types )... ) {}

		static constexpr InputElementDescription GetInputDescription()
		{
			return { InputType::Normal, InputFormat::R32G32B32_FLOAT, InputClassification::PerVertex, sizeof( InputNormal ) };
		}
	};

	struct InputTexCoord : Vector2
	{
		template< class... Types >
		InputTexCoord( Types... types ) : Vector2( std::forward< Types >( types )... ) {}

		static constexpr InputElementDescription GetInputDescription()
		{
			return { InputType::TexCoord, InputFormat::R32G32_FLOAT, InputClassification::PerVertex, sizeof( InputTexCoord ) };
		}
	};

	struct InputBlendWeights
	{
		Float m_weights[4] = {0.0f};
		static constexpr InputElementDescription GetInputDescription()
		{
			return {InputType::BlendWeights, InputFormat::R32G32B32A32_FLOAT, InputClassification::PerVertex, sizeof(InputBlendWeights)};
		}
	};

	struct InputBlendIndices
	{
		Uint32 m_indices[ 4 ] = { 0u };
		static constexpr InputElementDescription GetInputDescription()
		{
			return {InputType::BlendIndices, InputFormat::R32G32B32A32_UINT, InputClassification::PerVertex, sizeof(InputBlendIndices)};
		}
	};

	using Indices = std::vector< Uint32 >;

	struct Shape
	{
		Indices m_indices;
		Uint32 m_materialIndex = 0u;
	};

	class Vertices
	{
	public:
		Vertices() {}

		template< class arrT, class... arrTs >
		Vertices( const arrT& t0, const arrTs&... ts )
		{
			m_elementsAmount = static_cast< Uint32 >( t0.size() );
			m_elementSize = GetSingleVertexDataSize< arrT, arrTs... >();

			m_buffer = std::make_unique< Char[] >( m_elementSize * m_elementsAmount );
			AddData( 0u, t0, ts... );
		}

		Uint32 GetVertexByteWidth() const
		{
			return m_elementSize;
		}

		Uint32 GetVerticesByteWidth() const
		{
			return m_elementSize * m_elementsAmount;
		}

		const void* GetData() const
		{
			return m_buffer.get();
		}

		const std::vector< InputElementDescription >& GetInputElements() const
		{
			return m_inputElements;
		}

	private:
		template< class... arrTs >
		FORGE_INLINE decltype( typename std::enable_if<sizeof...( arrTs ) == 0, Uint32>::type() ) GetSingleVertexDataSize()
		{
			return 0u;
		}

		template< class arrT, class... arrTs >
		Uint32 GetSingleVertexDataSize()
		{
			return sizeof( arrT::value_type ) + GetSingleVertexDataSize< arrTs... >();
		}

		template< class... arrTs >
		FORGE_INLINE static decltype( typename std::enable_if<sizeof...( arrTs ) == 0, void>::type() ) AddData( Uint32 offset, const arrTs&... arrs )
		{}

		template< class arrT, class... arrTs >
		void AddData( Uint32 offset, const arrT& arr, const arrTs&... arrs )
		{
			Uint32 tSize = sizeof( arr[ 0 ] );

			for( Uint32 i = 0; i < m_elementsAmount; ++i )
			{
				memcpy( m_buffer.get() + i * m_elementSize + offset, &( arr[ 0 ] ) + i, tSize );
			}

			m_inputElements.emplace_back( arr[ 0 ].GetInputDescription() );

			AddData( tSize + offset, arrs... );
		}

		std::vector< InputElementDescription > m_inputElements;
		std::unique_ptr< Char[] > m_buffer;
		Uint32 m_elementSize = 0u;
		Uint32 m_elementsAmount = 0u;
	};

	class IVertexBuffer
	{
	public:
		virtual ~IVertexBuffer() {}

		virtual void Set() const = 0;

		struct SemanticDesc
		{
			std::string m_name;
			Uint32 m_amount = 0u;
		};

		virtual std::vector< SemanticDesc > GetSemanticDescriptions() const = 0;
	};
}

