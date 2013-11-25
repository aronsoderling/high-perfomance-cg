#include "RenderChimp.h"
#include "ResourceViewer.h"

#ifdef RC_MACOSX
#define KEY_VA Key3
#define KEY_TEX Key2
#else
#define KEY_VA KeyF3
#define KEY_TEX KeyF2
#endif

ResourceViewer::ResourceViewer() : KeyListener()
{
	texView = new TextureView();
	views.push_back(texView);
	vaView = new VertexArrayView();
	views.push_back(vaView);
	currentView = 0;
	state = VIEW_OFF;
}

ResourceViewer::~ResourceViewer()
{

}

void ResourceViewer::draw()
{
	if (state == VIEW_OFF)
		return;
	View *view = views[currentView];
	view->draw();
}

void ResourceViewer::nextResource()
{
	View *view = views[currentView];
	view->next();
}

void ResourceViewer::prevResource()
{
	View *view = views[currentView];
	view->prev();
}

void ResourceViewer::View::next()
{
	current++;
}

void ResourceViewer::View::prev()
{
	current--;
}

void ResourceViewer::View::fixCurrent(int n)
{
	while (current < 0)
		current += n;
	if (current >= n)
		current %= n;
}

void ResourceViewer::View::printString(char *str)
{
	char buf[1024];
	sprintf(buf, "~c0009%s", str);
	Console::text->print(buf, cx + 1, cy - 1);
	sprintf(buf, "~c2229%s", str);
	Console::text->print(buf, cx + 1, cy);
	Console::text->print(buf, cx - 1, cy);
	Console::text->print(buf, cx, cy + 1);
	Console::text->print(buf, cx, cy - 1);
	Console::text->print(str, cx, cy);
	cy -= Console::text->getGlyphHeight() + 4;
}

void ResourceViewer::consumeKeyEvent(const KeyEvent *e)
{
	if (e->getAction() != KeyPress)
		return ;
	if (Console::isShowing())
		return;
	if (e->getKey() == KEY_TEX) {
		currentView = 0;
		if (state == VIEW_TEX)
			state = VIEW_OFF;
		else
			state = VIEW_TEX;
	} else if (e->getKey() == KEY_VA) {
		currentView = 1;
		if (state == VIEW_VA)
			state = VIEW_OFF;
		else
			state = VIEW_VA;
	} else if (e->getKey() == KeyLeftArrow)
		prevResource();
	else if (e->getKey() == KeyRightArrow)
		nextResource();
    else if (e->getKey() == KeyUpArrow)
        for (int i=0;i<10;++i)
            nextResource();
    else if (e->getKey() == KeyDownArrow)
        for (int i=0;i<10;++i)
            prevResource();
	else if (e->getKey() == KeySpace)
		views[currentView]->toggleMode();
}

/*---------------------------------------------------------------------------*/

ResourceViewer::VertexArrayView::VertexArrayView() : View()
{
	//vaShader = SceneGraph::createShaderProgram("ResViewVtxSP", "ResourceViewer/ViewVA");
	vaShader = SceneGraph::createShaderProgram("ResViewVtxSP", 0, "ResourceViewer/ViewVA.vs", "ResourceViewer/ViewVA.fs", 0);
	cam = SceneGraph::createCamera("ResViewVtxCam");
	cam->setPerspectiveProjection(fPI * 0.5f, 4.0f / 3.0f, 0.1f, 1000.0f);
	camPivot = SceneGraph::createGroup("ResViewVtxCamPvt");
	camPivot->attachChild(cam);
	viewNormals = false;
}

