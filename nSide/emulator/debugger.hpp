//debugging function hook:
//no overhead (and no debugger invocation) if not compiled with -DDEBUGGER
//wraps testing of function to allow invocation without a defined callback
template<typename T> struct debug_function;
#if defined(DEBUGGER)
  template<typename R, typename... P> struct debug_function<auto (P...) -> R> : function<auto (P...) -> R> {
    using super = function<auto (P...) -> R>;
    using super::function;

    auto operator()(P... p) const -> R {
      if(!super::operator bool()) return R();
      return super::operator()(forward<P>(p)...);
    }
  };
#else
  template<typename R, typename... P> struct debug_function<auto (P...) -> R> {
    template<typename... A> debug_function(A...) {}
    auto operator()(P... p) const -> R { return R(); }
  };
#endif
