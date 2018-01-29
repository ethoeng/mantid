#ifndef MANTID_ISISREFLECTOMETRY_INSTRUMENTPARAMETERS_H
#define MANTID_ISISREFLECTOMETRY_INSTRUMENTPARAMETERS_H
#include <vector>
#include <boost/optional.hpp>
#include "GetInstrumentParameter.h"
#include "First.h"
#include "ValueOr.h"

namespace MantidQt {
namespace CustomInterfaces {

template <typename T>
boost::optional<T>
firstFromParameterFile(Mantid::Geometry::Instrument_const_sptr instrument,
                       std::string const &parameterName) {
  return first(getInstrumentParameter<T>(instrument, parameterName));
}

template <typename... Ts>
boost::optional<boost::variant<Ts...>> firstFromParameterFileVariant(
    Mantid::Geometry::Instrument_const_sptr instrument,
    std::string const &parameterName) {
  return first<Ts...>(
      getInstrumentParameter<boost::variant<Ts...>>(instrument, parameterName));
}

class MissingInstrumentParameterValue {
public:
  explicit MissingInstrumentParameterValue(std::string const &parameterName)
      : m_parameterName(parameterName) {}

  std::string const &parameterName() const;

private:
  std::string m_parameterName;
};

class InstrumentParameters {
public:
  explicit InstrumentParameters(
      Mantid::Geometry::Instrument_const_sptr instrument);

  template <typename T> T valueOrEmpty(std::string const &parameterName) {
    static_assert(!std::is_arithmetic<T>::value, "Use valueOrZero instead.");
    return fromFileOrDefaultConstruct<T>(parameterName);
  }

  template <typename T> T valueOrZero(std::string const &parameterName) {
    static_assert(std::is_arithmetic<T>::value, "Use valueOrEmpty instead.");
    return fromFileOrDefaultConstruct<T>(parameterName);
  }

  template <typename T>
  boost::optional<T> optional(std::string const &parameterName) {
    return fromFile<T>(parameterName);
  }

  template <typename T>
  T handleMissingValue(boost::optional<T> const &value,
                       std::string const &parameterName) {
    if (value)
      return value.get();
    else {
      m_missingValueErrors.emplace_back(parameterName);
      return T();
    }
  }

  template <typename T> T mandatory(std::string const &parameterName) {
    try {
      return handleMissingValue(
          firstFromParameterFile<T>(m_instrument, parameterName),
          parameterName);
    } catch (InstrumentParameterTypeMissmatch const &ex) {
      m_typeErrors.emplace_back(ex);
      return T();
    }
  }

  template <typename T, typename... Ts>
  boost::variant<T, Ts...> mandatoryVariant(std::string const &parameterName) {
    try {
      return handleMissingValue(
          firstFromParameterFileVariant<T, Ts...>(m_instrument, parameterName),
          parameterName);
    } catch (InstrumentParameterTypeMissmatch const &ex) {
      m_typeErrors.emplace_back(ex);
      return T();
    }
  }

  std::vector<InstrumentParameterTypeMissmatch> const &typeErrors() const;
  bool hasTypeErrors() const;
  std::vector<MissingInstrumentParameterValue> const &missingValues() const;
  bool hasMissingValues() const;

private:
  template <typename T>
  T fromFileOrDefaultConstruct(std::string const &parameterName) {
    return value_or(fromFile<T>(parameterName), T());
  }

  template <typename T>
  boost::optional<T> fromFile(std::string const &parameterName) {
    try {
      return firstFromParameterFile<T>(m_instrument, parameterName);
    } catch (InstrumentParameterTypeMissmatch const &ex) {
      m_typeErrors.emplace_back(ex);
      return boost::none;
    }
  }

  Mantid::Geometry::Instrument_const_sptr m_instrument;
  std::vector<InstrumentParameterTypeMissmatch> m_typeErrors;
  std::vector<MissingInstrumentParameterValue> m_missingValueErrors;
};
}
}
#endif // MANTID_ISISREFLECTOMETRY_INSTRUMENTPARAMETERS_H
