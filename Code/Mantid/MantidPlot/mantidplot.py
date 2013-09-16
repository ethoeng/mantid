"""
MantidPlot module to gain access to plotting functions etc.
Requires that the main script be run from within MantidPlot 
"""
# Requires MantidPlot
try:
    import _qti
except ImportError:
    raise ImportError('The "mantidplot" module can only be used from within MantidPlot.')

import mantidplotpy.proxies as proxies
from mantidplotpy.proxies import threadsafe_call, new_proxy

from PyQt4 import QtCore, QtGui
from PyQt4.QtCore import Qt
import os
import time

# Import into the global namespace qti classes that:
#   (a) don't need a proxy & (b) can be constructed from python or (c) have enumerations within them
from _qti import (PlotSymbol, ImageSymbol, ArrowMarker, ImageMarker,
                  GraphOptions, InstrumentWindow, InstrumentWindowPickTab, InstrumentWindowMaskTab)

# Make the ApplicationWindow instance accessible from the mantidplot namespace
from _qti import app

# Alias threadsafe_call so users have a more understandable name
gui_cmd = threadsafe_call

#---------------- Mantid Python access functions--------------------------
# Grab a few Mantid things so that we can recognise workspace variables
def _get_analysis_data_service():
    """Returns an object that can be used to get a workspace by name from Mantid
    
    Returns:
        A object that acts like a dictionary to retrieve workspaces
    """
    import mantid
    return mantid.AnalysisDataService.Instance()

#-------------------------- Wrapped MantidPlot functions -----------------

def runPythonScript(code, async = False, quiet = False, redirect = True):
    """
        Redirects the runPythonScript method to the app object
        @param code :: A string of code to execute
        @param async :: If the true the code is executed in a separate thread
        @param quiet :: If true no messages reporting status are issued
        @param redirect :: If true then output is redirected to MantidPlot
    """
    if async and QtCore.QThread.currentThread() != QtGui.qApp.thread():
        async = False
    threadsafe_call(_qti.app.runPythonScript, code, async, quiet, redirect)

# Overload for consistency with qtiplot table(..) & matrix(..) commands
def workspace(name):
    """Get a handle on a workspace.
    
    Args:
        name: The name of the workspace in the Analysis Data Service.
    """
    return _get_analysis_data_service()[name]

def table(name):
    """Get a handle on a table.
    
    Args:
        name: The name of the table.
        
    Returns:
        A handle to the table.
    """
    return new_proxy(proxies.MDIWindow, _qti.app.table, name)

def newTable(name=None,rows=30,columns=2):
    """Create a table.
    
    Args:
        name: The name to give to the table (if None, a unique name will be generated).
        row: The number of rows in the table (default: 30).
        columns: The number of columns in the table (default: 2).
        
    Returns:
        A handle to the created table.
    """
    if name is None:
        return new_proxy(proxies.MDIWindow, _qti.app.newTable)
    else:
        return new_proxy(proxies.MDIWindow, _qti.app.newTable, name,rows,columns)

def matrix(name):
    """Get a handle on a matrix.
    
    Args:
        name: The name of the matrix.
        
    Returns:
        A handle to the matrix.
    """
    return new_proxy(proxies.MDIWindow, _qti.app.matrix, name)

def newMatrix(name=None,rows=32,columns=32):
    """Create a matrix (N.B. This is not the same as a 'MantidMatrix').
    
    Args:
        name: The name to give to the matrix (if None, a unique name will be generated).
        row: The number of rows in the matrix (default: 32).
        columns: The number of columns in the matrix (default: 32).
        
    Returns:
        A handle to the created matrix.
    """
    if name is None:
        return new_proxy(proxies.MDIWindow, _qti.app.newMatrix)
    else:
        return new_proxy(proxies.MDIWindow, _qti.app.newMatrix,name,rows,columns)

def graph(name):
    """Get a handle on a graph widget.
    
    Args:
        name: The name of the graph window.
        
    Returns:
        A handle to the graph.
    """
    return new_proxy(proxies.Graph, _qti.app.graph, name)

def newGraph(name=None,layers=1,rows=1,columns=1):
    """Create a graph window.
    
    Args:
        name: The name to give to the graph (if None, a unique name will be generated).
        layers: The number of plots (a.k.a. layers) to put in the graph window (default: 1).
        rows: The number of rows of to put in the graph window (default: 1).
        columns: The number of columns of to put in the graph window (default: 1).
        
    Returns:
        A handle to the created graph widget.
    """
    if name is None:
        return new_proxy(proxies.Graph, _qti.app.newGraph)
    else:
        return new_proxy(proxies.Graph, _qti.app.newGraph,name,layers,rows,columns)

