/*!
 * bigmath.
 * bd_context.cpp
 *
 * \date 09/15/2020
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */

#include "bigmath/bd_context.h"

#include "bigmath/errors.h"

#include <sstream>
#include <string>

/*****************************************************************************/
/*                                bd_context API                                */
/*****************************************************************************/

/* bd_context for exact calculations with (practically) unbounded precision. */
const bigmath::bd_context maxcontext{
    MPD_MAX_PREC,
    MPD_MAX_EMAX,
    MPD_MIN_EMIN,
    MPD_ROUND_HALF_EVEN,
    MPD_IEEE_Invalid_operation,
    0,
    0};

/* Used for default initialization of new contexts such as TLS contexts. */
bigmath::bd_context bigmath::context_template{
    18,                                                               /* prec */
    999999,                                                           /* emax */
    -999999,                                                          /* emin */
    MPD_ROUND_HALF_EVEN,                                              /* rounding */
    MPD_IEEE_Invalid_operation | MPD_Division_by_zero | MPD_Overflow, /* traps */
    0,                                                                /* clamp */
    1                                                                 /* allcr */
};

thread_local bigmath::bd_context bigmath::context{*bigmath::context_template.getconst()};

/* Factory function for creating a context for maximum unrounded arithmetic.*/
bigmath::bd_context bigmath::MaxContext() {
    return bd_context(maxcontext);
}

/* Factory function for creating IEEE interchange format contexts.*/
bigmath::bd_context bigmath::IEEEContext(int bits) {
    mpd_context_t ctx;

    if (mpd_ieee_context(&ctx, bits) < 0) {
        throw value_error("argument must be a multiple of 32, with a maximum of " +
                          std::to_string(MPD_IEEE_CONTEXT_MAX_BITS));
    }

    return bd_context(ctx);
}

typedef struct {
    const uint32_t flag;
    const char* name;
    const char* fqname;
    void (*const raise)(const std::string& msg);
} cmap;

template<typename T>
void raise(const std::string& msg) {
    throw T(msg);
}

const cmap signal_map[] = {
    {MPD_IEEE_Invalid_operation, "IEEE_invalid_operation", "bigmath::IEEE_invalid_operation", raise<bigmath::IEEE_invalid_operation>},
    {MPD_Division_by_zero, "division_by_zero", "bigmath::division_by_zero", raise<bigmath::division_by_zero>},
    {MPD_Overflow, "overflow_error", "bigmath::overflow_error", raise<bigmath::overflow_error>},
    {MPD_Underflow, "underflow_error", "bigmath::underflow_error", raise<bigmath::underflow_error>},
    {MPD_Subnormal, "subnormal_error", "bigmath::subnormal_error", raise<bigmath::subnormal_error>},
    {MPD_Inexact, "inexact_error", "bigmath::inexact_error", raise<bigmath::inexact_error>},
    {MPD_Rounded, "rounder_error", "bigmath::rounder_error", raise<bigmath::rounder_error>},
    {MPD_Clamped, "clamped_error", "bigmath::clamped_error", raise<bigmath::clamped_error>},
    {UINT32_MAX, NULL, NULL, NULL}};

const cmap cond_map[] = {
    {MPD_Invalid_operation, "invalid_operation_error", "bigmath::invalid_operation_error", raise<bigmath::invalid_operation_error>},
    {MPD_Conversion_syntax, "conversion_syntax_error", "bigmath::conversion_syntax_error", raise<bigmath::conversion_syntax_error>},
    {MPD_Division_impossible, "division_impossible_error", "bigmath::division_impossible_error", raise<bigmath::division_impossible_error>},
    {MPD_Division_undefined, "division_undefined_error", "bigmath::division_undefined_error", raise<bigmath::division_undefined_error>},
    {UINT32_MAX, NULL, NULL, NULL}};

std::string signals(const uint32_t flags) {
    std::string s;
    s.reserve(MPD_MAX_SIGNAL_LIST);

    s += "[";
    for (const cmap* c = signal_map; c->flag != UINT32_MAX; c++) {
        if (flags & c->flag) {
            if (s != "[") {
                s += ", ";
            }
            s += c->name;
        }
    }

    s += "]";

    return s;
}