void ResourceViewer::VertexArrayView::draw()
{
	Renderer::clearColor(vec4f(0.0f, 0.1f, 0.2f, 1.0));
	Renderer::clearDepth(1.0f);
	SceneGraph::getResourceList<VertexArray>(vaList);
	if (vaList.empty())
		return;
	fixCurrent(vaList.size());
	VertexArray *v = (VertexArray *) vaList[current];
	//textureShader->setTexture("VTexture", t);

	Attribute_t *a = v->getAttribute("Vertex");
	bool nrm = false;
	bool vtxFound = false;
	if (a && a->type == ATTRIB_FLOAT32 && a->length == 3) {
		vtxFound = true;
		AABox aabb;
		v->getBoundingBox(&aabb);
		vaShader->setValue("minPos", aabb.minCorner);
		vaShader->setValue("maxPos", aabb.maxCorner);
		//f32 radius = smax2<f32>(-aabb.minCorner.minElement(), aabb.maxCorner.maxElement());
		camPivot->rotateY(Platform::getFrameTimeStep() * 0.75f);
		camPivot->setTranslate(aabb.centrum());
		f32 radius = (aabb.maxCorner - aabb.minCorner).length() * 1.0f;
		cam->setTranslate(0.0, 0.0, radius);
		u32 w, h;
		Platform::getDisplaySize(&w, &h);
		cam->setPerspectiveProjection(fPI * 0.5f, f32(w) / f32(h), radius * 0.01f, radius * 10.0f);
		Renderer::setCameraMatrices(cam);
		a = v->getAttribute("Normal");
		nrm = (a && viewNormals);
		vaShader->setValue("hasNormals", nrm ? 1 : 0);
		mat4f wm = mat4f::identityMatrix();
		vaShader->setValue("faded", 0.4f);
		vaShader->getRenderState()->disableCulling();
		Renderer::render(*v, &wm, &wm, 0, vaShader);
		vaShader->getRenderState()->enableCulling(CULL_BACK);
		Renderer::clearDepth(1.0f);
		vaShader->setValue("faded", 1.0f);
		Renderer::render(*v, &wm, &wm, 0, vaShader);
	}

	char buf[1024];
	u32 w, h;
	Platform::getDisplaySize(&w, &h);

	cy = (i32) (h / 2);
	cx = 20;

	sprintf(buf, "Viewing vertex array resources (%s)", nrm ? "Normal" : "Position");
	printString(buf);
	cy -= 5;
	if (!vtxFound) {
		sprintf(buf, "(Not displayed - only displaying VA's with a vec3 \"Vertex\" attribute).");
		printString(buf);
	}
	sprintf(buf, "Identifier: %s", v->getIdentifier());
	printString(buf);
	sprintf(buf, "References: %d", v->getReferenceCount());
	printString(buf);
	sprintf(buf, "# of Attribute struct members: %d", v->getAttributeCount());
	printString(buf);
	for (u32 i = 0; i < v->getAttributeCount(); i++) {
		a = v->getAttribute(i);
		sprintf(buf, "  (%d): %s", i, a->name);
		printString(buf);
	}
	sprintf(buf, "# of Indices: %d", v->getIndexLength());
	printString(buf);
	sprintf(buf, "# of Vertices: %d", v->getLength());
	printString(buf);
	/*
	sprintf(buf, "Width: %d", t->getWidth());
	printString(buf);
	sprintf(buf, "Height: %d", t->getHeight());
	printString(buf);
	sprintf(buf, "Format: %d", t->getFormat());
	printString(buf);
	sprintf(buf, "Mipmaps: %s", t->hasMipMaps() ? "Yes" : "No");
	printString(buf);
	*/
}

/*---------------------------------------------------------------------------*/

ResourceViewer::TextureView::TextureView() : View()
{
	//textureShader = SceneGraph::createShaderProgram("ResViewTexSP", "ResourceViewer/ViewTexture");
	textureShader = SceneGraph::createShaderProgram("ResViewTexSP", 0, "ResourceViewer/ViewTexture.vs", "ResourceViewer/ViewTexture.fs", 0);
	textureShader->getRenderState()->disableDepthTest();
	textureShader->getRenderState()->disableDepthWrite();
	textureShader->getRenderState()->disableFrustumCulling();
	fullscreenVA = SceneGraph::createFullScreenVertexArray("ResViewTexVA");
	viewSeparatedChannels = false;
}

void ResourceViewer::TextureView::draw()
{
	SceneGraph::getResourceList<Texture>(textureList);
	if (textureList.empty())
		return;
	fixCurrent(textureList.size());
	Texture *t = (Texture *) textureList[current];
	textureShader->setTexture("VTexture", t);
	i32 channel = 0;
	switch (t->getFormat()) {
		case TEXTURE_FORMAT_RGBA8:		channel = 1 | 2 | 4 | 8; break;
		case TEXTURE_FORMAT_RGB8:		channel = 1 | 2 | 4    ; break;
		case TEXTURE_FORMAT_GRAY8:		channel = 1 | 2 | 4    ; break;
		case TEXTURE_FORMAT_RGBA16:		channel = 1 | 2 | 4 | 8; break;
		case TEXTURE_FORMAT_RGB16:		channel = 1 | 2 | 4    ; break;
		case TEXTURE_FORMAT_GRAY16:		channel = 1 | 2 | 4    ; break;
		default: break;
	}
	textureShader->setValue("channel", channel);
	textureShader->setValue("split", viewSeparatedChannels ? 1 : 0);
	Renderer::render(*fullscreenVA, textureShader);

	char buf[1024];
	u32 w, h;
	Platform::getDisplaySize(&w, &h);

	cy = (i32) (h / 2);
	cx = 20;

	printString("Viewing texture resources");
	cy -= 5;
	sprintf(buf, "Identifier: %s", t->getIdentifier());
	printString(buf);
	sprintf(buf, "References: %d", t->getReferenceCount());
	printString(buf);
	sprintf(buf, "Width: %d", t->getWidth());
	printString(buf);
	sprintf(buf, "Height: %d", t->getHeight());
	printString(buf);
	sprintf(buf, "Format: %d", t->getFormat());
	printString(buf);
	sprintf(buf, "Mipmaps: %s", t->hasMipMaps() ? "Yes" : "No");
	printString(buf);
}

/*---------------------------------------------------------------------------*/

