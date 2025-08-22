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
		Uint32 m_size = 0u;
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
		Float m_weights[ 4 ] = { 0.0f };
		static constexpr InputElementDescription GetInputDescription()
		{
			return { InputType::BlendWeights, InputFormat::R32G32B32A32_FLOAT, InputClassification::PerVertex, sizeof( InputBlendWeights ) };
		}
	};

	struct InputBlendIndices
	{
		Uint32 m_indices[ 4 ] = { 0u };
		static constexpr InputElementDescription GetInputDescription()
		{
			return { InputType::BlendIndices, InputFormat::R32G32B32A32_UINT, InputClassification::PerVertex, sizeof( InputBlendIndices ) };
		}
	};

	using Indices = std::vector< Uint32 >;

	struct Shape
	{
		Indices m_indices;
		Uint32 m_materialIndex = 0u;
	};

	class Vertices;

	class VerticesBuilder
	{
	public:
		Vertices Build();

		template< class T >
		void AddData( std::vector< T > vec )
		{
			FORGE_ASSERT( !vec.empty() );
			FORGE_ASSERT( m_verticesAmount == 0u || m_verticesAmount == static_cast< Uint32 >( vec.size() ) );
			m_verticesAmount = static_cast< Uint32 >( vec.size() );

			const auto& element = m_elements.emplace_back( std::make_unique< ElementDesc< T > >( vec ) );
			m_vertexSize += element->GetElementSize();
		}

		class IElementDesc
		{
		public:
			IElementDesc( InputElementDescription desc );
			virtual ~IElementDesc() = 0;

			const InputElementDescription& GetDesc() const
			{
				return m_description;
			}

			virtual Uint32 GetElementSize() const = 0;

			virtual const void* GetData() const = 0;

			InputElementDescription m_description;
		};

		template< class T >
		class ElementDesc : public IElementDesc
		{
		public:
			ElementDesc( std::vector< T > elements )
				: m_elements( std::move( elements ) )
				, IElementDesc( T::GetInputDescription() )
			{}

			virtual Uint32 GetElementSize() const override
			{
				return sizeof( T );
			}

			virtual const void* GetData() const override
			{
				return static_cast< const void* >( m_elements.data() );
			}

		private:
			std::vector< T > m_elements;
		};

		const std::vector< std::unique_ptr< IElementDesc > >& GetElementsDescs() const
		{
			return m_elements;
		}

		Uint32 GetVerticesAmount() const
		{
			return m_verticesAmount;
		}

		Uint32 GetVertexSize() const
		{
			return m_vertexSize;
		}

	private:
		std::vector< std::unique_ptr< IElementDesc > > m_elements;
		Uint32 m_verticesAmount = 0u;
		Uint32 m_vertexSize = 0u;
	};

	class Vertices
	{
	public:
		Vertices() = default;

		template< class T, class... TSpans >
		Vertices( forge::ArraySpan< const T > t0, const TSpans&... ts )
		{
			m_verticesAmount = static_cast< Uint32 >( t0.GetSize() );
			m_vertexSize = GetSingleVertexDataSize< forge::ArraySpan< const T >, TSpans... >();

			m_buffer = forge::UniqueRawPtr( m_vertexSize * m_verticesAmount );
			AddDataInternalT( 0u, t0, ts... );
		}

		Vertices( const VerticesBuilder& builder )
		{
			m_verticesAmount = builder.GetVerticesAmount();
			m_vertexSize = builder.GetVertexSize();

			m_buffer = forge::UniqueRawPtr( m_vertexSize * m_verticesAmount );

			Uint32 offset = 0u;
			for ( const auto& element : builder.GetElementsDescs() )
			{
				AddDataInternal( offset, element->GetData(), element->GetElementSize(), element->GetDesc() );
				offset += element->GetElementSize();
			}
		}

		template< class T, class... TSpans >
		void AddData( const forge::ArraySpan< const T >& t0, const TSpans&... ts )
		{
			Uint32 prevVertexSize = m_vertexSize;
			Uint32 additionalVertexSize = GetSingleVertexDataSize< forge::ArraySpan< const T >, TSpans... >();

			m_vertexSize = prevVertexSize + additionalVertexSize;
			forge::UniqueRawPtr oldBuffer = std::move( m_buffer );
			m_buffer = forge::UniqueRawPtr( m_verticesAmount * m_vertexSize );

			AddDataInternal( 0u, oldBuffer.GetData(), prevVertexSize );

			AddDataInternalT( prevVertexSize, t0, ts... );
		}

		Uint32 GetVertexSize() const
		{
			return m_vertexSize;
		}

		Uint32 GetBufferSize() const
		{
			return m_vertexSize * m_verticesAmount;
		}

		Uint32 GetVerticesAmount() const
		{
			return m_verticesAmount;
		}

		const void* GetData() const
		{
			return m_buffer.GetData();
		}

		const std::vector< InputElementDescription >& GetInputElements() const
		{
			return m_inputElements;
		}

	private:
		template< class... TSpans >
		decltype( typename std::enable_if<sizeof...( TSpans ) == 0, Uint32>::type() ) GetSingleVertexDataSize()
		{
			return 0u;
		}

		template< class TSpan, class... TSpans >
		Uint32 GetSingleVertexDataSize()
		{
			return sizeof( TSpan::ValueType ) + GetSingleVertexDataSize< TSpans... >();
		}

		template< class... TSpans >
		static decltype( typename std::enable_if<sizeof...( TSpans ) == 0, void>::type() ) AddDataInternalT( Uint32 offset, const TSpans&... spans )
		{
		}

		template< class TSpan, class... TSpans >
		void AddDataInternalT( Uint32 offset, const TSpan& span, const TSpans&... spans )
		{
			Uint32 tSize = sizeof( TSpan::ValueType );

			AddDataInternal( offset, span.GetData(), tSize, TSpan::ValueType::GetInputDescription() );

			AddDataInternalT( tSize + offset, spans... );
		}

		void AddDataInternal( Uint32 offset, const void* data, Uint32 elementSize )
		{
			for ( Uint32 i = 0; i < m_verticesAmount; ++i )
			{
				memcpy( static_cast< Byte* >( m_buffer.GetData() ) + i * m_vertexSize + offset, static_cast< const Byte* >( data ) + i * elementSize, elementSize );
			}
		}

		void AddDataInternal( Uint32 offset, const void* data, Uint32 elementSize, const InputElementDescription& desc )
		{
			AddDataInternal( offset, data, elementSize );

			m_inputElements.emplace_back( desc );
		}

		std::vector< InputElementDescription > m_inputElements;
		forge::UniqueRawPtr m_buffer;
		Uint32 m_vertexSize = 0u;
		Uint32 m_verticesAmount = 0u;
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

