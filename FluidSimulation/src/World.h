
#ifndef RC_WORLD_H
#define RC_WORLD_H

/**
 * Defines a World node.
 */
class World : public Node {
	friend class SceneGraph;

/*----------------------------------------------------------[ Functions ]----*/

	protected:
		World(
				const char		*nname
			);

		~World();

	public:

		static NodeType nodeType() {return NODE_WORLD;}

		/**
		 * A camera must be set as the active camera, so that the scene is
		 * rendered from a specified point, with the cameras projection
		 * settings.
		 */
		void setActiveCamera(
				Camera		*camera
			);

		Camera *getActiveCamera();

		/**
		 * Returns the kNN nearest light sources from a given search volume.
		 *   Note: This may be pretty slow if there are many light since they
		 * are not organized in a BVH or anything like that yet - i.e. this is
		 * a linear search.
		 *
		 * WARNING: Untested function!
		 *
		 */
		void getKNNLights(
				Light		**light_list,
				u32			*n_lights_found,

				Sphere		*search_volume,
				u32			kNN

			);

		void logInfo();

	private:

		Node *duplicateSelf();

		void drawSelf();

		void getKNNLightsRec(
				Light		**light_list,

				Sphere		*search_volume,
				u32			kNN,

				u32			*n,
				f32			*max_dist,
				Node		*curr

			);

/*------------------------------------------------------------[ Members ]----*/

	private:
		Camera			*activeCamera;

};


#endif /* RC_WORLD_H */

