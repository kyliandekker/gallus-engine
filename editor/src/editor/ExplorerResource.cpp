#ifdef _EDITOR

#include "editor/ExplorerResource.h"

#include <unordered_map>
#include <filesystem>
// GetFileAttributes, FILE_ATTRIBUTE_HIDDEN, FILE_ATTRIBUTE_SYSTEM
#include <wtypes.h>
#include <rapidjson/utils.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#include <imgui/imgui_helpers.h>

#include "editor/explorer_resources/SceneExplorerResource.h"
#include "editor/explorer_resources/AudioExplorerResource.h"
#include "utils/string_extensions.h"
#include "core/DataStream.h"
#include "core/logger/Logger.h"
#include "core/Engine.h"

// General meta-data Variables
#define JSON_ASSETTYPE_VAR "assetType"

namespace gallus
{
	namespace editor
	{
		fs::path getMetadataPath(const fs::path& a_Path)
		{
			return file::FileLoader::GetPath(a_Path.generic_string() + ".meta");
		}

		bool loadMetadata(const fs::path a_Path, rapidjson::Document& a_Document)
		{
			core::DataStream data;
			if (!file::FileLoader::LoadFile(getMetadataPath(a_Path), data))
			{
				return false;
			}

			a_Document.Parse(reinterpret_cast<char*>(data.data()), data.size());

			if (a_Document.HasParseError())
			{
				LOG(LOGSEVERITY_ERROR, "TODO", "Failed loading in meta file '%s'.", getMetadataPath(a_Path).c_str());
				return false;
			}

			return true;
		}

		bool saveMetadata(const rapidjson::Document& a_JsonFile, const fs::path& a_Path)
		{
			rapidjson::StringBuffer buffer;
			rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
			a_JsonFile.Accept(writer);

			return file::FileLoader::SaveFile(getMetadataPath(a_Path), core::Data(buffer.GetString(), buffer.GetSize()));
		}

		ExplorerResource::~ExplorerResource()
		{
			for (auto resource : m_Resources)
			{
				delete resource;
			}
		}

		const fs::path& ExplorerResource::GetPath() const
		{
			return m_Path;
		}

		ExplorerResourceType ExplorerResource::GetResourceType() const
		{
			return m_ResourceType;
		}

		ExplorerResource* ExplorerResource::GetParent() const
		{
			return m_Parent;
		}

