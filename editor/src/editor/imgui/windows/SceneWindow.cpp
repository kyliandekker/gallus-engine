#ifdef _EDITOR

#include "editor/imgui/windows/SceneWindow.h"

#include <imgui/imgui_helpers.h>
#include <imgui/ImGuizmo.h>

#include "editor/imgui/font_icon.h"
#include "editor/imgui/ImGuiWindow.h"
#include "core/Engine.h"
#include "graphics/dx12/Texture.h"
#include "gameplay/systems/TransformSystem.h"

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			SceneWindow::SceneWindow(ImGuiWindow& a_Window) : BaseWindow(a_Window, ImGuiWindowFlags_NoCollapse, std::string(font::ICON_SCENE) + " Scene", "Scene")
			{}

			void SceneWindow::Render()
			{
				ImVec2 initialPos = ImGui::GetCursorPos();

				ImVec2 toolbarSize = ImVec2(ImGui::GetContentRegionAvail().x, m_Window.GetHeaderSize().y);
				ImGui::BeginToolbar(toolbarSize);

				float x = ImGui::GetContentRegionAvail().x / 2;
				x -= (toolbarSize.y / 2);
				x -= (toolbarSize.y / 2);

				ImGui::SetCursorPosX(x);

				//bool m_Started = core::ENGINE.GetECS().HasStarted(), m_Paused = core::ENGINE.GetECS().IsPaused();
				//if (ImGui::TransparentCheckboxButton(IMGUI_FORMAT_ID(font::ICON_PLAY, BUTTON_ID, "PLAYSTOP_SCENE").c_str(), &m_Started, ImVec2(toolbarSize.y, toolbarSize.y)))
				//{
				//	core::ENGINE.GetECS().SetStarted(m_Started);
				//}
				//ImGui::SameLine();

				//if (ImGui::TransparentCheckboxButton(IMGUI_FORMAT_ID(font::ICON_PAUSE, BUTTON_ID, "PAUSE_SCENE").c_str(), &m_Paused, ImVec2(toolbarSize.y, toolbarSize.y)))
				//{
				//	core::ENGINE.GetECS().SetPaused(m_Paused);
				//}

				ImGui::EndToolbar(ImVec2(ImGui::GetStyle().ItemSpacing.x, 0));

#ifdef _EDITOR
#ifdef _RENDER_TEX
				ImGui::SetCursorPos(ImVec2(initialPos.x, initialPos.y + toolbarSize.y));

				ImVec2 availableSize = ImGui::GetContentRegionAvail();
				ImVec2 textureSize = ImVec2(core::ENGINE.GetDX12().GetRenderTexture()->GetSize().x,
					core::ENGINE.GetDX12().GetRenderTexture()->GetSize().y);

				float textureAspect = textureSize.x / textureSize.y;
				float regionAspect = availableSize.x / availableSize.y;

				// UV coordinates for cropping
				ImVec2 uv0(0.0f, 0.0f);
				ImVec2 uv1(1.0f, 1.0f);

				if (textureAspect > regionAspect)
				{
					float newWidth = regionAspect / textureAspect;
					uv0.x = (1.0f - newWidth) * 0.5f;
					uv1.x = 1.0f - uv0.x;
				}
				else
				{
					float newHeight = textureAspect / regionAspect;
					uv0.y = (1.0f - newHeight) * 0.5f;
					uv1.y = 1.0f - uv0.y;
				}

				//// Render the cropped image
				ImGui::Image((ImTextureID) core::ENGINE.GetDX12().GetRenderTexture()->GetGPUHandle().ptr,
					availableSize, uv0, uv1);

				ImGui::SetCursorPos(ImVec2(initialPos.x, initialPos.y + toolbarSize.y));

				EntityUIView* entity = dynamic_cast<EntityUIView*>(core::ENGINE.GetEditor().GetSelectable());
				if (entity)
				{
					if (!core::ENGINE.GetECS().GetSystem<gameplay::TransformSystem>().ContainsID(entity->GetEntityID()))
					{
						return;
					}

					auto transformComponent = core::ENGINE.GetECS().GetSystem<gameplay::TransformSystem>().GetComponent(entity->GetEntityID());

					ImGuizmo::Enable(true);
					ImGuizmo::SetOrthographic(false); // Use perspective mode
					ImGuizmo::SetDrawlist();
					ImVec2 windowPos = ImGui::GetWindowPos();

					// Adjust ImGuizmo's rect to match the image cropping area
					ImGuizmo::SetRect(windowPos.x + initialPos.x, windowPos.y + initialPos.y + toolbarSize.y, availableSize.x, availableSize.y);

					// Get transformation matrices
					DirectX::XMMATRIX objectMat = transformComponent.Transform().GetWorldMatrix();
					DirectX::XMMATRIX viewMat = core::ENGINE.GetDX12().GetCamera()->GetViewMatrix();
					DirectX::XMMATRIX projMat = core::ENGINE.GetDX12().GetCamera()->GetProjectionMatrix();

					// Convert DirectX matrices to float[16] format for ImGuizmo
					float gizmoMatrix[16];
					DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(gizmoMatrix), objectMat);

					// Choose the type of transformation
					static ImGuizmo::OPERATION currentOperation = ImGuizmo::TRANSLATE;
					if (ImGui::IsKeyPressed(ImGuiKey_T) || ImGui::IsKeyPressed(ImGuiKey_P)) currentOperation = ImGuizmo::TRANSLATE;
					if (ImGui::IsKeyPressed(ImGuiKey_R)) currentOperation = ImGuizmo::ROTATE;
					if (ImGui::IsKeyPressed(ImGuiKey_S)) currentOperation = ImGuizmo::SCALE;

					if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && ImGui::IsWindowHovered() && ImGui::IsWindowFocused())
					{
						TEST("Mouse clicked within window, should interact with gizmo");
					}

					// Render the gizmo (check if manipulation occurred)
					if (ImGuizmo::Manipulate(reinterpret_cast<float*>(&viewMat),
						reinterpret_cast<float*>(&projMat),
						currentOperation,
						ImGuizmo::LOCAL,
						gizmoMatrix))
					{
						TEST("Gizmo manipulated!");
						objectMat = XMLoadFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(gizmoMatrix));
						transformComponent.Transform().SetWorldMatrix(objectMat);
					}
				}

#endif // _RENDER_TEX
#endif // _EDITOR

				//ImGui::SetCursorPosY(y);
				//std::string fpsValue = std::to_string(static_cast<uint64_t>(std::round(core::ENGINE.GetDX12().GetFPS().GetFPS()))) + "fps";

				//ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - (ImGui::CalcTextSize(fpsValue.c_str()).x + m_Window.GetWindowPadding().x));
				//ImGui::TextColored(ImVec4(1, 1, 0, 1), fpsValue.c_str());
			}
		}
	}
}

#endif // _EDITOR