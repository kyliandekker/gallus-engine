#pragma once

#ifdef _EDITOR

#include <vector>
#include <string>
#include <rapidjson/document.h>
#include <imgui/imgui.h>

#include "core/FileUtils.h"
#include "assets/AssetType.h"

namespace coopscoop
{
	namespace editor
	{
		enum class ExplorerResourceType
		{
			Folder,
			File,
		};

		class ExplorerResource
		{
			friend class AssetDatabase;
		public:
			virtual ~ExplorerResource();

			const fs::path& GetPath() const;
			ExplorerResourceType GetResourceType() const;
			ExplorerResource* GetParent() const;

			std::vector<ExplorerResource*> m_Resources;

			virtual bool Initialize() { return true; };

			bool Scan();
			bool Rename(const std::string& a_Name);
			void Delete();

			assets::AssetType GetAssetType() const;
			void SetAssetType(assets::AssetType a_AssetType);

			bool LoadMetadata();
			virtual bool ProcessMetadata(const rapidjson::Document& a_JsonFile);

			bool SaveMetadata() const;
			virtual rapidjson::Document PrepareMetadata() const;
		protected:
			fs::path m_Path;
			assets::AssetType m_AssetType;
			ExplorerResourceType m_ResourceType = ExplorerResourceType::Folder;
			ExplorerResource* m_Parent = nullptr;
		};

		class ConfigExplorerResource : public ExplorerResource
		{ };

		class MaterialExplorerResource : public ExplorerResource
		{ };

		class ImageExplorerResource : public ExplorerResource
		{
		public:
			virtual ~ImageExplorerResource() override;

			bool Initialize() override;
		};

		class TextureExplorerResource : public ImageExplorerResource
		{ };

		class SpriteExplorerResource : public ImageExplorerResource
		{ };

		class FontExplorerResource : public ImageExplorerResource
		{ };

		class AnimationExplorerResource : public ExplorerResource
		{ };

		class LocalizationExplorerResource : public ExplorerResource
		{ };

		class ModelExplorerResource : public ExplorerResource
		{ };

		class PrefabExplorerResource : public ExplorerResource
		{ };

		class SoundExplorerResource;
		class SongExplorerResource;
		class VOExplorerResource;
	}
}

#endif // _EDITOR