std::string flags(const uint32_t flags) {
    std::string s;
    s.reserve(MPD_MAX_FLAG_LIST);

    s += "[";

    for (const cmap* c = cond_map; c->flag != UINT32_MAX; c++) {
        if (flags & c->flag) {
            if (s != "[") {
                s += ", ";
            }
            s += c->name;
        }
    }

    for (const cmap* c = signal_map + 1; c->flag != UINT32_MAX; c++) {
        if (flags & c->flag) {
            if (s != "[") {
                s += ", ";
            }
            s += c->name;
        }
    }

    s += "]";

    return s;
}

void bigmath::bd_context::raiseit(const uint32_t status) {
    if (status & MPD_Malloc_error) {
        throw malloc_error();
    }

    const std::string msg = flags(status);
    for (const cmap* c = cond_map; c->flag != UINT32_MAX; c++) {
        if (status & c->flag) {
            c->raise(msg);
        }
    }

    for (const cmap* c = signal_map + 1; c->flag != UINT32_MAX; c++) {
        if (status & c->flag) {
            c->raise(msg);
        }
    }

    throw runtime_error("internal_error: unknown status flag");
}

bigmath::bd_context::bd_context(mpd_ssize_t prec, mpd_ssize_t emax, mpd_ssize_t emin, int round, uint32_t traps, int clamp, int allcr) {
    this->prec(prec);
    this->emax(emax);
    this->emin(emin);
    this->traps(traps);
    this->round(round);
    this->clamp(clamp);
    this->allcr(allcr);
    this->ctx.status = 0;
    this->ctx.newtrap = 0;
}

bigmath::bd_context::bd_context(const bigmath::bd_context& c) noexcept {
    *this = c;
}

bigmath::bd_context::bd_context(const bigmath::bd_context&& c) noexcept {
    *this = c;
}

bigmath::bd_context& bigmath::bd_context::operator=(const bigmath::bd_context& c) noexcept {
    ctx = *c.getconst();
    return *this;
}

bigmath::bd_context& bigmath::bd_context::operator=(const bigmath::bd_context&& c) noexcept {
    ctx = *c.getconst();
    return *this;
}

bool bigmath::bd_context::operator==(const bigmath::bd_context& other) const noexcept {
    return ctx.prec == other.ctx.prec &&
           ctx.emax == other.ctx.emax &&
           ctx.emin == other.ctx.emin &&
           ctx.traps == other.ctx.traps &&
           ctx.status == other.ctx.status &&
           ctx.round == other.ctx.round &&
           ctx.clamp == other.ctx.clamp &&
           ctx.allcr == other.ctx.allcr &&
           ctx.newtrap == other.ctx.newtrap;
}

bool bigmath::bd_context::operator!=(const bigmath::bd_context& other) const noexcept {
    return !(*this == other);
}

mpd_context_t* bigmath::bd_context::get() {
    return &ctx;
}

const mpd_context_t* bigmath::bd_context::getconst() const {
    return &ctx;
}

mpd_ssize_t bigmath::bd_context::prec() const {
    return ctx.prec;
}

std::string bigmath::bd_context::repr() const {
    const int rounding = round();
    std::ostringstream ss;

    if (rounding < 0 || rounding >= MPD_ROUND_GUARD) {
        throw runtime_error("invalid rounding mode");
    }

    const char* round_str = mpd_round_string[rounding];

    ss << "bd_context(prec=" << prec() << ", "
       << "emax=" << emax() << ", "
       << "emin=" << emin() << ", "
       << "round=" << round_str << ", "
       << "clamp=" << clamp() << ", "
       << "traps=" << signals(traps()) << ", "
       << "status=" << signals(status()) << ")";

    return ss.str();
}

std::ostream& operator<<(std::ostream& os, const bigmath::bd_context& c) {
    os << c.repr();
    return os;
}
mpd_ssize_t bigmath::bd_context::emax() const {
    return ctx.emax;
}
mpd_ssize_t bigmath::bd_context::emin() const {
    return ctx.emin;
}
int bigmath::bd_context::round() const {
    return ctx.round;
}
uint32_t bigmath::bd_context::status() const {
    return ctx.status;
}
uint32_t bigmath::bd_context::traps() const {
    return ctx.traps;
}
int bigmath::bd_context::clamp() const {
    return ctx.clamp;
}
int bigmath::bd_context::allcr() const {
    return ctx.allcr;
}
int64_t bigmath::bd_context::etiny() const {
    return mpd_etiny(&ctx);
}
int64_t bigmath::bd_context::etop() const {
    return mpd_etop(&ctx);
}
