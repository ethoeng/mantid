#ifndef MANTID_ISISREFLECTOMETRY_INVALIDDEFAULTSERROR_H
#define MANTID_ISISREFLECTOMETRY_INVALIDDEFAULTSERROR_H
#include "../../DllConfig.h"
#include <vector>

namespace MantidQt {
namespace CustomInterfaces {

class MANTIDQT_ISISREFLECTOMETRY_DLL InvalidDefaultsError {
public:
  InvalidDefaultsError(int row, std::vector<int> invalidColumns);
  std::vector<int> const &invalidColumns() const;
  int row() const;

private:
  std::vector<int> m_invalidColumns;
  int m_row;
};

} // namespace CustomInterfaces
} // namespace MantidQt
#endif // MANTID_ISISREFLECTOMETRY_INVALIDDEFAULTSERROR_H
