// (C) 2017-2019 NYSE | Intercontinental Exchange
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
//
// Contacting ICE: <https://www.theice.com/contact>
//
// Target: algo_lib (lib) -- Support library for all executables
// Exceptions: NO
// Header: include/algo/hook.h -- Callbacks (hooks)
//
// Created By: alexei.lebedev
// Recent Changes: alexei.lebedev
//

#pragma once

namespace algo {

    struct NotUsed {
    };

    template<class A,class B>  struct HookCheck                    { };
    template<class B>          struct HookCheck<NotUsed*,B>        { enum { value=1 }; };
    template<>                 struct HookCheck<NotUsed*,NotUsed*> { enum { value=1 }; };
    template<class T>          struct HookCheck<T,T>               { enum { value=1 }; typedef char size_check[1+8-int(sizeof(T))]; };
    template<class T>          struct HookCheck<T&,T*>             { enum { value=1 }; };
    template<class T>          struct HookCheck<float,T*>          { };
    template<class T>          struct HookCheck<double,T*>         { };
    template<>                 struct HookCheck<algo::SeqType,u64> { enum { value=1 }; };

    template<class B=NotUsed*, class C=NotUsed*, class D=NotUsed*> struct THook {
        void *ctx;
        typedef void (*HookFcn)(void*,B,C,D);
        HookFcn fcn;

        THook();
        THook(void *in_ctx, HookFcn in_fcn);

        // see hook.int.h
        template<class B2, class C2, class D2>
            THook(const THook<B2,C2,D2> &rhs)
            : ctx(rhs.ctx)
            , fcn(HookFcn(rhs.fcn))
        {
            (void) HookCheck<B2,B>::value;
            (void) HookCheck<C2,C>::value;
            (void) HookCheck<D2,D>::value;
        }
    };

    // construct a hook from a function and context pointer.
    // function may take either Ctx* or Ctx& as first argument.
    inline THook<>      Hook(void (*in_fcn)());

    // Construct hook from first argument and function pointer
    template<class Ctx>                          inline THook<>      Hook(Ctx *in_ctx, void (*in_fcn)(Ctx*)       );
    template<class Ctx>                          inline THook<>      Hook(Ctx *in_ctx, void (*in_fcn)(Ctx&)       );
    template<class Ctx,class B>                  inline THook<B>     Hook(Ctx *in_ctx, void (*in_fcn)(Ctx*,B)     );
    template<class Ctx,class B>                  inline THook<B>     Hook(Ctx *in_ctx, void (*in_fcn)(Ctx&,B)     );
    template<class Ctx,class B,class C>          inline THook<B,C>   Hook(Ctx *in_ctx, void (*in_fcn)(Ctx*,B,C)   );
    template<class Ctx,class B,class C>          inline THook<B,C>   Hook(Ctx *in_ctx, void (*in_fcn)(Ctx&,B,C)   );
    template<class Ctx,class B,class C, class D> inline THook<B,C,D> Hook(Ctx *in_ctx, void (*in_fcn)(Ctx*,B,C,D) );
    template<class Ctx,class B,class C, class D> inline THook<B,C,D> Hook(Ctx *in_ctx, void (*in_fcn)(Ctx&,B,C,D) );

    // Invoke with arguments. Assume hook is not NULL.
    template<class B>                   inline void qCall(const THook<B>     &hook, B arg1                        );
    template<class B, class C>          inline void qCall(const THook<B,C>   &hook, B arg1, C arg2                );
    template<class B, class C, class D> inline void qCall(const THook<B,C,D> &hook, B arg1, C arg2, D arg3        );

    // construct a hook from a function and context pointer.
    // function may take either Ctx* or Ctx& as first argument.
    inline THook<>      Hook(void (*in_fcn)());

    inline THook<> Hook(void (*in_fcn)());
    template<class Ctx> inline THook<> Hook(Ctx *in_ctx, void (*in_fcn)(Ctx*));
    template<class Ctx> inline THook<> Hook(Ctx *in_ctx, void (*in_fcn)(Ctx&));

    template<class Ctx,class B> inline THook<B> Hook(Ctx *in_ctx, void (*in_fcn)(Ctx*,B));
    template<class Ctx,class B> inline THook<B> Hook(Ctx *in_ctx, void (*in_fcn)(Ctx&,B));
    template<class Ctx,class B,class C> inline THook<B,C> Hook(Ctx *in_ctx, void (*in_fcn)(Ctx*,B,C));
    template<class Ctx,class B,class C> inline THook<B,C> Hook(Ctx *in_ctx, void (*in_fcn)(Ctx&,B,C));
    template<class Ctx,class B,class C, class D> inline THook<B,C,D> Hook(Ctx *in_ctx, void (*in_fcn)(Ctx*,B,C,D));
    template<class Ctx,class B,class C, class D> inline THook<B,C,D> Hook(Ctx *in_ctx, void (*in_fcn)(Ctx&,B,C,D));

    inline void qCall(const THook<> &hook);

    template<class B> inline void qCall(const THook<B> &hook, B arg1);
    template<class B, class C> inline void qCall(const THook<B,C> &hook, B arg1, C arg2);
    template<class B, class C, class D> inline void qCall(const THook<B,C,D> &hook, B arg1, C arg2, D arg3);
}
using algo::THook;
