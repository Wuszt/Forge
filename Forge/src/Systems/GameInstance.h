#pragma once

namespace systems
{
	class SystemsManager;
}

namespace forge
{
	class EntitiesManager;

	class GameInstance
	{
	public:
		GameInstance();
		GameInstance( const GameInstance& gi ) = delete;
		~GameInstance() = default;

		FORGE_INLINE systems::SystemsManager& GetSystemsManager() const
		{
			return *m_systemManager;
		}

		FORGE_INLINE forge::EntitiesManager& GetEntitiesManager() const
		{
			return *m_entitiesManager;
		}

	private:
		std::unique_ptr< EntitiesManager > m_entitiesManager;
		std::unique_ptr< systems::SystemsManager > m_systemManager;
	};

	class IManager
	{
	public:
		IManager( GameInstance& gameInstance )
			: m_gameInstance( gameInstance )
		{}

		IManager( const IManager& manager ) = delete;

		virtual void Initialize() {}
		virtual void Deinitialize() {}

		FORGE_INLINE GameInstance& GetGameInstance() const
		{
			return m_gameInstance;
		}

	protected:
		virtual ~IManager() = default;

		GameInstance& m_gameInstance;
	};

}

