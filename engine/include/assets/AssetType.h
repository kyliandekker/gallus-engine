#pragma once

#include <string>

namespace coopscoop
{
	namespace assets
	{
		/// <summary>
		/// Represents the various types of assets used within the engine.
		/// </summary>
		enum class AssetType
		{
			Cfg,
			Scene,
			Material,
			Texture,
			Sprite,
			Font,
			Sound,
			Song,
			VO,
			Animation,
			Model,
			Prefab,
		};

		/// <summary>
		/// Converts an asset type enumeration value to its corresponding string representation.
		/// </summary>
		/// <param name="a_AssetType">The asset type to convert.</param>
		/// <returns>A string representing the specified asset type.</returns>
		inline std::string AssetTypeToString(AssetType a_AssetType)
		{
			switch (a_AssetType)
			{
				case AssetType::Cfg:
				{
					return "Config File";
				}
				case AssetType::Scene:
				{
					return "Scene File";
				}
				case AssetType::Material:
				{
					return "Material File";
				}
				case AssetType::Texture:
				{
					return "Texture File";
				}
				case AssetType::Sprite:
				{
					return "Sprite File";
				}
				case AssetType::Font:
				{
					return "Font File";
				}
				case AssetType::Sound:
				{
					return "Sound File";
				}
				case AssetType::Song:
				{
					return "Song File";
				}
				case AssetType::VO:
				{
					return "VO File";
				}
				case AssetType::Animation:
				{
					return "Animation File";
				}
				case AssetType::Model:
				{
					return "Model File";
				}
				case AssetType::Prefab:
				{
					return "Prefab File";
				}
				default:
				{
					return "";
				}
			}
		}
	}
}