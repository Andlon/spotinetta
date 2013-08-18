#pragma once

#include "basicobject.h"
#include "definitions.h"

namespace Spotinetta {

namespace detail {
// BasicLoadableObject requires functions to be available at compile-time, and thus
// libspotify functions can not be used directly as they're merely dynamically linked.
// Use proxy functions like these to work around this restriction
inline sp_error user_add_ref(sp_user * user) { return sp_user_add_ref(user); }
inline sp_error user_release(sp_user * user) { return sp_user_release(user); }
inline bool user_is_loaded(sp_user * user) { return sp_user_is_loaded(user); }
}

typedef BasicLoadableObject<sp_user, &detail::user_add_ref, &detail::user_release, &detail::user_is_loaded> BasicUser;

class User : public BasicUser {
public:
    User() { }
    explicit User(sp_user * user, bool increment = true) : BasicUser(user, increment) { }

    QString canonicalName() const;
    QString displayName() const;
};

}
