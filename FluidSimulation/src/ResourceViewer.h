/*
 * Default objects
 */

#ifndef RC_RESOURCE_VIEWER_H
#define RC_RESOURCE_VIEWER_H

/*---------------------------------------------------------------------------*/

class ResourceViewer : public KeyListener {

	public:
	class View {
		public:
			View() : current(0) {}
			virtual ~View() {}
			virtual void draw() {}
			virtual void toggleMode() {}
			void next();
			void prev();
		protected:
			void fixCurrent(int n);
			void printString(char *str);
		protected:
			ResourceViewer *viewer;
			i32 current;
			i32 cx, cy;
	};
	class TextureView : public View {
		public:
			TextureView();
			virtual ~TextureView() {}
			void draw();
			void toggleMode() {viewSeparatedChannels = !viewSeparatedChannels;}
		private:
			std::vector<Resource *> textureList;
			ShaderProgram *textureShader;
			VertexArray *fullscreenVA;
			bool viewSeparatedChannels;
	};
	class VertexArrayView : public View {
		public:
			VertexArrayView();
			virtual ~VertexArrayView() {}
			void draw();
			void toggleMode() {viewNormals = !viewNormals;}
		private:
			std::vector<Resource *> vaList;
			ShaderProgram *vaShader;
			Camera *cam;
			Group *camPivot;
			bool viewNormals;
	};

	public:
		ResourceViewer();
		~ResourceViewer();

	public:
		void draw();
		void nextResource();
		void prevResource();

		void consumeKeyEvent(const KeyEvent *e);

	private:
		
		VertexArrayView *vaView;
		TextureView *texView;

		std::vector<View *> views;
		i32 currentView;

		typedef enum {
			VIEW_OFF,
			VIEW_VA,
			VIEW_TEX,
			VIEW_LAST
		} ResourceViewState;
		ResourceViewState state;

};

#endif /* RC_RESOURCE_VIEWER_H */

