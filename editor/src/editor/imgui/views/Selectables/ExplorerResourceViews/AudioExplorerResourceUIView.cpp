#ifdef _EDITOR

#include "editor/imgui/views/Selectables/ExplorerResourceViews/AudioExplorerResourceUIView.h"

#include <imgui/imgui_helpers.h>
#include <imgui/implot.h>

#include "editor/explorer_resources/AudioExplorerResource.h"
#include "editor/imgui/ImGuiDefines.h"
#include "editor/imgui/ImGuiWindow.h"

namespace gallus
{
	namespace editor
	{
		namespace imgui
		{
			template<class T>
			void AudioExplorerResourceUIView<T>::RenderIconGrid(float a_Width, float a_Height)
			{
				ImPlot::PushStyleColor(ImPlotCol_LegendBg, ImVec4(0, 0, 0, 0));
				ImPlot::PushStyleColor(ImPlotCol_FrameBg, ImVec4(0, 0, 0, 0));
				ImPlot::PushStyleColor(ImPlotCol_PlotBg, ImVec4(0, 0, 0, 0));
				ImPlot::PushStyleColor(ImPlotCol_PlotBorder, ImVec4(0, 0, 0, 0));
				ImPlot::PushStyleColor(ImPlotCol_AxisBg, ImVec4(0, 0, 0, 0));
				if (ImPlot::BeginPlot("Waveform_Left", ImVec2(a_Width, a_Height), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
				{
					if (m_Resource.m_LeftSamples != nullptr)
					{
						ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_Foreground | ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);
						ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_Foreground | ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);
						ImPlot::SetupAxisLimits(ImAxis_Y1, -0.5f, 0.5f, ImPlotCond_Always);
						ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(m_Resource.m_NumSamples));
						ImPlot::PlotLine("Waveform", m_Resource.m_LeftSamples, static_cast<int>(m_Resource.m_NumSamples));
					}

					ImPlot::EndPlot();
				}
				ImPlot::PopStyleColor();
				ImPlot::PopStyleColor();
				ImPlot::PopStyleColor();
				ImPlot::PopStyleColor();
				ImPlot::PopStyleColor();
			}

			template <class T>
			ExplorerResource* AudioExplorerResourceUIView<T>::GetResource()
			{
				return &m_Resource;
			}

			template <class T>
			void AudioExplorerResourceUIView<T>::RenderSelectable()
			{
				RenderBaseSelectable(&m_Resource);

				ImGui::DisplayHeader(m_Window.GetBoldFont(), "Type");
				ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(m_Window.GetFramePadding().x, 0));
				ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
				if (m_AssetTypeDropdown.Render(ImGui::IMGUI_FORMAT_ID("", COMBO_ID, "ASSETTYPE_INSPECTOR").c_str()))
				{
					m_Resource.SetAssetType(m_AssetTypeDropdown.GetValue());
					m_Resource.SaveMetadata();

					SetData(&m_Resource);
				}
				ImGui::PopStyleVar();

				EndBaseSelectable();

				RenderPreviewWindow();

				ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, ImVec2(0, 0));
				ImPlot::PushStyleVar(ImPlotStyleVar_LabelPadding, ImVec2(0, 0));
				ImPlot::PushStyleVar(ImPlotStyleVar_LegendPadding, ImVec2(0, 0));
				ImPlot::PushStyleVar(ImPlotStyleVar_LegendInnerPadding, ImVec2(0, 0));
				ImPlot::PushStyleVar(ImPlotStyleVar_LegendSpacing, ImVec2(0, 0));
				ImPlot::PushStyleVar(ImPlotStyleVar_MousePosPadding, ImVec2(0, 0));
				ImPlot::PushStyleVar(ImPlotStyleVar_AnnotationPadding, ImVec2(0, 0));
				ImPlot::PushStyleVar(ImPlotStyleVar_FitPadding, ImVec2(0, 0));
				ImPlot::PushStyleVar(ImPlotStyleVar_PlotDefaultSize, ImVec2(0, 0));
				ImPlot::PushStyleVar(ImPlotStyleVar_PlotMinSize, ImVec2(0, 0));

