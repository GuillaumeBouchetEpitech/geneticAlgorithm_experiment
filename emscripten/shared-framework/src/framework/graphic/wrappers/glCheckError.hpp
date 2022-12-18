
namespace debug {

void glCheckError(const char* file, unsigned int line, const char* expression);

} // namespace debug

#if 0

#define glCheck(expr)                                                          \
  {                                                                            \
    expr;                                                                      \
    debug::glCheckError(__FILE__, __LINE__, #expr);                            \
  }

#else

#define glCheck(expr) (expr)

#endif