def note(name):
    """Get a handle on a note.
    
    Args:
        name: The name of the note.
        
    Returns:
        A handle to the note.
    """
    return new_proxy(proxies.MDIWindow, _qti.app.note, name)

def newNote(name=None):
    """Create a note.
    
    Args:
        name: The name to give to the note (if None, a unique name will be generated).
        
    Returns:
        A handle to the created note.
    """
    if name is None:
        return new_proxy(proxies.MDIWindow, _qti.app.newNote)
    else:
        return new_proxy(proxies.MDIWindow, _qti.app.newNote, name)

#-----------------------------------------------------------------------------
# Intercept qtiplot "plot" command and forward to plotSpectrum for a workspace
def plot(source, *args, **kwargs):
    """Create a new plot given a workspace, table or matrix.
    
    Args:
        source: what to plot; if it is a Workspace, will
                call plotSpectrum()
    
    Returns:
        A handle to the created Graph widget.
    """
    if hasattr(source, '_getHeldObject') and isinstance(source._getHeldObject(), QtCore.QObject):
        return new_proxy(proxies.Graph,_qti.app.plot, source._getHeldObject(), *args, **kwargs)
    else:
        return plotSpectrum(source, *args, **kwargs)
        
#-----------------------------------------------------------------------------
def plotSpectrum(source, indices, error_bars = False, type = -1):
    """Open a 1D Plot of a spectrum in a workspace.
    
    This plots one or more spectra, with X as the bin boundaries,
    and Y as the counts in each bin.
    
    Args:
        source: workspace or name of a workspace
        indices: workspace index, or tuple or list of workspace indices to plot
        error_bars: bool, set to True to add error bars.
    """
    workspace_names = __getWorkspaceNames(source)
    index_list = __getWorkspaceIndices(indices)
    if len(workspace_names) == 0:
        raise ValueError("No workspace names given to plot")
    if len(index_list) == 0:
        raise ValueError("No indices given to plot")
    graph = proxies.Graph(threadsafe_call(_qti.app.mantidUI.plotSpectraList, workspace_names, index_list, error_bars, type))
    if graph._getHeldObject() == None:
        raise RuntimeError("Cannot create graph, see log for details.")
    else:
        return graph

def fitBrowser():
    """
    Access the fit browser. 
    """
    import mantidqtpython
    return proxies.FitBrowserProxy(_qti.app.mantidUI.fitFunctionBrowser())

#-----------------------------------------------------------------------------
def plotBin(source, indices, error_bars = False, graph_type = 0):
    """Create a 1D Plot of bin count vs spectrum in a workspace.
    
    This puts the spectrum number as the X variable, and the
    count in the particular bin # (in 'indices') as the Y value.
    
    If indices is a tuple or list, then several curves are created, one
    for each bin index.
    
    Args:
        source: workspace or name of a workspace
        indices: bin number(s) to plot
        error_bars: bool, set to True to add error bars.

    Returns:
        A handle to the created Graph widget.
    """
    def _callPlotBin(workspace, indexes, errors, graph_type):
        if isinstance(workspace, str):
            wkspname = workspace
        else:
            wkspname = workspace.getName()
        if type(indexes) == int:
            indexes = [indexes]
        return new_proxy(proxies.Graph,_qti.app.mantidUI.plotBin,wkspname, indexes, errors,graph_type)

    if isinstance(source, list) or isinstance(source, tuple):
        if len(source) > 1:
            raise RuntimeError("Currently unable to handle multiple sources for bin plotting. Merging must be done by hand.")
        else:
            source = source[0]
    return _callPlotBin(source, indices, error_bars, graph_type)

