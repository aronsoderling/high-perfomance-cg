
template<class T>
void Uniform::setValue(const char *name, T value)
{
	u32 flags = uniformMapFlag<T>();
	static const size_t typesize = sizeof(T);
	UniformEntry *entry = prepEntry(name, typesize, flags, 1);
	memcpy(entry->data, &value, typesize);
	entry->n = 1;
	entry->flags = flags;
	entry->size = typesize;
}

template<class T>
void Uniform::setValueArray(const char *name, T *list, u32 n_values)
{
	u32 flags = uniformMapFlag<T>();
	static const size_t typesize = sizeof(T);
	UniformEntry *entry = prepEntry(name, typesize, flags, n_values);
	memcpy(entry->data, list, typesize * n_values);
	entry->n = n_values;
	entry->flags = flags;
	entry->size = typesize;
}

template<class T>
void Uniform::setValueArray(const char *name, std::vector<T> &list)
{
	setValueArray<T>(name, &list[0], list.size());
}
