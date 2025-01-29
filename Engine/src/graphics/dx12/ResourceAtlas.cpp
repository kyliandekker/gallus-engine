#include "graphics/dx12/ResourceAtlas.h"

#include <algorithm>

#include "graphics/dx12/Texture.h"
#include "graphics/dx12/Mesh.h"

namespace coopscoop
{
	namespace graphics
	{
		namespace dx12
		{
			template<class T>
			T& ResourceAtlas::GetResource(std::vector<T*>& a_Vector, const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
			{
				std::wstring name = std::wstring(a_Path.begin(), a_Path.end());
				T* res = nullptr;
				for (size_t i = 0; i < a_Vector.size(); i++)
				{
					if (a_Vector[i] && a_Vector[i]->GetName() == name)
					{
						res = a_Vector[i];
						break;
					}
					if (!a_Vector[i])
					{
						break;
					}
				}
				if (!res)
				{
					for (size_t i = 0; i < a_Vector.size(); i++)
					{
						T* texture = a_Vector[i];
						if (!a_Vector[i])
						{
							a_Vector[i] = new T();
							a_Vector[i]->Load(a_Path, a_CommandList);

							res = a_Vector[i];
							break;
						}
					}
				}
				if (!res)
				{
					res = a_Vector[MISSING];
				}
				return *res;
			}

			Mesh& ResourceAtlas::LoadMesh(const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
			{
				return GetResource(m_Meshes, a_Path, a_CommandList);
			}

			Texture& ResourceAtlas::LoadTexture(const std::string& a_Path, Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList2> a_CommandList)
			{
				return GetResource(m_Textures, a_Path, a_CommandList);
			}

			Shader& ResourceAtlas::LoadShader(const std::string& a_Path)
			{
				return GetResource(m_Shaders, a_Path, nullptr);
			}
		}
	}
}