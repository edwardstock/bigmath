/*!
 * bigmath.
 * bd_context.h
 *
 * \date 09/15/2020
 * \author Eduard Maximovich (edward.vstock@gmail.com)
 * \link   https://github.com/edwardstock
 */
#ifndef BIGMATHPP_BD_CONTEXT_H
#define BIGMATHPP_BD_CONTEXT_H

#include "bigmath_config.h"
#include "mpdecimal_backport.h"
#include "utils.h"

namespace bigmath {

/******************************************************************************/
/*                              bd_context object                                */
/******************************************************************************/

class bd_context;

BIGMATHPP_API extern bd_context context_template;
BIGMATHPP_API extern thread_local bd_context context;

class BIGMATHPP_API bd_context {
private:
    mpd_context_t ctx;
    static void raiseit(const uint32_t status);

public:
    /* Constructors and destructors */
    bd_context(mpd_ssize_t prec = context_template.prec(),
               mpd_ssize_t emax = context_template.emax(),
               mpd_ssize_t emin = context_template.emin(),
               int round = context_template.round(),
               uint32_t traps = context_template.traps(),
               int clamp = context_template.clamp(),
               int allcr = context_template.allcr());

    bd_context(const bd_context& c) noexcept;
    bd_context(const bd_context&& c) noexcept;
    explicit bd_context(const mpd_context_t& c) noexcept {
        ctx = c;
    }
    ~bd_context() noexcept = default;

    bd_context& operator=(const bd_context& c) noexcept;
    bd_context& operator=(const bd_context&& c) noexcept;
    bool operator==(const bd_context& other) const noexcept;
    bool operator!=(const bd_context& other) const noexcept;

    /* get pointers to the full context */
    mpd_context_t* get();
    const mpd_context_t* getconst() const;

    /* get individual fields */
    mpd_ssize_t prec() const;
    mpd_ssize_t emax() const;
    mpd_ssize_t emin() const;

    /// \brief returns rounding mode
    /// \see bigmath::ROUND_*
    /// \return int
    int round() const;
    uint32_t status() const;
    uint32_t traps() const;
    int clamp() const;
    int allcr() const;
    int64_t etiny() const;
    int64_t etop() const;

    /* set individual fields */
    ALWAYS_INLINE void prec(mpd_ssize_t v) {
        if (!mpd_qsetprec(&ctx, v)) {
            throw value_error("valid range for prec is [1, MAX_PREC]");
        }
    }

    ALWAYS_INLINE void emax(mpd_ssize_t v) {
        if (!mpd_qsetemax(&ctx, v)) {
            throw value_error("valid range for emax is [0, MAX_EMAX]");
        }
    }

    ALWAYS_INLINE void emin(mpd_ssize_t v) {
        if (!mpd_qsetemin(&ctx, v)) {
            throw value_error("valid range for emin is [MIN_EMIN, 0]");
        }
    }

    ALWAYS_INLINE void round(int v) {
        if (!mpd_qsetround(&ctx, v)) {
            throw value_error("valid values for rounding are:\n"
                              "  [ROUND_CEILING, ROUND_FLOOR, ROUND_UP, ROUND_DOWN,\n"
                              "   ROUND_HALF_UP, ROUND_HALF_DOWN, ROUND_HALF_EVEN,\n"
                              "   ROUND_05UP]");
        }
    }

    ALWAYS_INLINE void status(uint32_t v) {
        if (!mpd_qsetstatus(&ctx, v)) {
            throw value_error("invalid status flag");
        }
    }

    ALWAYS_INLINE void traps(uint32_t v) {
        if (!mpd_qsettraps(&ctx, v)) {
            throw value_error("invalid status flag");
        }
    }

    ALWAYS_INLINE void clamp(int v) {
        if (!mpd_qsetclamp(&ctx, v)) {
            throw value_error("invalid value for clamp");
        }
    }

    ALWAYS_INLINE void allcr(int v) {
        if (!mpd_qsetcr(&ctx, v)) {
            throw value_error("invalid value for allcr");
        }
    }

    /* add flags to status and raise an exception if a relevant trap is active */
    ALWAYS_INLINE void raise(uint32_t flags) {
        ctx.status |= (flags & ~MPD_Malloc_error);
        const uint32_t active_traps = flags & (ctx.traps | MPD_Malloc_error);
        if (active_traps) {
            raiseit(active_traps);
        }
    }

    /* add selected status flags */
    ALWAYS_INLINE void add_status(uint32_t flags) {
        if (flags > MPD_Max_status) {
            throw value_error("invalid flags");
        }
        ctx.status |= flags;
    }

    /* clear all status flags */
    ALWAYS_INLINE void clear_status() {
        ctx.status = 0;
    }

    /* clear selected status flags */
    ALWAYS_INLINE void clear_status(uint32_t flags) {
        if (flags > MPD_Max_status) {
            throw value_error("invalid flags");
        }
        ctx.status &= ~flags;
    }

    /* add selected traps */
    ALWAYS_INLINE void add_traps(uint32_t flags) {
        if (flags > MPD_Max_status) {
            throw value_error("invalid flags");
        }
        ctx.traps |= flags;
    }

    /* clear all traps */
    ALWAYS_INLINE void clear_traps() {
        ctx.traps = 0;
    }

    /* clear selected traps */
    ALWAYS_INLINE void clear_traps(uint32_t flags) {
        if (flags > MPD_Max_status) {
            throw value_error("invalid flags");
        }
        ctx.traps &= ~flags;
    }

    std::string repr() const;
    friend std::ostream& operator<<(std::ostream& os, const bd_context& c);
};

BIGMATHPP_API bd_context MaxContext();
BIGMATHPP_API bd_context IEEEContext(int bits);

} // namespace bigmath

#endif //BIGMATHPP_BD_CONTEXT_H
