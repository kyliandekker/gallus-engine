#pragma once

#ifdef __EDITOR__

#include "editor/imgui/views/Selectables/EditorSelectable.h"

#include <vector>

//#include "gameplay/EntityID.h"
#include "editor/imgui/views/DataTypes/StringTextInput.h"
#include "editor/imgui/views/DataTypes/StringDropdown.h"

namespace coopscoop
{
	namespace editor
	{
		namespace imgui
		{
			class ComponentBaseUIView;
			class ImGuiWindow;

			/// <summary>
			/// Represents a UI view for an entity in the editor, providing rendering and interaction functionality.
			/// </summary>
			class EntityUIView : public EditorSelectable
			{
			public:
				/// <summary>
				/// Constructs an EntityUIView for a given entity ID.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <param name="a_EntityID">The entity ID associated with this view.</param>
				EntityUIView(const ImGuiWindow& a_Window, gameplay::EntityID& a_EntityID);

				/// <summary>
				/// Default constructor for EntityUIView.
				/// </summary>
				EntityUIView() = default;

				/// <summary>
				/// Retrieves the icon string representing this entity in the UI.
				/// </summary>
				/// <returns>A string representing the icon.</returns>
				std::string GetIcon() const;

				void Render() override
				{};

/// <summary>
/// Renders the entity UI with selection and click interaction.
/// </summary>
/// <param name="clicked">Reference to a boolean indicating if the entity was clicked.</param>
/// <param name="selected">Boolean indicating if the entity is currently selected.</param>
				void Render(bool& clicked, bool selected);

				/// <summary>
				/// Renders the selectable UI for the entity.
				/// </summary>
				void RenderSelectable() override;

				/// <summary>
				/// Executes logic when the entity is selected.
				/// </summary>
				void Select() override;

				/// <summary>
				/// Executes logic when the entity is deselected.
				/// </summary>
				void Deselect() override;

				/// <summary>
				/// Retrieves the entity ID associated with this view.
				/// </summary>
				/// <returns>A reference to the EntityID.</returns>
				gameplay::EntityID& GetEntityID() const;
			private:
				gameplay::EntityID& m_EntityID; /// The entity ID associated with this view.
				std::vector<ComponentBaseUIView*> m_Components; /// A list of component UI views associated with this entity.

				StringTextInput m_NameInput; /// Inspector input for editing the entity's name.
			};
		}
	}
}

#endif // __EDITOR__