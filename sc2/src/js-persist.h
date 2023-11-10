#ifndef JS_PERSIST_H
#define JS_PERSIST_H
#ifdef EMSCRIPTEN
#if defined(__cplusplus)
extern "C" {
#endif
extern void js_persist();
#if defined(__cplusplus)
}
#endif
#else
inline void js_persist() {}
#endif
#endif /* JS_PERSIST_H */
