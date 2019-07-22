#include <RmlUi/Core/Context.h>
#include <RmlUi/Core/RenderInterface.h>
#include <Urho3D/Core/Object.h>
#include <Urho3D/Graphics/ShaderVariation.h>
#include <Urho3D/Graphics/Texture2D.h>
#include <Urho3D/Math/Rect.h>
#include <vector>

using namespace Urho3D;

/**
	Urho3D render interface for Rocket
 */
class Urho3DRenderInterface : public Rml::Core::RenderInterface {
public:
	Urho3DRenderInterface(Urho3D::Context* context);
	~Urho3DRenderInterface();

	/// Called by Rocket when it wants to render geometry that it does not wish to optimise.
	virtual void RenderGeometry(Rml::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::Core::TextureHandle texture, const Rml::Core::Vector2f& translation);

	/// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
	virtual Rml::Core::CompiledGeometryHandle CompileGeometry(Rml::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, Rml::Core::TextureHandle texture);

	/// Called by Rocket when it wants to render application-compiled geometry.
	virtual void RenderCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry, const Rml::Core::Vector2f& translation);
	/// Called by Rocket when it wants to release application-compiled geometry.
	virtual void ReleaseCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry);

	/// Called by Rocket when it wants to enable or disable scissoring to clip content.
	virtual void EnableScissorRegion(bool enable);
	/// Called by Rocket when it wants to change the scissor region.
	virtual void SetScissorRegion(int x, int y, int width, int height);

	/// Called by Rocket when a texture is required by the library.
	virtual bool LoadTexture(Rml::Core::TextureHandle& texture_handle, Rml::Core::Vector2i& texture_dimensions, const Rml::Core::String& source);
	/// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
	virtual bool GenerateTexture(Rml::Core::TextureHandle& texture_handle, const Rml::Core::byte* source, const Rml::Core::Vector2i& source_dimensions);
	/// Called by Rocket when a loaded texture is no longer required.
	virtual void ReleaseTexture(Rml::Core::TextureHandle texture_handle);

	/// Returns the native horizontal texel offset for the renderer.
	/// @return The renderer's horizontal texel offset. The default implementation returns 0.
	virtual float GetHorizontalTexelOffset();
	/// Returns the native vertical texel offset for the renderer.
	/// @return The renderer's vertical texel offset. The default implementation returns 0.
	virtual float GetVerticalTexelOffset();

private:
	std::vector<Texture2D*> textures;
	IntRect scissors;
	Context* context;
	Graphics* graphics;

	SharedPtr<ShaderVariation> noTextureVS;
	SharedPtr<ShaderVariation> noTexturePS;
	SharedPtr<ShaderVariation> diffTextureVS;
	SharedPtr<ShaderVariation> diffTexturePS;
};