
/*---------------------------------------------------------------------------*/

template <> NodeType nodeGetTypeOf<Camera>();//   {return NODE_CAMERA;}
template <> NodeType nodeGetTypeOf<Group>();//    {return NODE_GROUP;}
template <> NodeType nodeGetTypeOf<Light>();//    {return NODE_LIGHT;}
template <> NodeType nodeGetTypeOf<World>();//    {return NODE_WORLD;}
template <> NodeType nodeGetTypeOf<Geometry>();// {return NODE_GEOMETRY;}

/*---------------------------------------------------------------------------*/
template<class T>
void Node::getChildren(std::vector<T *> &children)
{
	T *t;
	Node *node = firstChild;
	while (node) {
		if ((t = dynamic_cast<T *>(node)))
			children.push_back(t);
		node = node->next;
	}
}

/*---------------------------------------------------------------------------*/

template<class T>
void Node::getChildrenRecursive(std::vector<T *> &children)
{
	getChildren<T>(children);
	Node *node = firstChild;
	while (node) {
		node->getChildrenRecursive<T>(children);
		node = node->next;
	}
}

/*---------------------------------------------------------------------------*/

template<class T>
void Node::getChildren(std::vector<T *> &children, TraversalCriteria &criteria)
{
	Node *node = firstChild;
	while (node) {
		if (criteria(*node))
			children.push_back(node);
		node = node->next;
	}
}

/*---------------------------------------------------------------------------*/

template<class T>
void Node::getChildrenRecursive(std::vector<T *> &children, TraversalCriteria &criteria)
{
	getChildren(children, criteria);
	Node *node = firstChild;
	while (node)
		node->getChildrenRecursive(children, criteria);
}

/*---------------------------------------------------------------------------*/




