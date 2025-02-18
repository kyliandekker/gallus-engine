#pragma once

#ifdef _EDITOR

#include "editor/imgui/views/Components/ComponentUIView.h"

#include <DirectXMath.h>
#include "gameplay/systems/MeshSystem.h"
#include "gameplay/systems/components/MeshComponent.h"

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			/// <summary>
			/// UI view for rendering and interacting with the MeshComponent in the ImGui editor.
			/// Inherits from ComponentUIView and provides specific rendering for position, rotation and scale of the transform.
			/// </summary>
			class MeshComponentUIView : public ComponentUIView<gameplay::MeshComponent, gameplay::MeshSystem>
			{
			public:
				/// <summary>
				/// Constructs the MeshComponentUIView with specific views for the position, rotation and scale.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EntityID">The entity ID associated with the transform component.</param>
				/// <param name="a_MeshComponent">The MeshComponent to display and edit.</param>
				/// <param name="a_System">The MeshSystem responsible for managing the MeshComponent.</param>
				MeshComponentUIView(ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID, gameplay::MeshComponent& a_MeshComponent, gameplay::MeshSystem& a_System) : ComponentUIView(a_Window, a_EntityID, a_MeshComponent, a_System)
				{}
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
			};
		}
	}
}

#endif // _EDITOR