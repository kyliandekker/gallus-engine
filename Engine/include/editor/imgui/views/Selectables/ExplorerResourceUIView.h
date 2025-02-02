#pragma once

#ifdef __EDITOR__

#include "editor/imgui/views/Selectables/EditorSelectable.h"

#include <vector>
#include <string>

#include "editor/imgui/views/DataTypes/StringTextInput.h"
#include "assets/AssetType.h"

namespace coopscoop
{
	namespace editor
	{
		class ExplorerResource;
		enum class ExplorerResourceType;

		namespace imgui
		{
			class ImGuiWindow;

			enum class ExplorerResourceUIType
			{
				ExplorerResourceType_Resource,
				ExplorerResourceType_PreviousFolder
			};

			/// <summary>
			/// Represents a UI view for an explorer resource in the editor. 
			/// Manages display, interaction, and hierarchical structure for resources.
			/// </summary>
			class ExplorerResourceUIView : public EditorSelectable
			{
			public:
				/// <summary>
				/// Constructs an ExplorerResourceUIView for a given window.
				/// </summary>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				ExplorerResourceUIView(const ImGuiWindow& a_Window);

				/// <summary>
				/// Destructor for ExplorerResourceUIView.
				/// </summary>
				virtual ~ExplorerResourceUIView();

				/// <summary>
				/// Factory method to create a view for a given explorer resource.
				/// </summary>
				/// <param name="a_Resource">Pointer to the explorer resource.</param>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				/// <returns>A pointer to the created ExplorerResourceUIView.</returns>
				static ExplorerResourceUIView* CreateViewFromExplorerResource(ExplorerResource* a_Resource, const ImGuiWindow& a_Window);

				/// <summary>
				/// Retrieves the name of the resource associated with this view.
				/// </summary>
				/// <returns>A reference to the resource name string.</returns>
				const std::string& GetName() const;

				/// <summary>
				/// Sets the name of the resource associated with this view.
				/// </summary>
				void SetName(const std::string& a_Name);

				/// <summary>
				/// Retrieves the icon string for the resource.
				/// </summary>
				/// <returns>A reference to the icon string.</returns>
				const std::string& GetIcon() const;

				/// <summary>
				/// Sets the icon string for the resource.
				/// </summary>
				void SetIcon(const std::string& a_Icon);

				/// <summary>
				/// Checks if the resource node is folded out in the UI.
				/// </summary>
				/// <returns>True if the node is folded out, otherwise false.</returns>
				bool IsFoldedOut() const;

				/// <summary>
				/// Sets the folded-out state of the resource node.
				/// </summary>
				/// <param name="a_FoldedOut">True to fold out, false to collapse.</param>
				void SetFoldedOut(bool a_FoldedOut);

				void Render() override
				{};

/// <summary>
/// Renders the explorer resource in the UI.
/// </summary>
/// <param name="a_Clicked">Indicates if the resource was clicked.</param>
/// <param name="a_RightClicked">Indicates if the resource was right-clicked.</param>
/// <param name="a_DoubleClicked">Indicates if the resource was double-clicked.</param>
/// <param name="a_Selected">Indicates if the resource is selected.</param>
				void Render(bool& a_Clicked, bool& a_RightClicked, bool& a_DoubleClicked, bool a_Selected);

				/// <summary>
				/// Renders the explorer resource in the UI in a grid way.
				/// </summary>
				/// <param name="a_Clicked">Indicates if the resource was clicked.</param>
				/// <param name="a_RightClicked">Indicates if the resource was right-clicked.</param>
				/// <param name="a_DoubleClicked">Indicates if the resource was double-clicked.</param>
				/// <param name="a_Selected">Indicates if the resource is selected.</param>
				void RenderGrid(float a_Width, float a_Height, bool& a_Clicked, bool& a_RightClicked, bool& a_DoubleClicked, bool a_Selected);

				/// <summary>
				/// Renders the explorer resource in the UI in the tree.
				/// </summary>
				/// <param name="a_Clicked">Indicates if the resource was clicked.</param>
				/// <param name="a_Selected">Indicates if the resource is selected.</param>
				/// <param name="a_Indent">The amount of indents.</param>
				/// <param name="a_InitialPos">The starting position for 0 indent.</param>
				void RenderTree(bool& a_Clicked, bool a_Selected, int a_Indent, const ImVec2& a_InitialPos);