#-----------------------------------------------------------------------------
def stemPlot(source, index, power=None, startPoint=None, endPoint=None):
    """Generate a stem-and-leaf plot from an input table column or workspace spectrum
    
    Args:
        source: A reference to a workspace or a table.
        index: For a table, the column number or name. For a workspace, the workspace index.
        power: The stem unit as a power of 10. If not provided, a dialog will appear with a suggested value.
        startPoint: The first point (row or bin) to use (Default: the first one).
        endPoint: The last point (row or bin) to use (Default: the last one).
        
    Returns:
        A string representation of the stem plot
    """
    # Turn the optional arguments into the magic numbers that the C++ expects
    if power==None:
        power=1001
    if startPoint==None:
        startPoint=0
    if endPoint==None:
        endPoint=-1
    
    if isinstance(source,proxies.QtProxyObject):
        source = source._getHeldObject()
    elif hasattr(source, 'getName'):
        # If the source is a workspace, create a table from the specified index
        wsName = source.getName()
        source = threadsafe_call(_qti.app.mantidUI.workspaceToTable.wsName,wsName,[index],False,True)
        # The C++ stemPlot method takes the name of the column, so get that
        index = source.colName(2)
    # Get column name if necessary
    if isinstance(index, int):
        index = source.colName(index)
    # Call the C++ method
    return threadsafe_call(_qti.app.stemPlot, source,index,power,startPoint,endPoint)

#-----------------------------------------------------------------------------
def waterfallPlot(table, columns):
    """Create a waterfall plot from data in a table.
    
    Args:
        table: A reference to the table containing the data to plot
        columns: A tuple of the column numbers whose data to plot
        
    Returns:
        A handle to the created plot (Layer).
    """
    return new_proxy(proxies.Graph, _qti.app.waterfallPlot, table._getHeldObject(),columns)

#-----------------------------------------------------------------------------
def importImage(filename):
    """Load an image file into a matrix.
    
    Args:
        filename: The name of the file to load.
        
    Returns:
        A handle to the matrix containing the image data.
    """
    return new_proxy(proxies.MDIWindow, _qti.app.importImage, filename)

#-----------------------------------------------------------------------------
def newPlot3D():
    return new_proxy(proxies.Graph3D, _qti.app.newPlot3D)

def plot3D(*args):
    if isinstance(args[0],str):
        return new_proxy(proxies.Graph3D, _qti.app.plot3D, *args)
    else:
        return new_proxy(proxies.Graph3D, _qti.app.plot3D, args[0]._getHeldObject(),*args[1:])

#-----------------------------------------------------------------------------
def selectMultiPeak(source, showFitPropertyBrowser = True, xmin = None, xmax = None):
    """Switch on the multi-peak selecting tool for fitting with the Fit algorithm.
    
    Args:
        source: A reference to a MultiLayer with the data to fit.
        showFitPropertyBrowser: Whether to show the FitPropertyBrowser or not.
        xmin: An optionall minimum X value to select
        xmax: An optionall maximum X value to select
    """
    if xmin is not None and xmax is not None:
        threadsafe_call(_qti.app.selectMultiPeak, source._getHeldObject(), showFitPropertyBrowser, xmin, xmax)
    else:
        threadsafe_call(_qti.app.selectMultiPeak, source._getHeldObject(), showFitPropertyBrowser)

#-----------------------------------------------------------------------------
#-------------------------- Project/Folder functions -----------------------
#-----------------------------------------------------------------------------
def windows():
    """Get a list of the open windows."""
    f = _qti.app.windows()
    ret = []
    for item in f:
        ret.append(proxies.MDIWindow(item))
    return ret

def activeFolder():
    """Get a handle to the currently active folder."""
    return new_proxy(proxies.Folder, _qti.app.activeFolder)

# These methods don't seem to work
#def appendProject(filename, parentFolder=None):
#    if parentFolder is not None:
#        parentFolder = parentFolder._getHeldObject()
#    return proxies.Folder(_qti.app.appendProject(filename,parentFolder))
#
#def saveFolder(folder, filename, compress=False):
#    _qti.app.saveFolder(folder._getHeldObject(),filename,compress)

def rootFolder():
    """Get a handle to the top-level folder."""
    return new_proxy(proxies.Folder, _qti.app.rootFolder)

def addFolder(name,parentFolder=None):
    """Create a new folder.
    
    Args:
        name: The name of the folder to create.
        parentFolder: If given, make the new folder a subfolder of this one.
        
    Returns:
        A handle to the newly created folder.
    """
    if parentFolder is not None:
        parentFolder = parentFolder._getHeldObject()
    return new_proxy(proxies.Folder, _qti.app.addFolder, name,parentFolder)

def deleteFolder(folder):
    """Delete the referenced folder"""
    return threadsafe_call(_qti.app.deleteFolder, folder._getHeldObject())

