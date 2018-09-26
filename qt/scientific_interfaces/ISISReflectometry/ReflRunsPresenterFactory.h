#ifndef MANTID_ISISREFLECTOMETRY_REFLRUNSPRESENTERFACTORY_H
#define MANTID_ISISREFLECTOMETRY_REFLRUNSPRESENTERFACTORY_H
#include "../General/IReflSearcher.h"
#include "DllConfig.h"
#include "GUI/Event/IEventPresenter.h"
#include "GUI/RunsTable/RunsTablePresenterFactory.h"
#include "IReflRunsTabPresenter.h"
#include "IReflRunsTabView.h"
#include "MantidQtWidgets/Common/ProgressableView.h"
#include "ReflRunsTabPresenter.h"
#include <memory>

namespace MantidQt {
namespace CustomInterfaces {

class RunsPresenterFactory {
public:
  RunsPresenterFactory(RunsTablePresenterFactory runsTablePresenterFactory,
                       WorkspaceNamesFactory workspaceNamesFactory,
                       double thetaTolerance,
                       std::vector<std::string> instruments,
                       int defaultInstrumentIndex,
                       IReflMessageHandler *messageHandler,
                       boost::shared_ptr<IReflSearcher> searcher)
      : m_runsTablePresenterFactory(std::move(runsTablePresenterFactory)),
        m_workspaceNamesFactory(std::move(workspaceNamesFactory)),
        m_thetaTolerance(std::move(thetaTolerance)),
        m_instruments(std::move(instruments)),
        m_defaultInstrumentIndex(std::move(defaultInstrumentIndex)),
        m_messageHandler(messageHandler), m_searcher(std::move(searcher)) {}

  std::unique_ptr<IReflRunsTabPresenter> make(IReflRunsTabView *view) {
    return std::make_unique<ReflRunsTabPresenter>(
        view, view, m_runsTablePresenterFactory, m_workspaceNamesFactory,
        m_thetaTolerance, m_instruments, m_defaultInstrumentIndex,
        m_messageHandler, m_searcher);
  }

private:
  RunsTablePresenterFactory m_runsTablePresenterFactory;
  WorkspaceNamesFactory m_workspaceNamesFactory;
  double m_thetaTolerance;
  std::vector<std::string> m_instruments;
  int m_defaultInstrumentIndex;
  IReflMessageHandler *m_messageHandler;
  boost::shared_ptr<IReflSearcher> m_searcher;
};
} // namespace CustomInterfaces
} // namespace MantidQt
#endif // MANTID_ISISREFLECTOMETRY_REFLRUNSPRESENTERFACTORY_H
