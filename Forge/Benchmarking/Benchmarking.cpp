#include <iostream>
#include "../src/Core/PublicDefaults.h"
#include "../src/Core/Serializer.h"
#include "../src/Core/Streams.h"
#include "../src/Math/Random.h"

struct StructWithPrimitives
{
	RTTI_DECLARE_STRUCT( StructWithPrimitives );

	auto operator<=>( const StructWithPrimitives& ) const = default;
	Float m_float = 0.0f;
	Byte m_dummy0 = 0;
	Uint32 m_uint0 = 0u;
	Uint32 m_uint1 = 0u;
	Uint32 m_uint2 = 0u;
	Byte m_dummy1 = 0;
	Uint32 m_uint3 = 0u;
	Uint32 m_uint4 = 0u;
	Byte m_dummy2 = 0;
	Uint32 m_uint5 = 0u;

	static StructWithPrimitives GetInitialized( Uint32 seed = 123456789u )
	{
		Math::Random rng( seed );

		StructWithPrimitives data;

		data.m_float = rng.GetFloat();
		data.m_uint0 = rng.GetUnsigned();
		data.m_uint1 = rng.GetUnsigned();
		data.m_uint2 = rng.GetUnsigned();
		data.m_uint3 = rng.GetUnsigned();
		data.m_uint4 = rng.GetUnsigned();
		data.m_uint5 = rng.GetUnsigned();

		return data;
	}
};

RTTI_IMPLEMENT_TYPE( StructWithPrimitives,
	RTTI_REGISTER_PROPERTY( m_float );
RTTI_REGISTER_PROPERTY( m_uint0 );
RTTI_REGISTER_PROPERTY( m_uint1 );
RTTI_REGISTER_PROPERTY( m_uint2 );
RTTI_REGISTER_PROPERTY( m_uint3 );
RTTI_REGISTER_PROPERTY( m_uint4 );
RTTI_REGISTER_PROPERTY( m_uint5 );
);

struct StructWithContainers
{
	RTTI_DECLARE_STRUCT( StructWithContainers );

	auto operator<=>( const StructWithContainers& ) const = default;

	std::array< Uint32, 123u > m_array;
	std::vector< StructWithPrimitives > m_vec;
	std::unordered_set< Uint32 > m_set;
	std::unordered_map< Uint32, StructWithPrimitives > m_map;

	static StructWithContainers GetInitialized( Uint32 seed = 0u )
	{
		StructWithContainers src;

		for ( Uint32 i = 0u; i < 123u; ++i )
		{
			src.m_array[ i ] = i;
			src.m_vec.emplace_back( StructWithPrimitives::GetInitialized( seed * i + i ) );
			src.m_set.emplace( i );
			src.m_map.emplace( i, StructWithPrimitives::GetInitialized( seed * i + i ) );
		}

		return src;
	}
};

RTTI_IMPLEMENT_TYPE( StructWithContainers,
	RTTI_REGISTER_PROPERTY( m_array );
	RTTI_REGISTER_PROPERTY( m_vec );
	RTTI_REGISTER_PROPERTY( m_set );
	RTTI_REGISTER_PROPERTY( m_map );
);

struct StructWithPointers
{
	RTTI_DECLARE_STRUCT( StructWithPointers );

	std::unique_ptr< Uint32 > m_uniquePrimitive;
	std::unique_ptr< StructWithContainers > m_uniqueStruct;
	std::shared_ptr< Uint32 > m_sharedPrimitive0;
	std::shared_ptr< StructWithContainers > m_sharedStruct0;
	std::shared_ptr< Uint32 > m_sharedPrimitive1;
	std::shared_ptr< StructWithContainers > m_sharedStruct1;
};

RTTI_IMPLEMENT_TYPE( StructWithPointers,
	RTTI_REGISTER_PROPERTY( m_uniquePrimitive );
	RTTI_REGISTER_PROPERTY( m_uniqueStruct );
	RTTI_REGISTER_PROPERTY( m_sharedPrimitive0 );
	RTTI_REGISTER_PROPERTY( m_sharedStruct0 );
	RTTI_REGISTER_PROPERTY( m_sharedPrimitive1 );
	RTTI_REGISTER_PROPERTY( m_sharedStruct1 );
);

int main()
{
	forge::Time::Initialize();

    Uint64 iterations = 10;

	Double init = 0.0;
	Double serializing = 0.0;
	Double deserializing = 0.0;

	forge::StopWatch allSw;
	StructWithPointers src;
	src.m_uniquePrimitive = std::make_unique< Uint32 >( 123u );
	src.m_uniqueStruct = std::make_unique< StructWithContainers >( StructWithContainers::GetInitialized() );
	src.m_sharedPrimitive0 = std::make_shared< Uint32 >( 123u );
	src.m_sharedStruct0 = std::make_shared< StructWithContainers >( StructWithContainers::GetInitialized() );
	src.m_sharedPrimitive1 = src.m_sharedPrimitive0;
	src.m_sharedStruct1 = src.m_sharedStruct0;
    for ( Uint64 i = 0u; i < iterations; ++i )
    {
		forge::StopWatch initSw;

		StructWithPointers dest;

		const Uint64 c_bufferInitialSize = 256 * 1024;
		forge::MemoryStream stream( c_bufferInitialSize );

		init += initSw.GetDuration();

		forge::StopWatch serializationSw;
		forge::Serializer inSerializer( stream );
		inSerializer.Serialize( src );
		serializing += serializationSw.GetDuration();

		stream.ResetPos();

		forge::StopWatch deserializationSw;
		forge::Deserializer outSerializer( stream );
		outSerializer.Deserialize( dest );
		deserializing += deserializationSw.GetDuration();
    }

    std::cout << "All:" << allSw.GetDuration() << "\n";
	std::cout << "Init:" << init << "\n";
	std::cout << "Serializing:" << serializing << "\n";
	std::cout << "Deserializing:" << deserializing << "\n";
}