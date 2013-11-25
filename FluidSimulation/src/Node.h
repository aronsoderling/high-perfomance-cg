#ifndef RC_NODE_H
#define RC_NODE_H

/* Node types */
typedef enum {
	NODE_CAMERA = 0,
	NODE_GROUP,
	NODE_LIGHT,
	NODE_WORLD,
	NODE_GEOMETRY,
	NODE_TYPE_LAST
} NodeType;

class Node {
	friend class SceneGraph;
	friend class Transformable;
	friend class State;
	friend class Light;
	friend class Geometry;

	public:
		struct TraversalCriteria {
			virtual bool operator()(const Node &n) const = 0;
		};

		struct AlwaysTrue : public TraversalCriteria {
			bool operator()(const Node &n) const {return true;}
		};

		template<class T>
		struct Is : public TraversalCriteria {
			bool operator()(const Node &n) const {return n.getNodeType() == T::nodeType();}
		};

		/*
		template<>
		struct Is<Transformable> : public TraversalCriteria {
			bool operator()(const Node &n) const {return n.isTransformable();}
		};
		*/

/*----------------------------------------------------------[ Functions ]----*/

	protected:
		Node(
				const char	*nname,
				NodeType	ntype
			);

		/**
		 * Delete this node and its contents.
		 * This also recursively deletes the children of the node.
		 */
		virtual ~Node();

	public:
		/** Draw scene graph hierarchy, beginning with this node */
		void renderAll();

		/** Change the name of the node */
		void setName(
				char		*nname
			);

		/** Get the number of children */
		u32 getChildrenCount() const;

		/**
		 * Attach a child to this node. The child musn't already have parent.
		 * If it has, detach it first.
		 */
		void attachChild(
				Node		*node
			);

		/** Detaches the child node from this node. */
		void detachChild(
				Node		*node
			);

		/** Detach this node from its parent */
		void detachFromParent();

		/**
		 * Get the child on position "index". Returns 0 if no child is
		 * found.
		 */
		Node *getChild(
				u32			index
			);

		/**
		 * Search for a child with a certain name. Returns 0 if the child
		 * is not found.
		 */
		Node *getChild(
				char		*search_name,
				u32			max_depth
			);

		/** Get the next node of the same parent */
		Node *getNextSibling();

		/** Get a list of all children */
		template<class T>
		void getChildren(std::vector<T *> &children);

		/** Get a list of all children, recursively */
		template<class T>
		void getChildrenRecursive(std::vector<T *> &children);

		
		/** Get a list of all children that fulfills condition */
		template<class T>
		void getChildren(std::vector<T *> &children, TraversalCriteria &criteria);

		/** Get a list of all children that fulfills condition, recursively */
		template<class T>
		void getChildrenRecursive(std::vector<T *> &children, TraversalCriteria &criteria);


		/** Return parent node */
		Node *getParent();

		/**
		 * Mark this node as active or inactive.
		 * When drawing the scene graph, inactive nodes terminate the
		 * recursive traversal, so none of its children will be drawn either.
		 */
		void setActive(
				bool		active
			);

		/**
		 * Mark this node as visible or invisible.
		 * An invisible node is not drawn during a draw()-call, but the node's
		 * transforms are calculated and its children are drawn.
		 */
		void setVisible(
				bool		visible
			);

		/** Get the name of the node */
		char *getName();

		/** Get node type */
		NodeType getNodeType() const;

		/**
		 * Duplicates the node, which means that a new node is created, but all
		 * the resources are instantiated.
		 * Only the base node changes name to new_name, all its children will
		 * still have their old name.
		 */
		Node *duplicate(
				char		*new_name,
				bool		recursively		/* True = recursively duplicate all
											 * children as well
											 */
			);

		bool isTransformable() const;

		

//		void updateAndFreeze();

//		void unfreeze();

		virtual void logInfo();

	protected:

		/* Draw the current node */
		virtual void drawSelf() { }

		virtual Node *duplicateSelf();

		virtual void detached() { }

		static char *getNewNodeName(
				char			*buf,
				NodeType		type
			);

		void logNodeInfo();


/*------------------------------------------------------------[ Members ]----*/

	protected:
		char		*name;
		NodeType	type;

		Node		*parent;
		Node		*firstChild;
		Node		*next;
		u16			nChildren;

		bool		is_transformable;

		bool		broken;

	public:
		bool		isActive;		/* False = not traversed */
		bool		isVisible;		/* False = traversed but not drawn */

};

template <class T>
NodeType nodeGetTypeOf();



#include "Node.inl"

#endif /* RC_NODE_H */