				float totalSize = 200 - (ImGui::GetStyle().ItemSpacing.y * 3);
				if (ImPlot::BeginPlot("Waveform_Left", ImVec2(ImGui::GetContentRegionAvail().x, totalSize / 2), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
				{
					if (m_Resource.m_LeftSamples != nullptr)
					{
						ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);
						ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);
						ImPlot::SetupAxisLimits(ImAxis_Y1, -0.5f, 0.5f, ImPlotCond_Always);
						ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(m_Resource.m_NumSamples));
						ImPlot::PlotLine("Waveform", m_Resource.m_LeftSamples, static_cast<int>(m_Resource.m_NumSamples));
					}

					ImPlot::EndPlot();
				}

				if (ImPlot::BeginPlot("Waveform_Right", ImVec2(ImGui::GetContentRegionAvail().x, totalSize / 2), ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs | ImPlotFlags_NoFrame))
				{
					if (m_Resource.m_RightSamples != nullptr)
					{
						ImPlot::SetupAxis(ImAxis_X1, "", ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);
						ImPlot::SetupAxis(ImAxis_Y1, "", ImPlotAxisFlags_LockMin | ImPlotAxisFlags_LockMax | ImPlotAxisFlags_NoTickMarks | ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoGridLines);
						ImPlot::SetupAxisLimits(ImAxis_Y1, -0.5f, 0.5f, ImPlotCond_Always);
						ImPlot::SetupAxisLimits(ImAxis_X1, 0, static_cast<double>(m_Resource.m_NumSamples));
						ImPlot::PlotLine("Waveform", m_Resource.m_RightSamples, static_cast<int>(m_Resource.m_NumSamples));
					}

					ImPlot::EndPlot();
				}
				ImPlot::PopStyleVar();
				ImPlot::PopStyleVar();
				ImPlot::PopStyleVar();
				ImPlot::PopStyleVar();
				ImPlot::PopStyleVar();
				ImPlot::PopStyleVar();
				ImPlot::PopStyleVar();
				ImPlot::PopStyleVar();
				ImPlot::PopStyleVar();
				ImPlot::PopStyleVar();

				EndPreviewWindow();
			}

			template <class T>
			void AudioExplorerResourceUIView<T>::SetData(ExplorerResource* a_Resource)
			{
				ExplorerResourceWithPreviewUIView::SetData(a_Resource);
				m_AssetTypeDropdown.Initialize(
					m_Resource.GetAssetType(),
					{
						assets::AssetType::Sound,
						assets::AssetType::Song,
						assets::AssetType::VO,
					},
					assets::AssetTypeToString
					);
			}

			template<class T>
			AudioExplorerResourceUIView<T>::AudioExplorerResourceUIView(ImGuiWindow& a_Window, T* a_Resource) : ExplorerResourceWithPreviewUIView(a_Window), m_Resource(*a_Resource), m_AssetTypeDropdown(a_Window)
			{}

			template <class T>
			AudioExplorerResourceUIView<T>::AudioExplorerResourceUIView<T>(const AudioExplorerResourceUIView& a_Other) : ExplorerResourceWithPreviewUIView(a_Other), m_Resource(a_Other.m_Resource), m_AssetTypeDropdown(a_Other.m_Window)
			{}

			SoundExplorerResourceUIView::SoundExplorerResourceUIView(ImGuiWindow& a_Window, SoundExplorerResource* a_Resource) : AudioExplorerResourceUIView(a_Window, a_Resource)
			{}

			SoundExplorerResourceUIView::SoundExplorerResourceUIView(const SoundExplorerResourceUIView& a_Other) : AudioExplorerResourceUIView(a_Other)
			{}

			SongExplorerResourceUIView::SongExplorerResourceUIView(ImGuiWindow& a_Window, SongExplorerResource* a_Resource) : AudioExplorerResourceUIView(a_Window, a_Resource)
			{}

			SongExplorerResourceUIView::SongExplorerResourceUIView(const SongExplorerResourceUIView& a_Other) : AudioExplorerResourceUIView(a_Other)
			{}

			VOExplorerResourceUIView::VOExplorerResourceUIView(ImGuiWindow& a_Window, VOExplorerResource* a_Resource) : AudioExplorerResourceUIView(a_Window, a_Resource)
			{}

			VOExplorerResourceUIView::VOExplorerResourceUIView(const VOExplorerResourceUIView& a_Other) : AudioExplorerResourceUIView(a_Other)
			{}
		}
	}
}

#endif // _EDITOR