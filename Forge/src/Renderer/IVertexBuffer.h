#pragma once

namespace renderer
{
	enum class InputType
	{
		Position,
		Color,
		Count
	};

	enum class InputFormat
	{
		R32G32B32,
		R32G32B32A32,
	};

	enum class InputClassification
	{
		PerVertex
	};

	struct InputElement
	{
		InputType m_inputType;
		InputFormat m_inputFormat;
		InputClassification m_classification;
		Uint32 m_size;
	};

	template< typename... Types >
	struct IVertex
	{
		using ElementType = void;
	};

	template< class T, class ... Types >
	struct IVertex< T, Types... >
	{
		using ElementType = T;

		IVertex() {}

		IVertex( const T& val, Types... t )
			: m_data( val )
			, m_restOfData( std::forward< Types >( t )... )
		{}

		T m_data;
		IVertex< Types... > m_restOfData;

		static std::vector< InputElement > GetInputElements()
		{
			std::vector< InputElement > dynArray;
			GetInputElementsInternal< IVertex< T, Types... > >( dynArray );
			return dynArray;
		}

	private:
		template< class T >
		static void GetInputElementsInternal( std::vector< InputElement >& output )
		{
			output.emplace_back( T::ElementType::GetInputDescription() );
			GetInputElementsInternal< decltype( T::m_restOfData ) >( output );
		}

		template<>
		static void GetInputElementsInternal< IVertex<> >( std::vector< InputElement >& output )
		{}
	};

	struct InputColor : Vector4
	{
		template< class... Types >
		InputColor( Types... types ) : Vector4( std::forward< Types >( types )... ) {}

		static constexpr InputElement GetInputDescription()
		{
			return { InputType::Color, InputFormat::R32G32B32A32, InputClassification::PerVertex, sizeof( InputColor ) };
		}
	};

	struct InputPosition : Vector3
	{
		template< class... Types >
		InputPosition( Types... types ) : Vector3( std::forward< Types >( types )... ) {}

		static constexpr InputElement GetInputDescription()
		{
			return { InputType::Position, InputFormat::R32G32B32, InputClassification::PerVertex, sizeof( InputPosition ) };
		}
	};

	struct IVertices
	{
		virtual ~IVertices() {}

		virtual Uint32 GetVertexByteWidth() const = 0;
		virtual Uint32 GetVerticesByteWidth() const = 0;
		virtual const void* GetData() const = 0;
		virtual std::vector< InputElement > GetInputElements() const = 0;
	};

	template< class VertexType >
	struct Vertices : IVertices
	{
		Vertices( std::vector< VertexType >&& vertices )
			: m_vertices( std::move( vertices ) )
		{}

		std::vector< VertexType > m_vertices;

		virtual Uint32 GetVertexByteWidth() const override
		{
			return sizeof( VertexType );
		}

		virtual Uint32 GetVerticesByteWidth() const override
		{
			return static_cast<Uint32>( m_vertices.size() * sizeof( VertexType ) );
		}


		virtual const void* GetData() const override
		{
			return m_vertices.data();
		}


		virtual std::vector<InputElement> GetInputElements() const override
		{
			return VertexType::GetInputElements();
		}

	};

	class IVertexBuffer
	{
	public:
		~IVertexBuffer() {}

		virtual void Set() = 0;
	};
}