def changeFolder(folder, force=False):
    """Changes the current folder.
    
    Args:
        folder: A reference to the folder to change to.
        force: Whether to do stuff even if the new folder is already the active one (default: no).
    
    Returns:
        True on success.
    """
    return threadsafe_call(_qti.app.changeFolder, folder._getHeldObject(),force)

def copyFolder(source, destination):
    """Copy a folder (and its contents) into another.
    
    Returns:
        True on success.
    """
    return threadsafe_call(_qti.app.copyFolder, source._getHeldObject(),destination._getHeldObject())

def openTemplate(filename):
    """Load a previously saved window template"""
    return new_proxy(proxies.MDIWindow,_qti.app.openTemplate, filename)

def saveAsTemplate(window, filename):
    """Save the characteristics of the given window to file"""
    threadsafe_call(_qti.app.saveAsTemplate, window._getHeldObject(), filename)

def setWindowName(window, name):
    """Set the given window to have the given name"""
    threadsafe_call(_qti.app.setWindowName, window._getHeldObject(), name)

def setPreferences(layer):
    threadsafe_call(_qti.app.setPreferences, layer._getHeldObject())

def clone(window):
    return new_proxy(proxies.MDIWindow, _qti.app.clone, window._getHeldObject())

def tableToMatrix(table):
    return new_proxy(proxies.MDIWindow, _qti.app.tableToMatrix, table._getHeldObject())

def matrixToTable(matrix, conversionType=_qti.app.Direct):
    return new_proxy(proxies.MDIWindow, _qti.app.matrixToTable, matrix._getHeldObject(),conversionType)

#-----------------------------------------------------------------------------
#-------------------------- Wrapped MantidUI functions -----------------------
#-----------------------------------------------------------------------------

def mergePlots(graph1,graph2):
    """Combine two graphs into a single plot"""
    return new_proxy(proxies.Graph, _qti.app.mantidUI.mergePlots,graph1._getHeldObject(),graph2._getHeldObject())

def convertToWaterfall(graph):
    """Convert a graph (containing a number of plotted spectra) to a waterfall plot"""
    threadsafe_call(_qti.app.mantidUI.convertToWaterfall, graph._getHeldObject())

def getMantidMatrix(name):
    """Get a handle to the named Mantid matrix"""
    return new_proxy(proxies.MantidMatrix, _qti.app.mantidUI.getMantidMatrix, name)

def getInstrumentView(name, tab=InstrumentWindow.RENDER):
    """Create an instrument view window based on the given workspace.
    
    Args:
        name: The name of the workspace.
        tab: The index of the tab to display initially, (default=InstrumentWindow.RENDER)
        
    Returns:
        A handle to the created instrument view widget.
    """
    ads = _get_analysis_data_service()
    if name not in ads:
        raise ValueError("Workspace %s does not exist" % name)
    return new_proxy(proxies.InstrumentWindow, _qti.app.mantidUI.getInstrumentView, name, tab)

def importMatrixWorkspace(name, firstIndex=None, lastIndex=None, showDialog=False, visible=False):
    """Create a MantidMatrix object from the named workspace.
    
    Args:
        name: The name of the workspace.
        firstIndex: The workspace index to start at (default: the first one).
        lastIndex: The workspace index to stop at (default: the last one).
        showDialog: Whether to bring up a dialog to allow options to be entered manually (default: no).
        visible: Whether to initially show the created matrix (default: no).
        
    Returns:
        A handle to the created matrix.
    """
    # Turn the optional arguments into the magic numbers that the C++ expects
    if firstIndex is None:
        firstIndex = -1
    if lastIndex is None:
        lastIndex = -1
    return new_proxy(proxies.MantidMatrix, _qti.app.mantidUI.importMatrixWorkspace, name,
                             firstIndex,lastIndex,showDialog,visible)

def importTableWorkspace(name, visible=False):
    """Create a MantidPlot table from a table workspace.
    
    Args:
        name: The name of the workspace.
        visible: Whether to initially show the created matrix (default: no).
        
    Returns:
        A handle to the newly created table.
    """
    return new_proxy(proxies.MDIWindow,_qti.app.mantidUI.importTableWorkspace, name,False,visible)

def createPropertyInputDialog(alg_name, preset_values, optional_msg, enabled, disabled):
    """Raises a property input dialog for an algorithm"""
    return threadsafe_call(_qti.app.mantidUI.createPropertyInputDialog, alg_name, preset_values, optional_msg, enabled, disabled)
    