				/// <summary>
				/// Renders the resource icon or image in the hierarchy UI.
				/// </summary>
				virtual void RenderIcon();

				/// <summary>
				/// Renders the resource icon or image in the hierarchy UI.
				/// </summary>
				virtual void RenderIconGrid(float a_Width, float a_Height);

				/// <summary>
				/// Logic for handling double-clicks on the resource.
				/// Override to provide specific behavior.
				/// </summary>
				virtual void DoubleClicked()
				{};

/// <summary>
/// Renders base UI elements for the resource in the inspector.
/// </summary>
/// <param name="a_Resource">Pointer to the explorer resource.</param>
				virtual void RenderBaseSelectable(ExplorerResource* a_Resource);

				/// <summary>
				/// Finalizes rendering of the base selectable UI.
				/// </summary>
				virtual void EndBaseSelectable();

				/// <summary>
				/// Checks if the resource has subfolders.
				/// Override to specify folder support for the resource.
				/// </summary>
				/// <returns>True if the resource has subfolders, otherwise false.</returns>
				virtual bool HasFolders() const;

				/// <summary>
				/// Retrieves the associated resource.
				/// Override to provide a specific resource.
				/// </summary>
				/// <returns>A pointer to the resource, or nullptr by default.</returns>
				virtual ExplorerResource* GetResource()
				{
					return nullptr;
				}

			  /// <summary>
			  /// Clears all child resources associated with this view.
			  /// </summary>
				void ClearChildren();

				/// <summary>
				/// Retrieves the parent resource view, if applicable.
				/// </summary>
				/// <returns>A pointer to the parent resource view.</returns>
				ExplorerResourceUIView* GetParent() const;

				/// <summary>
				/// Retrieves a vector containing children views.
				/// </summary>
				/// <returns>A vector with pointers to the resource's children.</returns>
				std::vector<ExplorerResourceUIView*>& GetChildren();

				/// <summary>
				/// Retrieves the explorer resource ui type.
				/// </summary>
				/// <returns>The type of resource ui type.</returns>
				ExplorerResourceUIType GetExplorerResourceUIType() const;

				/// <summary>
				/// Sets the explorer resource ui type.
				/// </summary>
				void SetExplorerResourceUIType(ExplorerResourceUIType a_ExplorerResourceUIType);

				/// <summary>
				/// Retrieves the explorer resource type.
				/// </summary>
				/// <returns>The type of resource ui type.</returns>
				ExplorerResourceType GetExplorerResourceType() const;

				/// <summary>
				/// Sets the explorer resource type.
				/// </summary>
				void SetExplorerResourceType(ExplorerResourceType a_ExplorerResourceType);
			protected:
				/// <summary>
				/// A vector of child resource views.
				/// </summary>
				std::vector<ExplorerResourceUIView*> m_Resources;

				/// <summary>
				/// The parent resource view, if applicable.
				/// </summary>
				ExplorerResourceUIView* m_Parent = nullptr;

				/// <summary>
				/// Creates child views based on the explorer resource that has been passed.
				/// </summary>
				/// <param name="a_ResourceView">Pointer to the resource view that needs to be changed.</param>
				/// <param name="a_Resource">Pointer to the explorer resource.</param>
				/// <param name="a_Window">The ImGui window for rendering the view.</param>
				void CreateExplorerResourceUIViewChild(ExplorerResourceUIView* a_ResourceView, ExplorerResource* a_Resource, const ImGuiWindow& a_Window);

				/// <summary>
				/// Sets the data for the resource view.
				/// Override to specify resource data initialization.
				/// </summary>
				/// <param name="a_Resource">Pointer to the explorer resource.</param>
				virtual void SetData(ExplorerResource* a_Resource);

				std::string m_Name; /// The name of the resource.
				std::string m_Icon; /// The icon of the resource.
				ExplorerResourceType m_ResourceType; /// The resource type of the resource.
				std::string m_StrResourceType; /// This is necessary because we do not have access to the resource in this class but do want to display this data.
				ExplorerResourceUIType m_ExplorerResourceUIType = ExplorerResourceUIType::ExplorerResourceType_Resource; /// The type of UI resource. Can be "../" or a resource.

				bool m_FoldedOut = false; /// Whether the node is folded out.

				StringTextInput m_NameInput; /// Inspector input for editing the explorer item's name.
			};
		}
	}
}

#endif // __EDITOR__