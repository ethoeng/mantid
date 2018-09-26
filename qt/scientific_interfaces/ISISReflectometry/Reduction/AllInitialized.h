#ifndef MANTID_CUSTOMINTERFACES_ALLINITIALIZED_H_
#define MANTID_CUSTOMINTERFACES_ALLINITIALIZED_H_
#include <boost/optional.hpp>

namespace MantidQt {
namespace CustomInterfaces {

template <typename Param>
bool allInitialized(boost::optional<Param> const &param) {
  return param.is_initialized();
}

template <typename FirstParam, typename SecondParam, typename... Params>
bool allInitialized(boost::optional<FirstParam> const &first,
                    boost::optional<SecondParam> const &second,
                    boost::optional<Params> const &... params) {
  return first.is_initialized() && allInitialized(second, params...);
}

template <typename Result, typename... Params>
boost::optional<Result>
makeIfAllInitialized(boost::optional<Params> const &... params) {
  if (allInitialized(params...))
    return Result(params.get()...);
  else
    return boost::none;
}

} // namespace CustomInterfaces
} // namespace MantidQt
#endif // MANTID_CUSTOMINTERFACES_ALLINITIALIZED_H_