#-----------------------------------------------------------------------------
#-------------------------- SliceViewer functions ----------------------------
#-----------------------------------------------------------------------------

#-----------------------------------------------------------------------------
def plotSlice(source, label="", xydim=None, slicepoint=None,
                    colormin=None, colormax=None, colorscalelog=False,
                    normalization=1,
                    limits=None, **kwargs):
    """Opens the SliceViewer with the given MDWorkspace(s).
    
    Args:
        source :: one workspace, or a list of workspaces
        
    Optional Keyword Args:
        label :: label for the window title
        xydim :: indexes or names of the dimensions to plot, as an (X,Y) list or tuple. See SliceViewer::setXYDim()
        slicepoint :: list with the slice point in each dimension.  Must be the same length as the number of dimensions of the workspace. See SliceViewer::setSlicePoint()
        colormin :: value of the minimum color in the scale. See SliceViewer::setColorScaleMin()
        colormax :: value of the maximum color in the scale. See SliceViewer::setColorScaleMax()
        colorscalelog :: value of the maximum color in the scale. See SliceViewer::setColorScaleLog()
        limits :: list with the (xleft, xright, ybottom, ytop) limits to the view to show. See SliceViewer::setXYLimits()
        normalization :: 0=none; 1=volume (default); 2=# of events.
        
    Returns:
        a (list of) handle(s) to the SliceViewerWindow widgets that were open.
        Use SliceViewerWindow.getSlicer() to get access to the functions of the
        SliceViewer, e.g. setting the view and slice point.
    """ 
    workspace_names = __getWorkspaceNames(source)
    try:
        import mantidqtpython
    except:
        print "Could not find module mantidqtpython. Cannot open the SliceViewer."
        return
    
    # Make a list of widgets to return
    out = []
    for wsname in workspace_names:
        window = __doSliceViewer(wsname, label=label,
           xydim=xydim, slicepoint=slicepoint, colormin=colormin,
           colormax=colormax, colorscalelog=colorscalelog, limits=limits, 
           normalization=normalization,
           **kwargs) 
        pxy = proxies.SliceViewerWindowProxy(window)
        out.append(pxy)
        
    # Return the widget alone if only 1
    if len(out) == 1:
        return out[0]
    else:
        return out


#-----------------------------------------------------------------------------
def getSliceViewer(source, label=""):
    """Retrieves a handle to a previously-open SliceViewerWindow.
    This allows you to get a handle on, e.g., a SliceViewer that was open
    by the MultiSlice view in VATES Simple Interface.
    Will raise an exception if not found.
    
    Args:
        source :: name of the workspace that was open
        label :: additional label string that was used to identify the window.
        
    Returns:
        a handle to the SliceViewerWindow object that was created before. 
    """
    import mantidqtpython
    workspace_names = __getWorkspaceNames(source)
    if len(workspace_names) != 1:
        raise Exception("Please specify only one workspace.")
    else:
        svw = threadsafe_call(mantidqtpython.MantidQt.Factory.WidgetFactory.Instance().getSliceViewerWindow, workspace_names[0], label)
        if svw is not None:
            return proxies.SliceViewerWindowProxy(svw)
        else:
            return None


#-----------------------------------------------------------------------------
def closeAllSliceViewers():
    """
    Closes all currently open SliceViewer windows. This might be useful to 
    clean up your desktop after opening many windows.
    """
    import mantidqtpython
    threadsafe_call(mantidqtpython.MantidQt.Factory.WidgetFactory.Instance().closeAllSliceViewerWindows)

#-----------------------------------------------------------------------------
# Legacy function
plotTimeBin = plotBin

# import 'safe' methods (i.e. no proxy required) of ApplicationWindow into the global namespace
# Only 1 at the moment!
appImports = [
        'saveProjectAs'
        ]
for name in appImports:
    globals()[name] = getattr(_qti.app,name)
        
# Ensure these functions are available as without the _qti.app.mantidUI prefix
MantidUIImports = [
    'getSelectedWorkspaceName'
    ]
# Update globals
for name in MantidUIImports:
    globals()[name] = getattr(_qti.app.mantidUI,name)
    
