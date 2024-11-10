#pragma once
namespace forge
{
	class EditorInstance;
	class PropertiesChain;
}

namespace editor
{
	class TypeDrawer
	{
		RTTI_DECLARE_ABSTRACT_CLASS( TypeDrawer );

	public:
		class Drawable
		{
		public:
			using OnModifiedFunc = std::function< void( const forge::PropertiesChain& ) >;

			Drawable( void* address, forge::Index16 containerIndex, const Drawable* parent, OnModifiedFunc onModified )
				: m_address( address )
				, m_parent( parent )
				, m_containerIndex( containerIndex )
				, m_onModified( std::move( onModified ) )
			{}

			virtual ~Drawable() = default;
			virtual const rtti::Type& GetType() const = 0;
			virtual rtti::InstanceFlags GetInstanceFlags() const = 0;
			virtual Bool HasMetadata( const std::string& key ) const = 0;
			virtual const std::string* GetMetadataValue( const std::string& key ) const = 0;
			virtual const Char* GetName() const = 0;
			virtual const Char* GetID() const = 0;
			virtual const Drawable* GetParent() const { return m_parent; }

			void* GetAddress() const
			{
				return m_address;
			}

			forge::Index16 GetArrayIndex() const
			{
				return m_containerIndex;
			}

			void InvokeOnModification( const forge::PropertiesChain& propertiesChain ) const
			{
				if ( m_onModified )
				{
					m_onModified( propertiesChain );
				}
			}

		private:
			void* m_address = nullptr;
			const Drawable* m_parent = nullptr;
			forge::Index16 m_containerIndex;
			OnModifiedFunc m_onModified;
		};

		TypeDrawer( forge::EngineInstance& engineInstance )
			: m_engineInstance( &engineInstance )
		{}

		using TypeNameDrawerFunc = std::function< void( const Drawable& ) >;
		static void Draw( forge::EngineInstance& engineInstance, const Drawable& drawable, const TypeNameDrawerFunc& nameDrawer = {} );
		static void DrawChildren( forge::EngineInstance& engineInstance, const Drawable& drawable );

		void Draw( const Drawable& drawable, const TypeNameDrawerFunc& nameDrawer ) const;
		void DrawChildren( const Drawable& drawable ) const;
		void DrawValue( const Drawable& drawable ) const;

		virtual Bool HasChildren( const Drawable& drawable ) const;

		void SetEngineInstance( forge::EngineInstance& engineInstance )
		{
			m_engineInstance = &engineInstance;
		}

	protected:
		TypeDrawer() = default;

		virtual void OnDrawName( const Drawable& drawable ) const;
		virtual void OnDrawChildren( const Drawable& drawable ) const;
		virtual void OnDrawValue( const Drawable& drawable ) const;

		forge::EngineInstance& GetEngineInstance() const
		{
			return *m_engineInstance;
		}

		template< class T >
		static T& GetValue( void* address )
		{
			return *static_cast< T* >( address );
		}

	private:
		void DrawChildrenInternal( const Drawable& drawable, const Vector2& startPos, Float height ) const;
		forge::EngineInstance* m_engineInstance = nullptr;
	};

	class CustomTypeDrawer : public TypeDrawer
	{
		RTTI_DECLARE_ABSTRACT_CLASS( CustomTypeDrawer, editor::TypeDrawer );

	public:
		virtual const rtti::Type& GetSupportedType() const = 0;
	protected:
		CustomTypeDrawer() = default;
	};

	class DrawableType : public TypeDrawer::Drawable
	{
	public:
		DrawableType( void* address, const rtti::Type& type, const Char* id = "", OnModifiedFunc onModified = nullptr )
			: TypeDrawer::Drawable( address, forge::Index16(), nullptr, std::move( onModified ) )
			, m_type( type )
			, m_id( id )
		{}

		virtual const rtti::Type& GetType() const {	return m_type; }
		virtual rtti::InstanceFlags GetInstanceFlags() const { return rtti::InstanceFlags::None; }
		virtual Bool HasMetadata( const std::string& key ) const override { return false; }
		virtual const std::string* GetMetadataValue( const std::string& key ) const override { return nullptr; }
		virtual const Char* GetName() const override { return nullptr; }
		virtual const Char* GetID() const override { return m_id; }

	private:
		const rtti::Type& m_type;
		const Char* m_id = nullptr;
	};

	class DrawableProperty : public TypeDrawer::Drawable
	{
	public:
		DrawableProperty( void* address, const rtti::Property& property, const TypeDrawer::Drawable& parent, forge::Index16 containerIndex = forge::Index16(), OnModifiedFunc onModified = nullptr )
			: TypeDrawer::Drawable( address, containerIndex, &parent, std::move( onModified ) )
			, m_property( property )
		{}

		virtual const rtti::Type& GetType() const override { return m_property.GetType(); }
		virtual Bool HasMetadata( const std::string& key ) const override { return m_property.HasMetadata( key ); }
		virtual const std::string* GetMetadataValue( const std::string& key ) const override { return m_property.GetMetadataValue( key ); }
		virtual rtti::InstanceFlags GetInstanceFlags() const override { return m_property.GetFlags(); }
		virtual const Char* GetName() const override { return m_property.GetName(); }
		virtual const Char* GetID() const override { return m_property.GetName(); }

	private:
		const rtti::Property& m_property;
	};
}