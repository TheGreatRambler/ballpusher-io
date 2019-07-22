#include "Urho3DRenderInterface.h"
#include <RmlUi/Core.h>

#include <RmlUi/Core/Texture.h>
#include <RmlUi/Core/Vector3.h>
#include <Urho3D/Core/Context.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/IndexBuffer.h>
#include <Urho3D/Graphics/Material.h>
#include <Urho3D/Graphics/Renderer.h>
#include <Urho3D/Graphics/VertexBuffer.h>
#include <Urho3D/Math/Color.h>
#include <Urho3D/Math/Rect.h>
#include <Urho3D/Resource/ResourceCache.h>
#include <list>

using namespace Urho3D;

struct Urho3DCompiledGeometry {
	IndexBuffer* iBuff;
	VertexBuffer* vBuff;
	Texture2D* texture;
};

Urho3DRenderInterface::Urho3DRenderInterface(Context* context) {
	this->context = context;
	graphics = context->GetSubsystem<Graphics>();
	Renderer* renderer = context->GetSubsystem<Renderer>();

	noTextureVS = graphics->GetShader(Urho3D::VS, "Basic_VCol");
	noTexturePS = graphics->GetShader(Urho3D::PS, "Basic_VCol");

	diffTextureVS = graphics->GetShader(Urho3D::VS, "Basic_DiffVCol");
	diffTexturePS = graphics->GetShader(Urho3D::PS, "Basic_DiffVCol");
}

Urho3DRenderInterface::~Urho3DRenderInterface() {
}


// Called by Rocket when it wants to render geometry that it does not wish to optimise.
void Urho3DRenderInterface::RenderGeometry(Rml::Core::Vertex* RMLUI_UNUSED(vertices), int RMLUI_UNUSED(num_vertices), int* RMLUI_UNUSED(indices), int RMLUI_UNUSED(num_indices), const Rml::Core::TextureHandle RMLUI_UNUSED(texture), const Rml::Core::Vector2f& RMLUI_UNUSED(translation)) {
}

// Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
Rml::Core::CompiledGeometryHandle Urho3DRenderInterface::CompileGeometry(Rml::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, const Rml::Core::TextureHandle texture) {
	VertexBuffer* vBuff = new VertexBuffer(context);
	if (texture) {
		vBuff->SetSize(num_vertices, MASK_POSITION | MASK_COLOR | MASK_TEXCOORD1, true);
	} else {
		vBuff->SetSize(num_vertices, MASK_POSITION | MASK_COLOR, true);
	}

	float* dest = (float*) vBuff->Lock(0, num_vertices, true);
	for (int i = 0; i < num_vertices; ++i) {
		*dest++ = vertices[i].position.x;
		*dest++ = vertices[i].position.y;
		*dest++ = 0.f;

		Color color(vertices[i].colour.red / 255.f, vertices[i].colour.green / 255.f, vertices[i].colour.blue / 255.f, vertices[i].colour.alpha / 255.f);
		*((unsigned*) dest) = color.ToUInt();
		dest++;

		if (texture) {
			*dest++ = vertices[i].tex_coord.x;
			*dest++ = vertices[i].tex_coord.y;
		}
	}
	vBuff->Unlock();

	IndexBuffer* iBuff = new IndexBuffer(context);
	iBuff->SetSize(num_indices, true);

	// indices
	unsigned* indices_stream = (unsigned*) iBuff->Lock(0, num_indices, true);
	for (int i = 0; i < num_indices; ++i)
		indices_stream[i] = (unsigned int) indices[i];

	iBuff->Unlock();

	Texture2D* tex = texture ? textures.at(texture - 1) : NULL;

	Urho3DCompiledGeometry* geom = new Urho3DCompiledGeometry();
	geom->iBuff = iBuff;
	geom->vBuff = vBuff;
	geom->texture = tex;

	return (Rml::Core::CompiledGeometryHandle) geom;
}

