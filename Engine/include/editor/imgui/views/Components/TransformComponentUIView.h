#pragma once

#ifdef __EDITOR__

#include "editor/imgui/views/Components/ComponentUIView.h"

#include <glm/vec3.hpp>

#include "gameplay/systems/TransformComponent.h"
#include "gameplay/systems/TransformSystem.h"
#include "editor/imgui/views/DataTypes/VectorView.h"
#include "editor/imgui/views/DataTypes/ColorView.h"

namespace renegade
{
	namespace editor
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the TransformComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class TransformComponentUIView : public ComponentUIView<gameplay::TransformComponent, gameplay::TransformSystem>
			{
			public:
				/// <summary>
				/// Constructs the TransformComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EntityID">The entity ID associated with the transform component.</param>
				/// <param name="a_TransformComponent">The TransformComponent to display and edit.</param>
				/// <param name="a_System">The TransformSystem responsible for managing the TransformComponent.</param>
				TransformComponentUIView(const ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, gameplay::TransformComponent& a_TransformComponent, gameplay::TransformSystem& a_System) : ComponentUIView(a_Window, a_EntityID, a_TransformComponent, a_System),
					m_PositionView(a_Window),
					m_RotationView(a_Window),
					m_ScaleView(a_Window)
				{ }
			private:
				/// <summary>
				/// Render the inner part of the UI, including position, rotation, scale, and color.
				/// </summary>
				void RenderInner() override;

				/// <summary>
				/// Retrieves the name of the component UI.
				/// </summary>
				/// <returns>The name of the UI component.</returns>
				std::string GetName() const override;

				/// UI elements for displaying and editing the position, rotation, and scale as glm::vec3.
				Vector3View<glm::vec3>
					m_PositionView,
					m_RotationView,
					m_ScaleView;
			};
		}
	}
}

#endif // __EDITOR__