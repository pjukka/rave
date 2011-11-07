'''
Copyright (C) 2010- Swedish Meteorological and Hydrological Institute (SMHI)

This file is part of RAVE.

RAVE is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RAVE is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with RAVE.  If not, see <http://www.gnu.org/licenses/>.

'''
## Plugin for generating a composite that is initiated from the beast
## framework.
## Register in pgf with
## --name=eu.baltrad.beast.generatecomposite
## --strings=area,quantity,method,date,time,selection,anomaly-qc
## --floats=height -m rave_pgf_composite_plugin -f generate
##

## @file
## @author Anders Henja, SMHI
## @date 2010-10-15

import _cartesian
import _pycomposite
import _rave
import _area
import _projection
import _raveio
import _polarvolume
import _polarscan
import area
import string
import rave_tempfile
import odim_source
import math
import rave_pgf_composite_quality_registry

from rave_defines import CENTER_ID, GAIN, OFFSET


## Creates a dictionary from a rave argument list
#@param arglist the argument list
#@return a dictionary
def arglist2dict(arglist):
  result={}
  for i in range(0, len(arglist), 2):
    result[arglist[i]] = arglist[i+1]
  return result

##
# Converts a string into a number, either int or float
# @param sval the string to translate
# @return the translated value
# @throws ValueError if value not could be translated
#
def strToNumber(sval):
  try:
    return int(sval)
  except ValueError, e:
    return float(sval)


## Creates a composite
#@param files the list of files to be used for generating the composite
#@param arguments the arguments defining the composite
#@return a temporary h5 file with the composite
def generate(files, arguments):
  args = arglist2dict(arguments)
  
  generator = _pycomposite.new()
  
  a = area.area(args["area"])
  p = a.pcs
  pyarea = _area.new()
  pyarea.id = a.Id
  pyarea.xsize = a.xsize
  pyarea.ysize = a.ysize
  pyarea.xscale = a.xscale
  pyarea.yscale = a.yscale
  pyarea.extent = a.extent
  pyarea.projection = _projection.new(p.id, p.name, string.join(p.definition, ' '))

  # To incorporate quality detectors
  # if "anomaly-qc" in args.keys():
  #   detectors = string.split(args["anomaly-qc"], ",")
  #   if "ropo" in detectors:
  #     execute ropo code
  #   elif "bipo" in detectors:
  #     execute bipo code
  #   and so on
  #

  if "anomaly-qc" in args.keys():
      detectors = string.split(args["anomaly-qc"], ",")
  else:
      detectors = []

  nodes = ""
  qfields = []  # The quality fields we want to get as a result in the composite

  for d in detectors:
    p = rave_pgf_composite_quality_registry.get_plugin(d)
    if p != None:
      qfields.extend(p.getQualityFields())

  for fname in files:
    rio = _raveio.open(fname)

    obj = rio.object

    if len(nodes):
      nodes += ",'%s'" % odim_source.NODfromSource(obj)
    else:
      nodes += "'%s'" % odim_source.NODfromSource(obj)
    
    for d in detectors:
      p = rave_pgf_composite_quality_registry.get_plugin(d)
      if p != None:
        obj = p.process(obj)
        na = p.algorithm()
        if generator.algorithm == None and na != None: # Try to get the generator algorithm != None 
          generator.algorithm = na

    generator.add(obj)

  generator.quantity = "DBZH"

  if "quantity" in args.keys():
    generator.quantity = args["quantity"]
  
  method = "pcappi"
  if "method" in args.keys():
    method = args["method"]

  if method == "ppi":
    generator.product = _rave.Rave_ProductType_PPI
  elif method == "cappi":
    generator.product = _rave.Rave_ProductType_CAPPI
  else:
    generator.product = _rave.Rave_ProductType_PCAPPI

  generator.height = 1000.0
  generator.elangle = 0.0
  if "prodpar" in args.keys():
    if generator.product in [_rave.Rave_ProductType_CAPPI, _rave.Rave_ProductType_PCAPPI]:
      try:
        generator.height = strToNumber(args["prodpar"])
      except ValueError,e:
        pass
    elif generator.product in [_rave.Rave_ProductType_PPI]:
      try:
        v = strToNumber(args["prodpar"])
        generator.elangle = v * math.pi / 180.0
      except ValueError,e:
        pass

  generator.selection_method = _pycomposite.SelectionMethod_NEAREST
  if "selection" in args.keys():
    if args["selection"] == "NEAREST_RADAR":
      generator.selection_method = _pycomposite.SelectionMethod_NEAREST
    elif args["selection"] == "HEIGHT_ABOVE_SEALEVEL":
      generator.selection_method = _pycomposite.SelectionMethod_HEIGHT

  generator.time = args["time"]
  generator.date = args["date"]
  generator.gain = GAIN
  generator.offset = OFFSET

  result = generator.nearest(pyarea, qfields)
  
  # Fix so that we get a valid place for /what/source and /how/nodes 
  plc = result.source
  result.source = "%s,CMT:%s"%(CENTER_ID,plc)
  result.addAttribute('how/nodes', nodes)
  
  fileno, outfile = rave_tempfile.mktemp(suffix='.h5', close="True")
  
  ios = _raveio.new()
  ios.object = result
  ios.filename = outfile
  ios.save()
  
  return outfile
  
  
