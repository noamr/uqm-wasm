#ifndef EM_PersistFilesystem_H
#define EM_PersistFilesystem_H
#ifdef EMSCRIPTEN
#if defined(__cplusplus)
extern "C" {
#endif
extern void EM_PersistFilesystem();
#if defined(__cplusplus)
}
#endif
#else
inline void EM_PersistFilesystem() {}
#endif
#endif /* EM_PersistFilesystem_H */
