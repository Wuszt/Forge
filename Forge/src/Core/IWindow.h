#pragma once

namespace forge
{
	class IInput;

	class IWindow
	{
	public:
		enum class EventType
		{
			OnCloseRequested,
			OnResized
		};

		class IEvent
		{
		public:
			IEvent( IWindow& window )
				: m_window( window )
			{}

			virtual ~IEvent() = default;

			virtual EventType GetEventType() const = 0;

			IWindow& GetWindow() const
			{
				return m_window;
			}

		private:
			IWindow& m_window;
		};

		class OnCloseRequestedEvent : public IEvent
		{
		public:
			using IEvent::IEvent;

			virtual EventType GetEventType() const override
			{
				return EventType::OnCloseRequested;
			}
		};

		class OnResizedEvent : public IEvent
		{
		public:
			OnResizedEvent( IWindow& window, Uint32 width, Uint32 height )
				: IEvent( window )
				, m_width( width )
				, m_height( height )
			{}

			virtual EventType GetEventType() const override
			{
				return EventType::OnResized;
			}

			Uint32 GetWidth() const
			{
				return m_width;
			}

			Uint32 GetHeight() const
			{
				return m_height;
			}

		private:
			Uint32 m_width = 0u;
			Uint32 m_height = 0u;
		};

		virtual ~IWindow() = default;

		virtual Uint32 GetWidth() const = 0;
		virtual Uint32 GetHeight() const = 0;

		virtual Uint32 GetPosX() const = 0;
		virtual Uint32 GetPosY() const = 0;

		Float GetAspectRatio() const
		{
			return static_cast<Float>( GetWidth() ) / static_cast<Float>( GetHeight() );
		}

		virtual void Update() = 0;

		virtual IInput* GetInput() const = 0;

		typedef forge::Callback< const IEvent& > CallbackType;

		[[nodiscard]]
		forge::CallbackToken RegisterEventListener( const CallbackType::TFunc& func )
		{
			return m_windowCallbacks.AddListener( func );
		}

		static std::unique_ptr< IWindow > CreateNewWindow( Uint32 width, Uint32 height );

	protected:
		void DispatchEvent( const IEvent& event )
		{
			m_windowCallbacks.Invoke( event );
		}

	private:
		CallbackType m_windowCallbacks;
	};
}

