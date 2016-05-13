from mantid.api import AlgorithmFactory, FileFinder, PythonAlgorithm
from mantid.kernel import ConfigService, Direction, IntBoundedValidator, \
    StringListValidator


class GetIPTS(PythonAlgorithm):
    def category(self):
        return "PythonAlgorithms;Utility"

    def name(self):
        return "GetIPTS"

    def summary(self):
        return "Extracts the IPTS number from a run using FileFinder"

    def getValidInstruments(self):
        instruments = ['']

        for name in ['SNS', 'HFIR']:
            facility = ConfigService.getFacility(name)
            facilityInstruments = [item.shortName()
                                   for item in facility.instruments()
                                   if item != 'DAS']
            facilityInstruments.sort()
            instruments.extend(facilityInstruments)

        return instruments

    def findFile(self, instrument, runnumber):
        # start with run and check the five before it
        runIds = range(runnumber, runnumber-6, -1)
        # check for one after as well
        runIds.append(runnumber + 1)

        runIds = [str(id) for id in runIds if id > 0]

        # prepend non-empty instrument name for FileFinder
        if len(instrument) > 0:
            runIds = ['%s_%s' % (instrument, id) for id in runIds]

        # look for a file
        for runId in runIds:
            self.log().information("Looking for '%s'" % runId)
            try:
                return FileFinder.findRuns(runId)[0]
            except RuntimeError:
                pass  # just keep looking

        # failed to find any is an error
        raise RuntimeError("Cannot find IPTS directory for '%s'"
                           % runnumber)

    def getIPTSLocal(self, instrument, runnumber):
        filename = self.findFile(instrument, runnumber)

        # convert to the path to the proposal
        location = filename.find('IPTS')
        location = filename.find('/', location)
        direc = filename[0:location+1]
        return direc

    def PyInit(self):
        self.declareProperty('RunNumber', defaultValue=0,
                             direction=Direction.Input,
                             validator=IntBoundedValidator(lower=1),
                             doc="Extracts the IPTS number for a run")

        instruments = self.getValidInstruments()
        self.declareProperty('Instrument', '',
                             StringListValidator(instruments),
                             "Empty uses default instrument")

        self.declareProperty('Directory', '',
                             direction=Direction.Output)

    def PyExec(self):
        instrument = self.getProperty('Instrument').value
        runnumber = self.getProperty('RunNumber').value

        direc = self.getIPTSLocal(instrument, runnumber)
        self.setPropertyValue('Directory', direc)
        self.log().notice('IPTS directory is: %s' % direc)

AlgorithmFactory.subscribe(GetIPTS)