// Called by Rocket when it wants to render application-compiled geometry.
void Urho3DRenderInterface::RenderCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry, const Rml::Core::Vector2f& translation) {
	Urho3DCompiledGeometry* geom = (Urho3DCompiledGeometry*) geometry;

	// Setup projection matrix
	Vector2 invScreenSize(1.0f / (float) graphics->GetWidth(), 1.0f / (float) graphics->GetHeight());
	Vector2 scale(2.0f * invScreenSize.x_, -2.0f * invScreenSize.y_);
	Vector2 offset(-1.0f, 1.0f);

	Matrix4 projection(Matrix4::IDENTITY);
	projection.m00_ = scale.x_;
	projection.m03_ = offset.x_;
	projection.m11_ = scale.y_;
	projection.m13_ = offset.y_;
	projection.m22_ = 1.0f;
	projection.m23_ = 0.0f;
	projection.m33_ = 1.0f;

	// Setup graphics rendering states
	graphics->ClearParameterSources();
	graphics->SetCullMode(CULL_CW);
	graphics->SetDepthTest(CMP_ALWAYS);
	graphics->SetDepthWrite(false);
	graphics->SetStencilTest(false);
	graphics->ResetRenderTargets();
	graphics->SetBlendMode(BLEND_ALPHA);

	// Bind buffers
	graphics->SetVertexBuffer(geom->vBuff);
	graphics->SetIndexBuffer(geom->iBuff);

	ShaderVariation* ps = 0;
	ShaderVariation* vs = 0;

	if (geom->texture) {
		graphics->SetTexture(0, geom->texture);
		ps = diffTexturePS;
		vs = diffTextureVS;
	} else {
		graphics->SetTexture(0, NULL);
		ps = noTexturePS;
		vs = noTextureVS;
	}

	// Apply translation
	Matrix3x4 trans(Matrix3x4::IDENTITY);
	trans.SetTranslation(Vector3(translation.x, translation.y, 0.f));

	// Pass shader params
	graphics->SetShaders(vs, ps);
	if (graphics->NeedParameterUpdate(SP_OBJECT, this))
		graphics->SetShaderParameter(VSP_MODEL, trans);
	if (graphics->NeedParameterUpdate(SP_CAMERA, this))
		graphics->SetShaderParameter(VSP_VIEWPROJ, projection);
	if (graphics->NeedParameterUpdate(SP_MATERIAL, this))
		graphics->SetShaderParameter(PSP_MATDIFFCOLOR, Color(1.0f, 1.0f, 1.0f, 1.0f));

	// Draw indexed geometry
	graphics->Draw(TRIANGLE_LIST, 0, geom->iBuff->GetIndexCount(), 0, geom->vBuff->GetVertexCount());
}

// Called by Rocket when it wants to release application-compiled geometry.
void Urho3DRenderInterface::ReleaseCompiledGeometry(Rml::Core::CompiledGeometryHandle geometry) {
	Urho3DCompiledGeometry* geom = (Urho3DCompiledGeometry*) geometry;
	delete geom->iBuff;
	delete geom->vBuff;
	delete geom;
}

// Called by Rocket when it wants to enable or disable scissoring to clip content.
void Urho3DRenderInterface::EnableScissorRegion(bool enable) {
	if (enable)
		graphics->SetScissorTest(true, scissors);
	else
		graphics->SetScissorTest(false);
}

// Called by Rocket when it wants to change the scissor region.
void Urho3DRenderInterface::SetScissorRegion(int x, int y, int width, int height) {
	scissors.left_ = x;
	scissors.top_ = y;
	scissors.bottom_ = height;
	scissors.right_ = width;
}

// Called by Rocket when a texture is required by the library.
bool Urho3DRenderInterface::LoadTexture(Rml::Core::TextureHandle& texture_handle, Rml::Core::Vector2i& texture_dimensions, const Rml::Core::String& source) {
	Texture2D* tex = context->GetSubsystem<ResourceCache>()->GetResource<Texture2D>(source.CString());
	if (tex) {
		texture_dimensions.x = tex->GetWidth();
		texture_dimensions.y = tex->GetHeight();
	}

	textures.push_back(tex);
	texture_handle = (Rml::Core::TextureHandle) textures.size(); // indexing from 1, 0 means no texture

	return true;
}

// Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
bool Urho3DRenderInterface::GenerateTexture(Rml::Core::TextureHandle& texture_handle, const Rml::Core::byte* source, const Rml::Core::Vector2i& source_dimensions) {
	SharedPtr<Image> img = SharedPtr<Image>(new Image(context));
	img->SetSize(source_dimensions.x, source_dimensions.y, 4);
	img->SetData(source);
	Texture2D* tex = new Texture2D(context);
	tex->SetData(img);

	textures.push_back(tex);
	texture_handle = (Rml::Core::TextureHandle) textures.size(); // indexing from 1, 0 means no texture

	return true;
}

// Called by Rocket when a loaded texture is no longer required.
void Urho3DRenderInterface::ReleaseTexture(Rml::Core::TextureHandle texture_handle) {
	Texture2D* tex = textures.at(texture_handle - 1);
	delete tex;
	tex = NULL;
}

float Urho3DRenderInterface::GetHorizontalTexelOffset() {
#ifdef USE_OPENGL
	return 0.f;
#else
	return 0.5f;
#endif
}

float Urho3DRenderInterface::GetVerticalTexelOffset() {
#ifdef USE_OPENGL
	return 0.f;
#else
	return 0.5f;
#endif
}