# Set some aliases for Layer enumerations so that old code will still work
Layer = _qti.Layer
Layer.Log10 = _qti.GraphOptions.Log10
Layer.Linear = _qti.GraphOptions.Linear
Layer.Left = _qti.GraphOptions.Left
Layer.Right = _qti.GraphOptions.Right
Layer.Bottom = _qti.GraphOptions.Bottom
Layer.Top = _qti.GraphOptions.Top

#-----------------------------------------------------------------------------
#--------------------------- "Private" functions -----------------------------
#-----------------------------------------------------------------------------


#-----------------------------------------------------------------------------
def __getWorkspaceNames(source):
    """Takes a "source", which could be a WorkspaceGroup, or a list
    of workspaces, or a list of names, and converts
    it to a list of workspace names.
    
    Args:
        source :: input list or workspace group
        
    Returns:
        list of workspace names 
    """
    ws_names = []
    if isinstance(source, list) or isinstance(source,tuple):
        for w in source:
            names = __getWorkspaceNames(w)
            ws_names += names
    elif hasattr(source, 'getName'):
        if hasattr(source, '_getHeldObject'):
            wspace = source._getHeldObject()
        else:
            wspace = source
        if wspace == None:
            return []
        if hasattr(wspace, 'getNames'):
            grp_names = wspace.getNames()
            for n in grp_names:
                if n != wspace.getName():
                    ws_names.append(n)
        else:
            ws_names.append(wspace.getName())
    elif isinstance(source,str):
        w = _get_analysis_data_service()[source]
        if w != None:
            names = __getWorkspaceNames(w)
            for n in names:
                ws_names.append(n)
    else:
        raise TypeError('Incorrect type passed as workspace argument "' + str(source) + '"')
    return ws_names
    
#-----------------------------------------------------------------------------
def __doSliceViewer(wsname, label="", xydim=None, slicepoint=None,
                    colormin=None, colormax=None, colorscalelog=False,
                    limits=None, normalization=1):
    """Open a single SliceViewerWindow for the workspace, and shows it
    
    Args:
        wsname :: name of the workspace
    See plotSlice() for full list of keyword parameters.
        
    Returns:
        A handle to the created SliceViewerWindow widget
    """
    import mantidqtpython
    from PyQt4 import QtCore
    
    svw = threadsafe_call(mantidqtpython.MantidQt.Factory.WidgetFactory.Instance().createSliceViewerWindow, wsname, label)
    threadsafe_call(svw.show)
    
    # -- Connect to main window's shut down signal ---
    QtCore.QObject.connect(_qti.app, QtCore.SIGNAL("shutting_down()"),
                    svw, QtCore.SLOT("close()"))
    
    sv = threadsafe_call(svw.getSlicer)
    # --- X/Y Dimensions ---
    if (not xydim is None):
        if len(xydim) != 2:
            raise Exception("You need to specify two values in the 'xydim' parameter")
        else:
            threadsafe_call(sv.setXYDim, xydim[0], xydim[1])
    
    # --- Slice point ---
    if not slicepoint is None:
        for d in xrange(len(slicepoint)): 
            try:
                val = float(slicepoint[d])
            except ValueError:
                raise ValueError("Could not convert item %d of slicepoint parameter to float (got '%s'" % (d, slicepoint[d]))
            sv.setSlicePoint(d, val)  
            
    # Set the normalization before the color scale
    threadsafe_call(sv.setNormalization, normalization)
    
    # --- Color scale ---
    if (not colormin is None) and (not colormax is None):
        threadsafe_call(sv.setColorScale, colormin, colormax, colorscalelog)
    else:
        if (not colormin is None): threadsafe_call(sv.setColorScaleMin, colormin)
        if (not colormax is None): threadsafe_call(sv.setColorScaleMax, colormax)
    try:
        threadsafe_call(sv.setColorScaleLog, colorscalelog)
    except:
        print "Log color scale not possible."
    
    # --- XY limits ---
    if not limits is None:
        threadsafe_call(sv.setXYLimits, limits[0], limits[1], limits[2], limits[3])
    
    return svw


def get_screenshot_dir():
    """ Returns the directory for screenshots,
    or NONE if not set """
    expected_env_var = 'MANTID_SCREENSHOT_REPORT'
    dest = os.getenv(expected_env_var)
    if not dest is None:
        # Create the report directory if needed
        if not os.path.exists(dest):
            os.mkdir(dest)
    else:
        errormsg = "The expected environmental does not exist: " + expected_env_var
        print errormsg
        raise RuntimeError(errormsg)
    return dest

    

     
