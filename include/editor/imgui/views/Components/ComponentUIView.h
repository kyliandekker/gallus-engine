//#pragma once
//
//#ifdef _EDITOR
//
//#include "editor/imgui/views/ImGuiUIView.h"
//
//#include <string>
//
//#include "gameplay/systems/Component.h"
//#include "gameplay/ECSBaseSystem.h"
//
//namespace renegade
//{
//	namespace gameplay
//	{
//		class EntityID;
//	}
//	namespace editor
//	{
//		namespace imgui
//		{
//			class ImGuiWindow;
//
//			/// <summary>
//			/// Base class for rendering components in the ImGui editor UI. 
//			/// Provides common functionality for rendering and interacting with 
//			/// components and their associated systems within the entity-component system (ECS).
//			/// </summary>
//			class ComponentBaseUIView : public ImGuiUIView
//			{
//			public:
//				/// <summary>
//				/// Constructs the ComponentBaseUIView with an associated ImGui window 
//				/// and entity ID.
//				/// </summary>
//				/// <param name="a_Window">The ImGui window for rendering the view.</param>
//				/// <param name="a_EntityID">The EntityID associated with the component being rendered.</param>
//				ComponentBaseUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID) : ImGuiUIView(a_Window), m_EntityID(a_EntityID)
//				{}
//
//				/// <summary>
//				/// Renders the component UI in the editor.
//				/// This function can be overridden by derived classes to render
//				/// component-specific details.
//				/// </summary>
//				void Render() {};
//
//				/// <summary>
//				/// Renders the base component using the provided component and system.
//				/// </summary>
//				/// <param name="a_Component">The component to be rendered.</param>
//				/// <param name="a_System">The system managing the component.</param>
//				void RenderBaseComponent(gameplay::Component& a_Component, gameplay::ECSSystem& a_System);
//			protected:
//				gameplay::EntityID& m_EntityID; // Reference to the entity's ID. Used for associating the component with an entity.
//
//				/// <summary>
//				/// Render the inner part of the UI, specific to each derived class.
//				/// </summary>
//				virtual void RenderInner() = 0;
//
//				/// <summary>
//				/// Retrieves the name of the component UI.
//				/// </summary>
//				/// <returns>The name of the UI component.</returns>
//				virtual std::string GetName() const { return ""; };
//
//				bool m_FoldedOut = true; /// Indicates whether the component UI is folded out or not.
//			};
//
//			/// <summary>
//			/// Template class for rendering specific component UIs in the editor.
//			/// Inherits from ComponentBaseUIView and allows the rendering of specific
//			/// component types with their corresponding system.
//			/// </summary>
//			/// <typeparam name="C">The component type.</typeparam>
//			/// <typeparam name="S">The system type managing the component.</typeparam>
//			template <class C, class S>
//			class ComponentUIView : public ComponentBaseUIView
//			{
//			public:
//				/// <summary>
//				/// Constructs the ComponentUIView with a specific component and system.
//				/// </summary>
//				/// <param name="a_Window">The ImGui window for rendering the view.</param>
//				/// <param name="a_EntityID">The EntityID associated with the component being rendered.</param>
//				/// <param name="a_Component">The component to render.</param>
//				/// <param name="a_System">The system managing the component.</param>
//				ComponentUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, C& a_Component, S& a_System) : ComponentBaseUIView(a_Window, a_EntityID), m_Component(a_Component), m_System(a_System)
//				{}
//
//				/// <summary>
//				/// Renders the component's UI using the base render method.
//				/// </summary>
//				void Render() override
//				{
//					RenderBaseComponent(m_Component, m_System);
//				}
//
//				/// <summary>
//				/// Render the inner part of the UI, specific to each derived class.
//				/// </summary>
//				void RenderInner() override = 0;
//			protected:
//				C& m_Component; /// Reference to the specific component being rendered.
//				S& m_System; /// Reference to the system managing the component.
//			};
//		}
//	}
//}
//
//#endif // _EDITOR