		bool ExplorerResource::Scan()
		{
			// TODO: Fix
			const std::unordered_map<std::string, std::vector<assets::AssetType>> WAT =
			{
				{ ".cfg", { assets::AssetType::Cfg } },
				{ ".scene", { assets::AssetType::Scene } },
				{ ".mat", { assets::AssetType::Material } },
				{ ".png", { assets::AssetType::Texture, assets::AssetType::Sprite, assets::AssetType::Font } },
				{ ".bmp", { assets::AssetType::Texture, assets::AssetType::Sprite, assets::AssetType::Font } },
				{ ".wav", { assets::AssetType::Sound, assets::AssetType::Song } },
				{ ".anim", { assets::AssetType::Animation } },
				{ ".gltf", { assets::AssetType::Model } },
			};

			if (!fs::exists(m_Path))
			{
				return false;
			}

			m_ResourceType = fs::is_directory(m_Path) ? ExplorerResourceType::Folder : ExplorerResourceType::File;

			// Scan the folder.
			if (m_ResourceType == ExplorerResourceType::Folder)
			{
				// Clear all resources first.
				for (auto resource : m_Resources)
				{
					delete resource;
				}
				m_Resources.clear();

				// Go through each file/folder and check their status.
				fs::directory_iterator ds = fs::directory_iterator(m_Path, std::filesystem::directory_options::skip_permission_denied);
				for (const auto& dirEntry : ds)
				{
					// Check for hidden or system files. These should not be added.
					DWORD attributes = GetFileAttributes(dirEntry.path().generic_string().c_str());
					if (attributes & FILE_ATTRIBUTE_HIDDEN || attributes & FILE_ATTRIBUTE_SYSTEM)
					{
						continue;
					}

					// If it is not a directory, it is a file and needs to get past the meta checks.
					if (!fs::is_directory(dirEntry.path()))
					{
						std::string extension = dirEntry.path().extension().generic_string();

						// Get the extension. If the extension is not recognized, it will just be ignored.
						if (WAT.find(extension) == WAT.end())
						{
							continue;
						}

						ExplorerResource* resource = nullptr;

						auto it = WAT.find(extension);
						assets::AssetType assetType = it->second[0];

						rapidjson::Document document;
						bool hasMetadata = loadMetadata(dirEntry.path(), document);
						if (hasMetadata)
						{
							int iAssetType = 0;
							rapidjson::GetInt(document, JSON_ASSETTYPE_VAR, iAssetType);
							assetType = (assets::AssetType) iAssetType;
						}

						switch (assetType)
						{
							case assets::AssetType::Cfg:
							{
								resource = new ConfigExplorerResource();
								break;
							}
							case assets::AssetType::Scene:
							{
								resource = new SceneExplorerResource();
								break;
							}
							case assets::AssetType::Material:
							{
								resource = new MaterialExplorerResource();
								break;
							}
							case assets::AssetType::Texture:
							{
								resource = new TextureExplorerResource();
								break;
							}
							case assets::AssetType::Sprite:
							{
								resource = new SpriteExplorerResource();
								break;
							}
							case assets::AssetType::Font:
							{
								resource = new FontExplorerResource();
								break;
							}
							case assets::AssetType::Sound:
							{
								resource = new SoundExplorerResource();
								break;
							}
							case assets::AssetType::Song:
							{
								resource = new SongExplorerResource();
								break;
							}
							case assets::AssetType::VO:
							{
								resource = new VOExplorerResource();
								break;
							}
							case assets::AssetType::Animation:
							{
								resource = new AnimationExplorerResource();
								break;
							}
							case assets::AssetType::Model:
							{
								resource = new ModelExplorerResource();
								break;
							}
							case assets::AssetType::Prefab:
							{
								resource = new PrefabExplorerResource();
								break;
							}
						}

						if (resource != nullptr)
						{
							resource->m_Path = dirEntry.path();
							resource->m_Parent = this;
							resource->m_ResourceType = ExplorerResourceType::File;
							resource->m_AssetType = assetType;
							resource->Initialize();
							if (hasMetadata)
							{
								resource->ProcessMetadata(document);
							}
							else
							{
								resource->SaveMetadata();
							}
							m_Resources.push_back(resource);
						}
					}
					else
					{
						// Create the resource that will be added.
						ExplorerResource* explorerResource = new ExplorerResource();
						explorerResource->m_Path = dirEntry.path();
						explorerResource->m_Parent = this;
						explorerResource->m_ResourceType = ExplorerResourceType::Folder;

						m_Resources.push_back(explorerResource);
					}
				}
			}

			for (auto& resource : m_Resources)
			{
				resource->Scan();
			}

			return true;
		}

		// TODO: TEST.
		bool ExplorerResource::Rename(const std::string& a_Name)
		{
			fs::path newPath = file::FileLoader::GetPath(m_Path.parent_path().generic_string() + a_Name);

			std::string complete_name = a_Name;

			fs::rename(m_Path, newPath);
			m_Path = newPath;

			Scan();
			return false;
		}

		void ExplorerResource::Delete()
		{
			fs::remove(m_Path.c_str());
		}

		assets::AssetType ExplorerResource::GetAssetType() const
		{
			return m_AssetType;
		}

		void ExplorerResource::SetAssetType(assets::AssetType a_AssetType)
		{
			m_AssetType = a_AssetType;
		}

		bool ExplorerResource::LoadMetadata()
		{
			rapidjson::Document document;
			loadMetadata(m_Path, document);
			return ProcessMetadata(document);
		}

		bool ExplorerResource::ProcessMetadata(const rapidjson::Document& a_JsonFile)
		{
			int assetType = 0;
			rapidjson::GetInt(a_JsonFile, JSON_ASSETTYPE_VAR, assetType);
			m_AssetType = (assets::AssetType) assetType;

			return true;
		}

		bool ExplorerResource::SaveMetadata() const
		{
			rapidjson::Document document = PrepareMetadata();
			return saveMetadata(document, m_Path);
		}

		rapidjson::Document ExplorerResource::PrepareMetadata() const
		{
			rapidjson::Document document;
			document.SetObject();
			rapidjson::Document::AllocatorType& allocator = document.GetAllocator();

			int assetType = (int) m_AssetType;
			document.AddMember(JSON_ASSETTYPE_VAR, assetType, allocator);

			return document;
		}

		ImageExplorerResource::~ImageExplorerResource()
		{
			//if (m_DescHandle)
			//{
			//	m_DescHandle->Release();
			//}
		}

		bool ImageExplorerResource::Initialize()
		{
			//if (core::ENGINE.GetWindow().GetDX12Window().LoadTexture(m_Path, m_DescHandle))
			//{
			//	return true;
			//}

			return false;
		}
	}
}

#endif // _EDITOR