#======================================================================
def _replace_report_text(filename, section, newtext):
    """ Search html report text to 
replace a line <!-- Filename --> etc.
Then, the contents of that section are replaced
@param filename :: full path to .html report
@param section :: string giving the name of the section
@param newtext :: replacement contents of that section. No new lines!
@return the new contents of the entire page 
"""
    # Get the current report contents if any
    if os.path.exists(filename):
        f = open(filename, 'r')
        contents = f.read()
        f.close()
    else:
        contents = ""


    lines = contents.splitlines()
    sections = dict()
    # Find the text in each section
    for line in lines:
        if line.startswith("<!-- "):
            # All lines should go <!-- Section -->
            n = line.find(" ", 5)
            if n > 0:
                current_section = line[5:n].strip()
                current_text = line[n+4:]
                sections[current_section] = current_text
            
    # Replace the section
    sections[section] = newtext.replace("\n","")
    
    # Make the output
    items = sections.items()
    items.sort()
    output = []
    for (section_name, text) in items:
        output.append("<!-- %s -->%s" % (section_name, text))
    
    # Save the new text
    contents = os.linesep.join(output)
    f = open(filename, 'w')
    f.write(contents)
    f.close()
   

class Screenshot(QtCore.QObject):
    """
        Handles taking a screenshot while
        ensuring the call takes place on the GUI
        thread
    """
    
    def take_picture(self, widget, filename):
        """
        Takes a screenshot and saves it to the 
        filename given, ensuring the call is processed
        through a slot if the call is from a separate 
        thread
        """
        # First save the screenshot
        widget.show()
        widget.resize(widget.size())
        QtCore.QCoreApplication.processEvents()
        
        pix = QtGui.QPixmap.grabWidget(widget)
        pix.save(filename)

def screenshot(widget, filename, description, png_exists=False):
    """ Take a screenshot of the widget for displaying in a html report.
    
    The MANTID_SCREENSHOT_REPORT environment variable must be set 
    to the destination folder. Screenshot taking is skipped otherwise.
    
    :param widget: QWidget to grab.
    :param filename: Save to this file (no extension!).
    :param description: Short descriptive text of what the screenshot should look like.
    :param png_exists: if True, then the 'filename' already exists. Don't grab a screenshot, but add to the report.
    """
    dest = get_screenshot_dir()
    if not dest is None:
        report = os.path.join(dest, "index.html")
        
        if png_exists:
            pass
        else:
            # Find the widget if handled with a proxy
            if hasattr(widget, "_getHeldObject"):
                widget = widget._getHeldObject()
                
        if widget is not None:
            camera = Screenshot()
            threadsafe_call(camera.take_picture, widget, os.path.join(dest, filename+".png"))
        
        # Modify the section in the HTML page
        section_text = '<h2>%s</h2>' % filename
        now = time.strftime('%Y-%m-%d %H:%M:%S', time.localtime())
        section_text += '%s (%s)<br />' % (description, now)
        section_text += '<img src="%s.png" alt="%s"></img>' % (filename, description)
        
        _replace_report_text(report, filename, section_text)
        
def screenshot_to_dir(widget, filename, screenshot_dir):
    """Take a screenshot_to_dir of a widget
    
    @param widget :: QWidget to take an image of
    @param filename :: Destination filename for that image
    @param screenshot_dir :: Directory to put the screenshots into.
    """
        # Find the widget if handled with a proxy
    if hasattr(widget, "_getHeldObject"):
        widget = widget._getHeldObject()
                
    if widget is not None:
        camera = Screenshot()
        threadsafe_call(camera.take_picture, widget, os.path.join(screenshot_dir, filename+".png"))
    

#=============================================================================
# Helper methods
#=============================================================================

def __getWorkspaceIndices(source):
    """
        Returns a list of workspace indices from a source.
        The source can be a list, a tuple, an int or a string.
    """
    index_list = []
    if isinstance(source,list) or isinstance(source,tuple):
        for i in source:
            nums = __getWorkspaceIndices(i)
            for j in nums:
                index_list.append(j)
    elif isinstance(source, int):
        index_list.append(source)
    elif isinstance(source, str):
        elems = source.split(',')
        for i in elems:
            try:
                index_list.append(int(i))
            except ValueError:
                pass
    else:
        raise TypeError('Incorrect type passed as index argument "' + str(source) + '"')
    return index_list

#-----------------------------------------------------------------------------
